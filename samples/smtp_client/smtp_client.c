// smtp_client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "lib_acl.h"
#include "lib_protocol.h"

static int smtp_sender(void)
{
	SMTP_CLIENT* conn;
	char  addr[128], line[256];

	acl_printf("please enter smtp server addr: ");
	if (acl_gets_nonl(addr, sizeof(addr)) == NULL)
	{
		acl_puts("invalid smtp server addr");
		return -1;
	}

	/* ���� SMTP ������ */
	conn = smtp_open(addr, 60, 1024);
	if (conn == NULL)
	{
		acl_printf("connect %s error %s\r\n", addr, acl_last_serror());
		return -1;
	}

	/* �� SMTP ��������û�ӭ��Ϣ */
	if (smtp_get_banner(conn) != 0)
	{
		acl_puts("get banner from server error");
		smtp_close(conn);
		return -1;
	}

	/* �û��Ƿ���Ҫ���� SMTP �����֤ */
	acl_printf("Do you want to auth login? n[y|n]: ");
	if (acl_gets_nonl(line, sizeof(line)) == NULL)
	{
		acl_puts("invalid input");
		smtp_close(conn);
		return -1;
	}

	/* ���û���ݽ��� SMTP �����֤ */
	else if (strcasecmp(line, "Y") == 0)
	{
		char user[128], pass[128];
		acl_printf("Please input user account: ");
		if (acl_gets_nonl(user, sizeof(user)) == NULL)
		{
			acl_puts("input invalid");
			smtp_close(conn);
			return -1;
		}

		acl_printf("Please input user password: ");
		if (acl_gets_nonl(pass, sizeof(pass)) == NULL)
		{
			acl_puts("input invalid");
			smtp_close(conn);
			return -1;
		}

		/* ��ʼ���� SMTP �����֤ */
		if (smtp_auth(conn, user, pass) != 0)
		{
			acl_printf("smtp auth(%s, %s) error: %s, code: %d\r\n",
				user, pass, conn->buf, conn->smtp_code);
			smtp_close(conn);
			return -1;
		}
		else
			acl_printf("smtp auth ok: %s\r\n", conn->buf);
	}

	/* ��÷����������ַ */
	acl_printf("please input sender's email: ");
	if (acl_gets_nonl(line, sizeof(line)) == NULL)
	{
		acl_puts("invalid sender's email");
		smtp_close(conn);
		return -1;
	}

	/* ���� MAIL FROM: ���� */
	if (smtp_mail(conn, line) != 0)
	{
		acl_printf("smtp send MAIL FROM %s error\r\n", line);
		smtp_close(conn);
		return -1;
	}

	/* ���� RCPT TO: ���� */
	while (1)
	{
		acl_printf("please input mail recipients: ");
		if (acl_gets_nonl(line, sizeof(line)) == NULL)
		{
			acl_puts("invalid mail recipients");
			smtp_close(conn);
			return -1;
		}

		/* ���� RCPT TO: ���� */
		else if (smtp_rcpt(conn, line) != 0)
		{
			acl_printf("send RCPT TO: %s error: %s, code: %d\r\n",
				line, conn->buf, conn->smtp_code);
			smtp_close(conn);
			return -1;
		}

		acl_printf("Do you want to add another recipients? n[y|n]: ");
		if (acl_gets_nonl(line, sizeof(line)) == NULL)
		{
			acl_puts("input invalid");
			smtp_close(conn);
			return -1;
		}
		else if (strcasecmp(line, "y") != 0)
			break;
	}

	/* ���� DATA: ���� */
	if (smtp_data(conn) != 0)
	{
		acl_printf("send DATA error %s, code: %d\r\n",
			conn->buf, conn->smtp_code);
		smtp_close(conn);
		return -1;
	}

	/* ���ն˽����û���������ʼ����ݲ����� SMTP ������ */
	acl_puts("Please enter the email data below, end with \\r\\n.\\r\\n");

	while (1)
	{
		if (acl_gets_nonl(line, sizeof(line)) == NULL)
		{
			acl_puts("readline error");
			smtp_close(conn);
			return -1;
		}
		if (strcmp(line, ".") == 0)
			break;
		if (smtp_printf(conn, "%s\r\n", line) != 0)
		{
			acl_printf("send data to smtpd error, data: %s\r\n", line);
			smtp_close(conn);
			return -1;
		}
	}

	/* ���� \r\n.\r\n */
	if (smtp_data_end(conn) != 0)
	{
		acl_printf("send . error: %s, code: %d\r\n",
			conn->buf, conn->smtp_code);
		smtp_close(conn);
		return -1;
	}
	else
		acl_printf("send ok: %s\r\n", conn->buf);
	/* ���� QUIT ���� */
	smtp_quit(conn);
	smtp_close(conn);
	return 0;
}

int main(void)
{
	int   ret;

#ifdef WIN32
	acl_init();
#endif

	while (1)
	{
		char line[128];

		ret = smtp_sender();
		if (ret == -1)
			break;
		acl_printf("Do you want to send another email? n[y|n]: ");
		if (acl_gets_nonl(line, sizeof(line)) == NULL)
		{
			acl_puts("invalid input");
			break;
		}
		else if (strcasecmp(line, "y") != 0)
			break;
	}

#ifdef WIN32
	acl_vstream_printf("enter any key to exit\r\n");
	acl_vstream_getc(ACL_VSTREAM_IN);
#endif

	return ret;
}
