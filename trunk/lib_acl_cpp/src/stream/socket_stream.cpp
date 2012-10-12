#include "acl_stdafx.hpp"
#include "log.hpp"
#include "socket_stream.hpp"

namespace acl {

	socket_stream::socket_stream()
	{
		dummy[0] = 0;
	}

	socket_stream::~socket_stream()
	{
		close();
	}

	bool socket_stream::open(ACL_SOCKET fd)
	{
		ACL_VSTREAM* conn = acl_vstream_fdopen(fd, O_RDWR,
			8192, 0, ACL_VSTREAM_TYPE_SOCK);
		acl_assert(conn);
		return (open(conn));
	}

	bool socket_stream::open(const char* addr, int conn_timeout, int rw_timeout)
	{
		ACL_VSTREAM* conn = acl_vstream_connect(addr, ACL_BLOCKING,
			conn_timeout, rw_timeout, 8192);
		if (conn == NULL)
			return (false);

		return (open(conn));
	}

	bool socket_stream::open(ACL_VSTREAM* vstream)
	{
		// 先关闭旧的流对象
		if (m_pStream)
			acl_vstream_close(m_pStream);
		m_pStream = vstream;
		m_bEof = false;
		m_bOpened = true;
		return (true);
	}

	bool socket_stream::close()
	{
		if (m_bOpened == false)
			return (false);
		if (m_pStream == NULL)
			return (true);

		m_bEof = true;
		m_bOpened = false;

		int   ret = acl_vstream_close(m_pStream);
		m_pStream = NULL;

		return (ret == 0 ? true : false);
	}

	ACL_SOCKET socket_stream::sock_handle() const
	{
		if (m_pStream == NULL)
			return (ACL_SOCKET_INVALID);
		return (ACL_VSTREAM_SOCK(m_pStream));
	}

	ACL_SOCKET socket_stream::unbind_sock()
	{
		if (m_pStream == NULL)
			return (ACL_SOCKET_INVALID);
		ACL_SOCKET sock = ACL_VSTREAM_SOCK(m_pStream);
		m_pStream->fd.sock = ACL_SOCKET_INVALID;
		m_bEof = true;
		m_bOpened = false;
		return (sock);
	}

	const char* socket_stream::get_peer() const
	{
		if (m_pStream == NULL)
			return (dummy);
		return (ACL_VSTREAM_PEER(m_pStream));
	}

	const char* socket_stream::get_local() const
	{
		if (m_pStream == NULL)
			return (dummy);
		// xxx: acl_vstream 中没有对此地址赋值
		if (m_pStream->local_addr[0] == 0)
		{
			(void) acl_getsockname(ACL_VSTREAM_SOCK(m_pStream),
				m_pStream->local_addr,
				sizeof(m_pStream->local_addr));
		}
		return (ACL_VSTREAM_LOCAL(m_pStream));
	}

} // namespace acl
