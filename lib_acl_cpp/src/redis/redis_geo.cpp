#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/stdlib/dbuf_pool.hpp"
#include "acl_cpp/redis/redis_geo.hpp"

namespace acl
{

#define BUFLEN	32

/////////////////////////////////////////////////////////////////////////////

geo_pos::geo_pos(const char* name)
: name_(name)
{
	dist_ = -1;
	hash_ = -1;
	longitude_ = GEO_INVALID_POS;
	latitude_ = GEO_INVALID_POS;
}

geo_pos::geo_pos(const geo_pos& pos)
{
	name_ = pos.name_;
	dist_ = pos.dist_;
	hash_ = pos.hash_;
	longitude_ = pos.longitude_;
	latitude_ = pos.latitude_;
}

geo_pos::~geo_pos()
{
}

void geo_pos::set_dist(double dist)
{
	dist_ = dist;
}

void geo_pos::set_hash(acl_int64 hash)
{
	hash_ = hash;
}

void geo_pos::set_longitude(double longitude)
{
	longitude_ = longitude;
}

void geo_pos::set_latitude(double latitude)
{
	latitude_ = latitude;
}

/////////////////////////////////////////////////////////////////////////////

redis_geo::redis_geo()
: redis_command(NULL)
{
}

redis_geo::redis_geo(redis_client* conn)
: redis_command(conn)
{
}

redis_geo::redis_geo(redis_client_cluster* cluster, size_t max_conns)
: redis_command(cluster, max_conns)
{
}

redis_geo::~redis_geo()
{
}

int redis_geo::geoadd(const char* key, const char* member,
	double longitude, double latitude)
{
	size_t argc = 5;
	const char* argv[5];
	size_t lens[5];

	argv[0] = "GEOADD";
	lens[0] = sizeof("GEOADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	char* buf = (char*) pool_->dbuf_alloc(BUFLEN);
	safe_snprintf(buf, BUFLEN, "%.8f", longitude);
	argv[2] = buf;
	lens[2] = strlen(buf);

	buf = (char*) pool_->dbuf_alloc(BUFLEN);
	safe_snprintf(buf, BUFLEN, "%.8f", latitude);
	argv[3] = buf;
	lens[3] = strlen(buf);

	argv[4] = member;
	lens[4] = strlen(member);

	hash_slot(key);
	build_request(argc, argv, lens);
	return get_number();
}

int redis_geo::geoadd(const char* key, size_t size, const char* members[],
	const double longitudes[], const double latitudes[])
{
	size_t argc = 2 + 3 * size;
	const char** argv = (const char**)
		pool_->dbuf_alloc(argc * sizeof(char*));
	size_t *lens = (size_t*) pool_->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "GEOADD";
	lens[0] = sizeof("GEOADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	for (size_t i = 0, n = 2; i < size; i++)
	{
		char* buf = (char*) pool_->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", longitudes[i]);
		argv[n] = buf;
		lens[n] = strlen(argv[n]);
		n++;

		buf = (char*) pool_->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", latitudes[i]);
		argv[n] = buf;
		lens[n] = strlen(argv[n]);
		n++;

		argv[n] = members[i];
		lens[n] = strlen(argv[n]);
		n++;
	}

	hash_slot(key);
	build_request(argc, argv, lens);
	return get_number();
}

int redis_geo::geoadd(const char* key, const std::vector<string>& members,
	const std::vector<double>& longitudes,
	const std::vector<double>& latitudes)
{
	if (members.empty())
	{
		logger_error("members empty");
		return -1;
	}
	if (members.size() != longitudes.size())
	{
		logger_error("longitudes's size(%d) != members's size(%d)",
			(int) longitudes.size(), (int) members.size());
		return -1;
	}
	if (latitudes.size() != longitudes.size())
	{
		logger_error("latitudes's size(%d) != longitudes's size(%d)",
			(int) latitudes.size(), (int) longitudes.size());
		return -1;
	}

	size_t argc = 2 + 3 * members.size();
	const char** argv = (const char**)
		pool_->dbuf_alloc(argc * sizeof(char*));
	size_t *lens = (size_t*) pool_->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "GEOADD";
	lens[0] = sizeof("GEOADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	size_t size = members.size();
	for (size_t i = 0, n = 2; i < size; i++)
	{
		char* buf = (char*) pool_->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", longitudes[i]);
		argv[n] = buf;
		lens[n] = strlen(argv[n]);
		n++;

		buf = (char*) pool_->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", latitudes[i]);
		argv[n] = buf;
		lens[n] = strlen(argv[n]);
		n++;

		argv[n] = members[i].c_str();
		lens[n] = members[i].size();
		n++;
	}

	hash_slot(key);
	build_request(argc, argv, lens);
	return get_number();
}

bool redis_geo::geohash(const char* key, const std::vector<string>& members,
	std::vector<string>& results)
{
	hash_slot(key);
	build("GEOHASH", key, members);
	return get_strings(results) < 0 ? false : true;
}

bool redis_geo::geopos(const char* key, const std::vector<string>& members,
	std::vector<std::pair<double, double> >& results)
{
	hash_slot(key);
	build("GEOHASH", key, members);
	const redis_result* result = run();
	if (result == NULL || result->get_type() != REDIS_RESULT_ARRAY)
		return false;
	size_t size;
	const redis_result** children = result->get_children(&size);
	if (children == NULL)
		return false;

	string buf;

	for (size_t i = 0; i < size; i++)
	{
		const redis_result* child = children[i];
		if (child->get_type() != REDIS_RESULT_ARRAY)
		{
			results[i] = std::make_pair(GEO_INVALID_POS,
				GEO_INVALID_POS);
			continue;
		}

		size_t n;
		const redis_result** xy = child->get_children(&n);
		if (xy == NULL || n != 2)
		{
			results[i] = std::make_pair(GEO_INVALID_POS,
				GEO_INVALID_POS);
			continue;
		}
		const redis_result* lo = xy[0], *la = xy[1];
		if (lo->get_type() != REDIS_RESULT_STRING
			|| la->get_type() != REDIS_RESULT_STRING)
		{
			results[i] = std::make_pair(GEO_INVALID_POS,
				GEO_INVALID_POS);
			continue;
		}

		double longitude = lo->get_double();
		if (longitude < -180 || longitude > 180)
		{
			results[i] = std::make_pair(GEO_INVALID_POS,
				GEO_INVALID_POS);
			continue;
		}

		double latitude = la->get_double();
		if (latitude < -85.05112878 || latitude > 85.05112878)
		{
			results[i] = std::make_pair(GEO_INVALID_POS,
				GEO_INVALID_POS);
			continue;
		}
		results[i] = std::make_pair(longitude, latitude);
	}

	return true;
}

double redis_geo::geodist(const char* key, const char* member1,
	const char* member2, int unit /* = GEO_UNIT_M */)
{
	std::vector<string> names;
	names.push_back(member1);
	names.push_back(member2);
	const char* unit_s = get_unit(unit);
	if (unit_s != NULL)
		names.push_back(unit_s);

	build("GEODIST", key, names);

	string buf;
	if (get_string(buf) == 0)
		return -1;
	return atof(buf.c_str());
}

const std::vector<geo_pos>& redis_geo::georadius(const char* key,
	double longitude, double latitude, double radius,
	int unit /* = GEO_UNIT_M */,
	int with /* = GEO_WITH_COORD | GEO_WITH_DIST */,
	int sort /* = GEO_SORT_ASC */)
{
	positions_.clear();

	const char* argv[9];
	size_t lens[9];
	size_t argc = 0;

	argv[argc] = "GEORADIUS";
	lens[argc] = sizeof("GEORADIUS") - 1;
	argc++;

	argv[argc] = key;
	lens[argc] = strlen(key);
	argc++;

	char* buf = (char*) pool_->dbuf_alloc(BUFLEN);
	safe_snprintf(buf, BUFLEN, "%.8f", longitude);
	argv[argc] = buf;
	lens[argc] = strlen(buf);
	argc++;

	buf = (char*) pool_->dbuf_alloc(BUFLEN);
	safe_snprintf(buf, BUFLEN, "%.8f", latitude);
	argv[argc] = buf;
	lens[argc] = strlen(buf);
	argc++;

	buf = (char*) pool_->dbuf_alloc(BUFLEN);
	safe_snprintf(buf, BUFLEN, "%.8f", radius);
	argv[argc] = buf;
	lens[argc] = strlen(buf);
	argc++;

	const char* unit_s = get_unit(unit);
	if (unit_s == NULL)
		unit_s = "m";
	argv[argc] = unit_s;
	lens[argc] = strlen(unit_s);
	argc++;

	if ((with & GEO_WITH_COORD) != 0)
	{
		argv[argc] = "WITHCOORD";
		lens[argc] = sizeof("WITHCOORD") - 1;
		argc++;
	}
	if ((with & GEO_WITH_DIST) != 0)
	{
		argv[argc] = "WITHDIST";
		lens[argc] = sizeof("WITHDIST") - 1;
		argc++;
	}
	if ((with & GEO_WITH_HASH) != 0)
	{
		argv[argc] = "WITHHASH";
		lens[argc] = sizeof("WITHHASH") - 1;
		argc++;
	}

	if (sort == GEO_SORT_ASC)
	{
		argv[argc] = "ASC";
		lens[argc] = sizeof("ASC") - 1;
		argc++;
	}
	else if (sort == GEO_SORT_DESC)
	{
		argv[argc] = "DESC";
		lens[argc] = sizeof("DESC") - 1;
		argc++;
	}

	hash_slot(key);
	build_request(argc, argv, lens);
	const redis_result* result = run();
	if (result == NULL || result->get_type() != REDIS_RESULT_ARRAY)
		return positions_;
	size_t size;
	const redis_result** children = result->get_children(&size);
	if (children == NULL)
		return positions_;

	for (size_t i = 0; i < size; i++)
		add_one_pos(*children[i]);

	return positions_;
}

const std::vector<geo_pos>& redis_geo::georadiusbymember(const char* key,
	const char* member, double radius,
	int unit /* = GEO_UNIT_M */,
	int with /* = GEO_WITH_COORD | GEO_WITH_DIST */,
	int sort /* = GEO_SORT_ASC */)
{
	positions_.clear();

	const char* argv[8];
	size_t lens[8];
	size_t argc = 0;

	argv[argc] = "GEORADIUSBYMEMBER";
	lens[argc] = sizeof("GEORADIUSBYMEMBER") - 1;
	argc++;

	argv[argc] = key;
	lens[argc] = strlen(key);
	argc++;

	argv[argc] = member;
	lens[argc] = strlen(member);
	argc++;

	char* buf = (char*) pool_->dbuf_alloc(BUFLEN);
	safe_snprintf(buf, BUFLEN, "%.8f", radius);
	argv[argc] = buf;
	lens[argc] = strlen(buf);
	argc++;

	const char* unit_s = get_unit(unit);
	if (unit_s == NULL)
		unit_s = "m";
	argv[argc] = unit_s;
	lens[argc] = strlen(unit_s);
	argc++;

	if ((with & GEO_WITH_COORD) != 0)
	{
		argv[argc] = "WITHCOORD";
		lens[argc] = sizeof("WITHCOORD") - 1;
		argc++;
	}
	if ((with & GEO_WITH_DIST) != 0)
	{
		argv[argc] = "WITHDIST";
		lens[argc] = sizeof("WITHDIST") - 1;
		argc++;
	}
	if ((with & GEO_WITH_HASH) != 0)
	{
		argv[argc] = "WITHHASH";
		lens[argc] = sizeof("WITHHASH") - 1;
		argc++;
	}

	if (sort == GEO_SORT_ASC)
	{
		argv[argc] = "ASC";
		lens[argc] = sizeof("ASC") - 1;
		argc++;
	}
	else if (sort == GEO_SORT_DESC)
	{
		argv[argc] = "DESC";
		lens[argc] = sizeof("DESC") - 1;
		argc++;
	}

	hash_slot(key);
	build_request(argc, argv, lens);
	const redis_result* result = run();
	if (result == NULL || result->get_type() != REDIS_RESULT_ARRAY)
		return positions_;
	size_t size;
	const redis_result** children = result->get_children(&size);
	if (children == NULL)
		return positions_;

	for (size_t i = 0; i < size; i++)
		add_one_pos(*children[i]);

	return positions_;
}

void redis_geo::add_one_pos(const redis_result& rr)
{
	string buf;
	acl::redis_result_t type = rr.get_type();
	if (type == REDIS_RESULT_STRING)
	{
		rr.argv_to_string(buf);
		positions_.push_back(geo_pos(buf.c_str()));
		return;
	}
	if (type != REDIS_RESULT_ARRAY)
		return;

	size_t size;
	const redis_result** children = rr.get_children(&size);
	if (children == NULL || size == 0)
		return;

	if (children[0]->get_type() != REDIS_RESULT_STRING)
		return;
	children[0]->argv_to_string(buf);
	geo_pos pos(buf.c_str());

	for (size_t i = 1; i < size; i++)
	{
		type = children[i]->get_type();
		if (type == REDIS_RESULT_STRING)
			pos.set_dist(children[i]->get_double());
		else if (type == REDIS_RESULT_INTEGER)
		{
			bool ok;
			acl_int64 hash = children[i]->get_integer64(&ok);
			if (ok)
				pos.set_hash(hash);
		}
		else if (type != REDIS_RESULT_ARRAY)
			continue;

		size_t n;
		const redis_result** results = children[i]->get_children(&n);
		if (results != NULL && n == 2
			&& results[0]->get_type() == REDIS_RESULT_STRING
			&& results[1]->get_type() == REDIS_RESULT_STRING)
		{
			pos.set_longitude(results[0]->get_double());
			pos.set_latitude(results[0]->get_double());
		}
	}

	positions_.push_back(pos);
}

/////////////////////////////////////////////////////////////////////////////

typedef struct
{
	int unit;
	const char* str;
} UNIT_MAP;

const char* redis_geo::get_unit(int unit)
{
	static const UNIT_MAP _map[] = {
		{ GEO_UNIT_FT, "ft" },
		{ GEO_UNIT_M, "m" },
		{ GEO_UNIT_MI, "mi" },
		{ GEO_UNIT_KM, "km" },
	};

	if (unit < GEO_UNIT_FT || unit > GEO_UNIT_KM)
		return NULL;
	return _map[unit].str;
}

} // namespace acl
