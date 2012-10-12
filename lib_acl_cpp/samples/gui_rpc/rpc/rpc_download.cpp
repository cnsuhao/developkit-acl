#include "stdafx.h"
#include "resource.h"
#include "gui_rpcDlg.h"
#include "rpc_download.h"

typedef enum
{
	CTX_T_REQ_HDR,
	CTX_T_RES_HDR,
	CTX_T_CONTENT_LENGTH,
	CTX_T_PARTIAL_LENGTH,
	CTX_T_END
} ctx_t;

struct DOWN_CTX 
{
	ctx_t type;
	int length;
};

static double stamp_sub(const struct timeval *from, const struct timeval *sub_by)
{
	struct timeval res;

	memcpy(&res, from, sizeof(struct timeval));

	res.tv_usec -= sub_by->tv_usec;
	if (res.tv_usec < 0)
	{
		--res.tv_sec;
		res.tv_usec += 1000000;
	}

	res.tv_sec -= sub_by->tv_sec;
	return (res.tv_sec * 1000.0 + res.tv_usec/1000.0);
}

using namespace acl;

// 子线程处理函数
void rpc_download::rpc_run()
{
	//http_request req(addr_, 60, 60, false);  // HTTP 请求对象
	http_request req(addr_);  // HTTP 请求对象
	// 设置 HTTP 请求头信息
	req.request_header().set_url(url_.c_str())
		.set_content_type("text/html")
		.set_host(addr_.c_str())
		.set_method(HTTP_METHOD_GET);

	// 通知主线程 HTTP 请求头数据

	req.request_header().build_request(req_hdr_);
	DOWN_CTX* ctx = new DOWN_CTX;
	ctx->type = CTX_T_REQ_HDR;
	rpc_signal(ctx);

	struct timeval begin, end;;
	gettimeofday(&begin, NULL);

	// 发送 HTTP 请求数据
	if (req.request(NULL, 0) == false)
	{
		printf("send request error\r\n");
		error_ = false;
		gettimeofday(&end, NULL);
		total_spent_ = stamp_sub(&end, &begin);
		return;
	}

	// 获得 HTTP 请求的连接对象
	http_client* conn = req.get_client();
	ASSERT(conn);

	// 通知主线程 HTTP 响应头数据

	(void) conn->get_respond_head(&res_hdr_);
	ctx = new DOWN_CTX;
	ctx->type = CTX_T_RES_HDR;
	rpc_signal(ctx);

	// 通知主线程 HTTP 响应体数据长度

	ctx = new DOWN_CTX;
	ctx->type = CTX_T_CONTENT_LENGTH;
	ctx->length = (int) conn->body_length();  // 获得 HTTP 响应数据的数据体长度
	content_length_ = ctx->length;
	rpc_signal(ctx);

	string buf(8192);
	int   real_size;
	while (true)
	{
		// 读 HTTP 响应数据体
		int ret = req.read_body(buf, true, &real_size);
		if (ret <= 0)
		{
			ctx = new DOWN_CTX;
			ctx->type = CTX_T_END;
			ctx->length = ret;
			// 通知主线程
			rpc_signal(ctx);
			break;
		}
		ctx = new DOWN_CTX;
		ctx->type = CTX_T_PARTIAL_LENGTH;
		ctx->length = real_size;
		// 通知主线程
		rpc_signal(ctx);
	}

	gettimeofday(&end, NULL);
	total_spent_ = stamp_sub(&end, &begin);
}

// 主线程处理过程，收到子线程任务完成的消息
void rpc_download::rpc_onover()
{
	m_dlg.DownloadOver(total_read_, total_spent_);
	delete this;
}

// 主线程处理过程，收到子线程的通知消息
void rpc_download::rpc_wakeup(void* ctx)
{
	DOWN_CTX* down_ctx = (DOWN_CTX*) ctx;
	switch (down_ctx->type)
	{
	case CTX_T_REQ_HDR:
		m_dlg.SetRequestHdr(req_hdr_.c_str());
		break;
	case CTX_T_RES_HDR:
		m_dlg.SetResponseHdr(res_hdr_.c_str());
		break;
	case CTX_T_CONTENT_LENGTH:
		break;
	case CTX_T_PARTIAL_LENGTH:
		total_read_ += down_ctx->length;
		m_dlg.OnDownload(content_length_, total_read_);
		break;
	case CTX_T_END:
		printf("%s: read over\r\n", addr_.c_str());
		break;
	default:
		printf("%s: ERROR\r\n", addr_.c_str());
		break;
	}
	delete down_ctx;
}