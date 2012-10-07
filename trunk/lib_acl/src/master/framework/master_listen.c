/* System library. */
#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#endif

#ifdef ACL_UNIX

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/* Utility library. */

#include "stdlib/acl_msg.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_stringops.h"
#include "stdlib/unix/acl_set_eugid.h"
#include "stdlib/unix/acl_set_ugid.h"
#include "stdlib/acl_iostuff.h"
#include "stdlib/acl_vstream.h"

#include "net/acl_listen.h"
#include "net/acl_tcp_ctl.h"

/* Application-specific. */

#include "../master_params.h"
#include "master.h"

/* acl_master_listen_init - enable connection requests */

void    acl_master_listen_init(ACL_MASTER_SERV *serv)
{
	char   *myname = "acl_master_listen_init";
	char   *end_point;
	const char *ptr;
	int     n, qlen;

	qlen = serv->max_proc > acl_var_master_proc_limit ? serv->max_proc : acl_var_master_proc_limit;
	if (qlen < 128) {
		acl_msg_warn("%s(%d): qlen(%d) too small, use 128 now", myname, __LINE__, qlen);
		qlen = 128;
	}

	/*
	 * Find out what transport we should use, then create one or
	 * more listener sockets. Make the listener sockets non-blocking,
	 * so that child processes don't block in accept() when multiple
	 * processes are selecting on the same socket and only one of
	 * them gets the connection.
	 */
	switch (serv->type) {

	/*
	 * UNIX-domain or stream listener endpoints always come as singlets.
	 */
	case ACL_MASTER_SERV_TYPE_UNIX:
		acl_set_eugid(acl_var_master_owner_uid, acl_var_master_owner_gid);
		serv->listen_fds[0] = acl_unix_listen(serv->name, qlen, ACL_NON_BLOCKING);
		if (serv->listen_fds[0] == ACL_SOCKET_INVALID)
			acl_msg_fatal("%s(%d)->%s: listen on addr(%s) error(%s)",
				__FILE__, __LINE__, myname, serv->name, strerror(errno));

		serv->listen_streams[0] = acl_vstream_fdopen(serv->listen_fds[0],
						O_RDONLY,
						acl_var_master_buf_size,
						acl_var_master_rw_timeout,
						ACL_VSTREAM_TYPE_LISTEN_UNIX);
		acl_close_on_exec(serv->listen_fds[0], ACL_CLOSE_ON_EXEC);
		acl_set_ugid(getuid(), getgid());
		acl_msg_info("%s(%d)->%s: success listen on domain socket: %s, qlen: %d",
			__FILE__, __LINE__, myname, serv->name, qlen);
		break;

	/*
	 * FIFO listener endpoints always come as singlets.
	 */
	case ACL_MASTER_SERV_TYPE_FIFO:
		acl_set_eugid(acl_var_master_owner_uid, acl_var_master_owner_gid);
		serv->listen_fds[0] = acl_fifo_listen(serv->name, 0622, ACL_NON_BLOCKING);
		if (serv->listen_fds[0] == ACL_SOCKET_INVALID)
			acl_msg_fatal("%s(%d)->%s: acl_fifo_listen on name(%s) error(%s)",
				__FILE__, __LINE__, myname, serv->name, strerror(errno));

		serv->listen_streams[0] = acl_vstream_fdopen(serv->listen_fds[0],
						O_RDONLY,
						acl_var_master_buf_size,
						acl_var_master_rw_timeout,
						ACL_VSTREAM_TYPE_LISTEN);
		acl_close_on_exec(serv->listen_fds[0], ACL_CLOSE_ON_EXEC);
		acl_set_ugid(getuid(), getgid());
		acl_msg_info("%s(%d)->%s: success listen on fifo socket: %s, qlen: %d",
			__FILE__, __LINE__, myname, serv->name, qlen);
		break;

	/*
	 * INET-domain listener endpoints can be wildcarded (the default) or
	 * bound to specific interface addresses.
	 * 
	 * With dual-stack IPv4/6 systems it does not matter, we have to specify
	 * the addresses anyway, either explicit or wild-card.
	 */
	case ACL_MASTER_SERV_TYPE_INET:
		for (n = 0; n < serv->listen_fd_count; n++) {
			if (ACL_MASTER_INET_ADDRLIST(serv) == NULL)
				end_point = acl_concatenate(":", ACL_MASTER_INET_PORT(serv), (char *) 0);
			else {
				ptr = acl_netdb_index_ip(ACL_MASTER_INET_ADDRLIST(serv), 0);
				if (ptr == NULL)
					acl_msg_fatal("%s(%d): null listen addr", myname, __LINE__);
				end_point = acl_concatenate(ptr, ":", ACL_MASTER_INET_PORT(serv), (char *) 0);
			}

			serv->listen_fds[n] = acl_inet_listen(end_point, qlen, ACL_NON_BLOCKING);
			if (serv->listen_fds[n] == ACL_SOCKET_INVALID)
				acl_msg_fatal("%s(%d)->%s: listen on addr(%s) error(%s)",
					__FILE__, __LINE__, myname, end_point, strerror(errno));
			acl_tcp_defer_accept(serv->listen_fds[n], serv->defer_accept);
			serv->listen_streams[n] = acl_vstream_fdopen(serv->listen_fds[n],
							O_RDONLY,
							acl_var_master_buf_size,
							acl_var_master_rw_timeout,
							ACL_VSTREAM_TYPE_LISTEN_INET);
			acl_close_on_exec(serv->listen_fds[n], ACL_CLOSE_ON_EXEC);
			acl_msg_info("%s(%d)->%s: success listen on inet socket: %s, qlen: %d",
				__FILE__, __LINE__, myname, end_point, qlen);
			acl_myfree(end_point);
		}
		break;
	default:
		acl_msg_panic("%s: unknown service type: %d", myname, serv->type);
	}
}

/* acl_master_listen_cleanup - disable connection requests */

void    acl_master_listen_cleanup(ACL_MASTER_SERV *serv)
{
	char   *myname = "acl_master_listen_cleanup";
	int     n;

	/*
	 * XXX The listen socket is shared with child processes. Closing the
	 * socket in the master process does not really disable listeners in
	 * child processes. There seems to be no documented way to turn off a
	 * listener. The 4.4BSD shutdown(2) man page promises an ENOTCONN error
	 * when shutdown(2) is applied to a socket that is not connected.
	 */
	for (n = 0; n < serv->listen_fd_count; n++) {
		if (close(serv->listen_fds[n]) < 0)
			acl_msg_warn("%s: close listener socket %d: %s",
					myname, serv->listen_fds[n], strerror(errno));
		serv->listen_fds[n] = -1;
		acl_vstream_free(serv->listen_streams[n]);
		serv->listen_streams[n] = NULL;
	}
}
#endif /* ACL_UNIX */
