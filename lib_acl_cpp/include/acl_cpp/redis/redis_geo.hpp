#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <map>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

#define GEO_INVALID_POS	360

enum
{
	GEO_UNIT_FT,
	GEO_UNIT_M,
	GEO_UNIT_MI,
	GEO_UNIT_KM,
};

enum
{
	GEO_WITH_COORD = 1 << 0,
	GEO_WITH_DIST  = 1 << 1,
	GEO_WITH_HASH  = 1 << 2,
};

enum
{
	GEO_SORT_NONE,
	GEO_SORT_ASC,
	GEO_SORT_DESC,
};

class ACL_CPP_API geo_pos
{
public:
	geo_pos(const char* name);
	geo_pos(const geo_pos& pos);
	~geo_pos();

	void set_name(const char* name);
	const char* get_name() const
	{
		return name_.c_str();
	}

	void set_dist(double dist);
	double get_dist() const
	{
		return dist_;
	}

#if defined(_WIN32) || defined(_WIN64)
	void set_hash(__int64 hash);
	__int64 get_hash() const
#else
	void set_hash(long long int hash);
	long long int get_hash() const
#endif // defined(_WIN32) || defined(_WIN64)
	{
		return hash_;
	}

	void set_longitude(double longitude);
	double get_longitude() const
	{
		return longitude_;
	}

	void set_latitude(double latitude);
	double get_latitude() const
	{
		return latitude_;
	}

private:
	string name_;
	double dist_;
#if defined(_WIN32) || defined(_WIN64)
	__int64 hash_;
#else
	long long int hash_;
#endif // defined(_WIN32) || defined(_WIN64)

	double longitude_;
	double latitude_;
};

class redis_client;

class ACL_CPP_API redis_geo : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_geo();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_geo(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_geo(redis_client_cluster* cluster, size_t max_conns);
	virtual ~redis_geo();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���һ��ָ���ĵ���λ��������ָ���� key ��
	 * Add the specified geospatial item (latitude, logitude, name)
	 * to the specified key.
	 * @param key {const char*} ��Ӧ�ļ�ֵ
	 *  the specified key
	 * @param member {const char*} �õ�������ı�ʶ��
	 *  the geospatial's identifier
	 * @param loginitude {double} ����
	 *  the geospatial's loginitude
	 * @param latitude {double} γ��
	 *  the geospatial's latitude
	 * @return {int} 1����ӳɹ���0���õ��������ʶ���Ѵ��ڣ���ʹ����ֵ�������޸ģ�
	 *  Ҳ������ 0��-1����ʾ����
	 *  the return value as below:
	 *  1: add one new member successfully
	 *  0: the member already existed, and the geospatial may be changed
	 * -1: some erro happened
	 */
	int geoadd(const char* key, const char* member,
		double longitude, double latitude);

	/**
	 * ��ָ�� key ���һ���ַλ����������
	 * Add the specified geospatial items (latitude, logitude, name)
	 * to the specified key.
	 * @param key {const char*} ��Ӧ�ļ�ֵ
	 *  the specified key
	 * @param size {size_t} ����ĳ���
	 *  the array's size
	 * @param memebers {const char* []} ��Ա���飬�䳤���� size ָ��
	 *  the members array, which's length was specified by size parameter
	 * @param longitudes {const double[]} �����������飬�䳤���� size ָ��
	 *  the logintitudes array, which's length was specifed by size parameter
	 * @param latitudes {const double[]} γ���������飬�䳤���� size ָ��
	 *  the lattitudes array, which's length was specifed by size parameter
	 * @return {int} ��ӳɹ��ĳ�Ա����������ֵ�������£�
	 *  return the successfully added members's count:
	 *  > 0: ��ʾ�ɹ���ӵĳ�Ա������
	 *       represent the successfully added memebers's count
	 *    0: ��Щ��Ա���Ѿ�����
	 *       the members's belong the key already existing
	 *   -1: ��ʾ��������ͨ�� result_error �����鿴����ԭ��
	 *       some error happened, the result_error function can be used
	 *       to find the error's reason
	 */
	int geoadd(const char* key, size_t size, const char* memebers[],
		const double longitudes[], const double latitudes[]);

	/**
	 * ��ָ�� key ���һ���ַλ����������
	 * Add the specified geospatial items (latitude, logitude, name)
	 * to the specified key.
	 * @param key {const char*} ��Ӧ�ļ�ֵ
	 *  the specified key
	 * @param memebers {const std::vector<string>&} ��Ա����
	 *  the members array
	 * @param longitudes {const std::vector<double>&} ������������
	 *  the logintitudes array
	 * @param latitudes {const std::vector<double>&} γ����������
	 *  the lattitudes array
	 * @return {int} ��ӳɹ��ĳ�Ա����������ֵ�������£�
	 *  return the successfully added members's count:
	 *  > 0: ��ʾ�ɹ���ӵĳ�Ա������
	 *       represent the successfully added memebers's count
	 *    0: ��Щ��Ա���Ѿ�����
	 *       the members's belong the key already existing
	 *   -1: ��ʾ��������ͨ�� result_error �����鿴����ԭ��
	 *       some error happened, the result_error function can be used
	 *       to find the error's reason
	 *  ע�⣺��������(members, longitudes, latitudes)�����鳤�ȱ������
	 *  Notice: the three array's length must be equal between members,
	 *    longitudes and latitudes
	 */
	int geoadd(const char* key, const std::vector<string>& members,
		const std::vector<double>& longitudes,
		const std::vector<double>& latitudes);

	bool geohash(const char* key, const std::vector<string>& members,
		std::vector<string>& results);
	bool geopos(const char* key, const std::vector<string>& members,
		std::vector<std::pair<double, double> >& results);

	double geodist(const char* key, const char* member1,
		const char* member2, int unit = GEO_UNIT_M);
	const std::vector<geo_pos>& georadius(const char* key,
		double longitude, double latitude, double radius,
		int unit = GEO_UNIT_M,
		int with = GEO_WITH_COORD | GEO_WITH_DIST,
		int sort = GEO_SORT_ASC);
	const std::vector<geo_pos>& georadiusbymember(const char* key,
		const char* member, double radius,
		int unit = GEO_UNIT_M,
		int with = GEO_WITH_COORD | GEO_WITH_DIST,
		int sort = GEO_SORT_ASC);

private:
	std::vector<geo_pos> positions_;

	void add_one_pos(const redis_result& rr);

	static const char* get_unit(int unit);
};

} // namespace acl
