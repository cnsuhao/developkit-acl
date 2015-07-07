#include "stdafx.h"
#include "rfc1035.h"
#include "configure.h"
#include "service_main.h"

#define UDP_READ_NONE	1

typedef struct UDP_CTX 
{
	SERVICE *service;
	char  remote_ip[32];
	short remote_port;
	char  local_ip[32];
	short local_port;

	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;

	// 临时变量用于传递参数
	struct sockaddr_in client_addr;
	int   client_addr_len;

	ACL_ASTREAM *stream_request;
	ACL_ASTREAM *stream_respond;
} UDP_CTX;

static int udp_read(ACL_SOCKET fd, void *buf, size_t size,
	struct sockaddr_in *from_addr, int *from_addr_len)
{
	int   ret;

#ifdef ACL_UNIX
	ret = recvfrom(fd, buf, size, 0, (struct sockaddr*) from_addr,
		(socklen_t*) from_addr_len);
#else
	ret = recvfrom(fd, (char*) buf, (int) size, 0,
		(struct sockaddr*) from_addr, from_addr_len);
#endif

	return (ret);
}

static int udp_write(ACL_SOCKET fd, const void *buf, size_t size,
	struct sockaddr_in *to_addr, int to_addr_len)
{
	int   ret;

#ifdef ACL_UNIX
	ret = sendto(fd, buf, size, 0,
		(struct sockaddr*) to_addr, to_addr_len);
#else
	ret = sendto(fd, (const char*) buf, (int) size, 0,
		(struct sockaddr*) to_addr, to_addr_len);
#endif

	return (ret);
}

static int request_read_fn(ACL_SOCKET fd, void *buf, size_t size,
	int timeout acl_unused, ACL_VSTREAM *fp acl_unused, void *arg)
{
	const char *myname = "request_read_fn";
	UDP_CTX *ctx = (UDP_CTX*) arg;
	int   ret;

	ctx->client_addr_len = sizeof(ctx->client_addr);
	ret = udp_read(fd, buf, size, &ctx->client_addr,
		&ctx->client_addr_len);
	if (ret < 0) {
		acl_msg_error("%s(%d): recvfrom error(%s)",
			myname, __LINE__, acl_last_serror());
		return (UDP_READ_NONE);
	}
	return (ret);
}

static int respond_read_fn(ACL_SOCKET fd, void *buf, size_t size,
	int timeout acl_unused, ACL_VSTREAM *fp acl_unused, void *arg)
{
	const char *myname = "respond_read_fn";
	UDP_CTX *ctx = (UDP_CTX*) arg;
	struct sockaddr_in server_addr;
	int   ret, addr_len = sizeof(server_addr);

	ret = udp_read(fd, buf, size, &server_addr, &addr_len);
	if (ret < 0) {
		acl_msg_error("%s(%d): recvfrom error(%s)",
			myname, __LINE__, acl_last_serror());
	}
	if (server_addr.sin_addr.s_addr != ctx->remote_addr.sin_addr.s_addr) {
		char  ip[32];

		acl_inet_ntoa(server_addr.sin_addr, ip, sizeof(ip));
		acl_msg_warn("%s(%d): invalid addr(%s) from server",
			myname, __LINE__, ip);
		return (UDP_READ_NONE);
	}
	return (ret);
}

static int udp_write_fn(ACL_SOCKET fd acl_unused, const void *buf acl_unused,
	size_t size acl_unused, int timeout acl_unused,
	ACL_VSTREAM *fp acl_unused, void *arg acl_unused)
{
	const char *myname = "udp_write_fn";

	acl_msg_fatal("%s(%d): not support!", myname, __LINE__);
	return (-1);
}

#if 0
static UDP_CTX *udp_ctx_new(const char *remote_ip, short remote_port,
	const char *local_ip, short local_port)
{
	UDP_CTX *ctx = (UDP_CTX*) acl_mycalloc(1, sizeof(UDP_CTX));

	ACL_SAFE_STRNCPY(ctx->remote_ip, remote_ip, sizeof(ctx->remote_ip));
	ctx->remote_port = remote_port;
	ACL_SAFE_STRNCPY(ctx->local_ip, local_ip, sizeof(ctx->local_ip));
	ctx->local_port = local_port;
	return (ctx);
}
#endif

static ACL_VSTREAM *stream_udp_open(void)
{
	const char *myname = "stream_udp_open";
	ACL_VSTREAM *stream;
	ACL_SOCKET  fd;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == ACL_SOCKET_INVALID)
		acl_msg_fatal("%s(%d): socket create error", myname, __LINE__);

	stream = acl_vstream_fdopen(fd, O_RDWR, 1024, 0, ACL_VSTREAM_TYPE_SOCK);
	return (stream);
}

static ACL_VSTREAM *stream_udp_bind(struct sockaddr_in addr)
{
	const char *myname = "stream_udp_bind";
	ACL_VSTREAM *stream;
	ACL_SOCKET fd;

	stream = stream_udp_open();
	fd = ACL_VSTREAM_SOCK(stream);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		acl_msg_fatal("%s(%d): can't bind", myname, __LINE__);
	}
	return (stream);
}

static ACL_ARGV *build_ip_list(DOMAIN_MAP *domain_map)
{
	ACL_ARGV *argv = acl_argv_alloc(10);
	ACL_ARGV *ip_list = domain_map->ip_list;
	int   i, n;

	n = ip_list->argc;
	if (domain_map->idx >= n)
		domain_map->idx = 0;
	i = domain_map->idx++;
	while (n-- > 0) {
		acl_argv_add(argv, ip_list->argv[i++], NULL);
		if (i >= ip_list->argc)
			i = 0;
	}

	return (argv);
}

static void reply_client_local(ACL_SOCKET fd, DOMAIN_MAP *domain_map,
	SERVICE_CTX *service_ctx)
{
	char  respond_buf[MAX_BUF];
	ACL_ARGV *ip_list;
	int   dlen;

	ip_list = build_ip_list(domain_map);
	dlen = rfc1035BuildAReply(domain_map->domain, ip_list,
		service_ctx->domain_root, var_cfg_dns_ip,
		service_ctx->id_original, respond_buf, sizeof(respond_buf));

	acl_argv_free(ip_list);

	(void) udp_write(fd, respond_buf, dlen, &service_ctx->client_addr,
		service_ctx->client_addr_len);
}

static void reply_client(ACL_SOCKET fd, char *buf, int dlen,
	SERVICE_CTX *service_ctx)
{
	const char *myname = "reply_client";
	rfc1035_message *msg;
	char  respond_buf[MAX_BUF];
	int   ret;

	// 先备份源数据
	memcpy(respond_buf, buf, dlen);

	ret = rfc1035MessageUnpack(buf, dlen, &msg);
	if (service_ctx->qtype == RFC1035_TYPE_A && ret == -3) {
		const char *domain_query = service_ctx->domain[0]
			? service_ctx->domain : NULL;
		DOMAIN_MAP *domain_map= domain_map_unknown();
		ACL_ARGV *ip_list;

		ip_list = build_ip_list(domain_map);
		acl_msg_error("%s(%d): rfc1035MessageUnpack error(%d)",
			myname, __LINE__, ret);
		dlen = rfc1035BuildAReply(
			domain_query ? domain_query : "unknown",
			ip_list,
			service_ctx->domain_root,
			var_cfg_dns_ip,
			service_ctx->id_original,
			respond_buf,
			sizeof(respond_buf));
		acl_argv_free(ip_list);

		acl_msg_info("%s(%d): respond my pkt: restore id=%d",
			myname, __LINE__, service_ctx->id_original);
		(void) udp_write(fd, respond_buf, dlen,
			&service_ctx->client_addr,
			service_ctx->client_addr_len);
	} else {
		// 恢复原ID, 需要转换成网络字节序
		unsigned short id_original;

		id_original = htons(service_ctx->id_original);
		memcpy(respond_buf, &id_original, 2);
		acl_msg_info("%s(%d): reply to client, dlen=%d, id=%d, "
			"ret=%d, domain(%s)", myname, __LINE__, dlen,
			id_original, ret,
			msg->query ? msg->query->name : "unknown");
		rfc1035MessageDestroy(msg);
		(void) udp_write(fd, respond_buf, dlen,
			&service_ctx->client_addr,
			service_ctx->client_addr_len);
	}
}

static int read_respond_callback(ACL_ASTREAM *astream, void *context,
	 char *data, int len)
{
	const char *myname = "read_respond_callback";
	UDP_CTX *ctx = (UDP_CTX*) context;
	SERVICE *service = ctx->service;
	ACL_ASTREAM *client = ctx->stream_request;
	ACL_VSTREAM *client_stream = acl_aio_vstream(client);
	SERVICE_CTX *service_ctx;
	unsigned short id;
	char  respond_buf[MAX_BUF];

	if (len == UDP_READ_NONE) {
		acl_aio_read(astream);
		return (0);
	}

	memcpy(&id, data, 2);
	id = ntohs(id);

	service_ctx = service_ctx_find(service, SERVICE_CTX_UDP_REQUEST, id);
	if (service_ctx == NULL) {
		char  key[KEY_LEN];

		create_key(key, sizeof(key), SERVICE_CTX_UDP_REQUEST, id);
		acl_msg_warn("%s(%d): not found id(%s)",
			myname, __LINE__, key);
		acl_aio_read(astream);
		return (0);
	}

	len = len > MAX_BUF ? MAX_BUF : len;
	memcpy(respond_buf, data, len);

	reply_client(ACL_VSTREAM_SOCK(client_stream), respond_buf,
		len, service_ctx);

	service_ctx_free(service_ctx);

	acl_aio_read(astream);
	return (0);
}

static void parse_query(const rfc1035_query *query, SERVICE_CTX *service_ctx)
{
	ACL_ARGV *argv;

	snprintf(service_ctx->domain, sizeof(service_ctx->domain),
		"%s", query->name);
	argv = acl_argv_split(service_ctx->domain, ".");
	if (argv->argc >= 2) {
		int   i, k, size = sizeof(service_ctx->domain_root), n;
		char *ptr = service_ctx->domain_root;

		k = argv->argc - 2;
		for (i = k; i < argv->argc; i++) {
			if (i > k) {
				*ptr++ = '.';
				size--;
				if (size <= 0)
					break;
			}
			n = (int) strlen(argv->argv[i]);
			ACL_SAFE_STRNCPY(ptr, argv->argv[i], size);
			size -= n;
			if (size <= 0)
				break;
			ptr += n;
		}
	} else {
		ACL_SAFE_STRNCPY(service_ctx->domain_root,
			var_cfg_dns_name, sizeof(service_ctx->domain_root));
	}
	acl_argv_free(argv);

	service_ctx->qtype = query->qtype;
	acl_msg_info("type=%d, class=%d", query->qtype, query->qclass);
}

static int read_request_callback(ACL_ASTREAM *astream, void *context,
	char *data, int len)
{
	UDP_CTX *ctx = (UDP_CTX*) context;
	SERVICE *service = ctx->service;
	ACL_ASTREAM *server = ctx->stream_respond;
	ACL_VSTREAM *server_stream = acl_aio_vstream(server);
	ACL_VSTREAM *client_stream = acl_aio_vstream(astream);
	SERVICE_CTX *service_ctx;
	rfc1035_message *msg;
	unsigned short id;
	int  ret;

	if (len == UDP_READ_NONE) {
		acl_aio_read(astream);
		return (0);
	}

	service_ctx = service_ctx_new(ctx->service, astream,
		SERVICE_CTX_UDP_REQUEST, service->curr_id++);

	if (service->curr_id == (unsigned short) -1)
		service->curr_id = 0;

	ret = rfc1035MessageUnpack(data, len, &msg);
	if (ret >= 0) {
		if (msg->query)
			parse_query(msg->query, service_ctx);

		rfc1035MessageDestroy(msg);
	}

	memcpy(&service_ctx->client_addr, &ctx->client_addr,
		ctx->client_addr_len);
	service_ctx->client_addr_len = ctx->client_addr_len;
	// 备份原ID , 且以主机字节序存储
	memcpy(&service_ctx->id_original, data, 2);
	service_ctx->id_original = ntohs(service_ctx->id_original);
	acl_msg_info("id_original=%d", service_ctx->id_original);

	if (service_ctx->qtype == RFC1035_TYPE_A) {
		DOMAIN_MAP *domain_map;

		domain_map = domain_map_find(service_ctx->domain);
		if (domain_map) {
			reply_client_local(
				ACL_VSTREAM_SOCK(client_stream),
				domain_map,
				service_ctx);
			return (0);
		}
	}

	service_ctx->request_len = len > MAX_BUF ? MAX_BUF : len;
	memcpy(service_ctx->request_buf, data, len);
	id = htons(service_ctx->id);
	memcpy(service_ctx->request_buf, &id, 2);

	acl_msg_info("request one key=%s, request_len=%d, len=%d",
		service_ctx->key, service_ctx->request_len, len);

	(void) udp_write(ACL_VSTREAM_SOCK(server_stream),
		service_ctx->request_buf, service_ctx->request_len,
		&ctx->remote_addr, sizeof(ctx->remote_addr));

	acl_aio_read(astream);
	return (0);
}

void service_udp_init(SERVICE *service, const char *local_ip,
	int local_port, const char *remote_ip, int remote_port)
{
	UDP_CTX *ctx = (UDP_CTX*) acl_mycalloc(1, sizeof(UDP_CTX));
	struct sockaddr_in addr;
	ACL_VSTREAM *stream;

	ctx->service = service;

	ACL_SAFE_STRNCPY(ctx->local_ip, local_ip, sizeof(ctx->local_ip));
	ctx->local_port = local_port;
	ctx->local_addr.sin_addr.s_addr = inet_addr(local_ip);
	ctx->local_addr.sin_port = htons(local_port);
	ctx->local_addr.sin_family = AF_INET;

	ACL_SAFE_STRNCPY(ctx->remote_ip, remote_ip, sizeof(ctx->remote_ip));
	ctx->remote_port = remote_port;
	ctx->remote_addr.sin_addr.s_addr = inet_addr(remote_ip);
	ctx->remote_addr.sin_port = htons(remote_port);
	ctx->remote_addr.sin_family = AF_INET;

	// 创建接收客户端请求的流
	stream = stream_udp_bind(ctx->local_addr);
	acl_vstream_ctl(stream,
		ACL_VSTREAM_CTL_READ_FN, request_read_fn,
		ACL_VSTREAM_CTL_WRITE_FN, udp_write_fn,
		ACL_VSTREAM_CTL_CONTEXT, ctx,
		ACL_VSTREAM_CTL_END);

	ctx->stream_request = acl_aio_open(service->aio, stream);
	acl_aio_ctl(ctx->stream_request,
		ACL_AIO_CTL_READ_HOOK_ADD, read_request_callback, ctx,
		ACL_AIO_CTL_CTX, ctx,
		ACL_AIO_CTL_END);
	acl_aio_read(ctx->stream_request);

	// 创建接收服务端响应的流
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;

	stream = stream_udp_bind(addr);
	acl_vstream_ctl(stream,
		ACL_VSTREAM_CTL_READ_FN, respond_read_fn,
		ACL_VSTREAM_CTL_WRITE_FN, udp_write_fn,
		ACL_VSTREAM_CTL_CONTEXT, ctx,
		ACL_VSTREAM_CTL_END);

	ctx->stream_respond = acl_aio_open(service->aio, stream);
	acl_aio_ctl(ctx->stream_respond,
		ACL_AIO_CTL_READ_HOOK_ADD, read_respond_callback, ctx,
		ACL_AIO_CTL_CTX, ctx,
		ACL_AIO_CTL_END);
	acl_aio_read(ctx->stream_respond);
}
