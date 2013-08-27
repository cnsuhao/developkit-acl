#!/bin/sh

HOME_PATH={install_path}
PROG_NAME=acl_master
PID_FILE=$HOME_PATH/var/pid/acl_master.pid
EXE_PATH=$HOME_PATH/libexec/$PROG_NAME
CONF_PATH=$HOME_PATH/conf
LOG_PATH=$HOME_PATH/var/log/acl_master

RUNNING="no"
PID=

check_proc()
{
	RUNNING="no"

	if [ -f $PID_FILE ]; then
		PID=`sed 1q $PID_FILE | awk '{print $1}'`
		if [ -d "/proc/$PID" ]; then
			link_path=`ls -l /proc/$PID/exe | awk '{print $10}'`
			if [ "$link_path" = "$EXE_PATH" ]; then
				RUNNING="yes"
			fi
		fi
	fi
}

start()
{
	trap '' 1
	ulimit -c unlimited
	ulimit -n 20480

	check_proc
	if [ "$RUNNING" = "yes" ]; then
		echo "$PROG_NAME (pid=$PID) running ..."
		exit 1
	fi

	echo "starting $PROG_NAME ..."

	$EXE_PATH -c $CONF_PATH -l $LOG_PATH &

	sleep 1

	check_proc
	if [ "$RUNNING" != "yes" ]; then
		echo "start $PROG_NAME failed!"
		exit 1
	fi

	echo "$PROG_NAME started!"
}

stop()
{
	check_proc
	if [ "$RUNNING" = "yes" ]; then
		echo "stoping $PROG_NAME now ..."
		kill `sed 1q $PID_FILE`
		rm -f $PID_FILE
		echo "$PROG_NAME stoped!"
	else
		echo "$PROG_NAME not running!"
	fi
}

reload()
{
	check_proc
	if [ "$RUNNING" = "yes" ]; then
		kill -HUP `sed 1q $PID_FILE`
		echo "$PROG_NAME reloaded!"
	else
		echo "$PROG_NAME not running!"
	fi
}

# See how we were called.
case "$1" in
	start)
		start
		;;
	stop)
		stop
		;;
	reload)
		reload
		;;
	*)
		echo $"Usage: $0 {start|stop|reload}"
		;;
esac
