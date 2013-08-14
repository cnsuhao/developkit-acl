#include "lib_acl.h"

static void trigger_event(int event acl_unused, void *context acl_unused)
{
	printf("timer trigger now\r\n");
}

static void read_callback(int event_type acl_unused, void *context)
{
	ACL_VSTREAM *in = (ACL_VSTREAM*) context;
	ACL_EVENT *eventp = (ACL_EVENT*) in->context;
	char  buf[256];

	if (acl_vstream_gets_nonl(in, buf, sizeof(buf)) == ACL_VSTREAM_EOF) {
		acl_event_disable_read(eventp, in);
		printf(">>>>gets error\r\n");
		acl_vstream_free(in);
	} else {
		printf(">>>>gets: %s\r\n", buf);
	}
}

static void usage(const char *procname)
{
	printf("usage: %s -h [help]\r\n"
		"	-M [printf time meter]\r\n"
		"	-t select|poll|kernel [default: select]\r\n"
		"	-s delay_sec [defaut: 1]\r\n"
		"	-u delay_usec [default: 0]\r\n"
		"	-m timer_delay [default: 100 microsecond, disable timer if < 0]\r\n"
		"	-k [if timer keep on, default: 1]\r\n", procname);
}

int main(int argc acl_unused, char *argv[] acl_unused)
{
	ACL_EVENT *eventp;
	ACL_VSTREAM *in;
	char  event_type[64];
	int   ch, delay_sec = 1, delay_usec = 0;
	int   timer_delay = 100, timer_keep = 0;
	int   meter = 0;

	/* 初始化 acl 库 */
	acl_init();

	/* 将错误日志输出至标准输出 */
	acl_msg_stdout_enable(1);

	event_type[0] = 0;

	while ((ch = getopt(argc, argv, "ht:s:u:m:kM")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 't':
			ACL_SAFE_STRNCPY(event_type, optarg, sizeof(event_type));
			break;
		case 's':
			delay_sec = atoi(optarg);
			if (delay_sec < 0)
				delay_sec = 0;
			break;
		case 'u':
			delay_usec = atoi(optarg);
			if (delay_usec < 0)
				delay_usec = 0;
			break;
		case 'm':
			timer_delay = atoi(optarg);
			break;
		case 'k':
			timer_keep = 1;
			break;
		case 'M':
			meter = 1;
			break;
		default:
			break;
		}
	}

	/* 创建非阻塞事件引擎 */
	if (strcasecmp(event_type, "kernel") == 0)
		eventp = acl_event_new_kernel(delay_sec, delay_usec);
	else if (strcasecmp(event_type, "poll") == 0)
		eventp = acl_event_new_poll(delay_sec, delay_usec);
	else
		eventp = acl_event_new_select(delay_sec, delay_usec);

	if (timer_delay >= 0)
		acl_event_request_timer(eventp, trigger_event, NULL,
			timer_delay, timer_keep);

	/* 将标准输入置入异步 IO 监听事件中 */
	in = acl_vstream_fhopen(0, 0);
	in->context = eventp;
	acl_event_enable_read(eventp, in, 0, read_callback, ACL_VSTREAM_IN);

	printf("begin wait input from standard in\r\n");

	/* 进行事件循环中 */
	while (1) {
		if (meter)
			ACL_METER_TIME("begin event");
		acl_event_loop(eventp);
		if (meter)
			ACL_METER_TIME("end event");
	}

	return (0);
}

