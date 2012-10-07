# Microsoft Developer Studio Project File - Name="lib_acl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_acl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_acl_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_acl_vc6.mak" CFG="lib_acl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_acl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_acl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_acl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../lib_acl/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_MT" /D "MS_VC6" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib_acl_vc6.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy lib_acl_vc6.lib ..\..\..\dist\lib\win32\lib_acl_vc6.lib /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_acl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /I "../../../lib_acl/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MT" /D "MS_VC6" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib_acl_vc6d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy lib_acl_vc6d.lib ..\..\..\dist\lib\win32\lib_acl_vc6d.lib /Y
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lib_acl - Win32 Release"
# Name "lib_acl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "stdlib"

# PROP Default_Filter ""
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_argv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_argv_split.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_array.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_binhash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_btree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_cache.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_cache2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_dlink.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_fifo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_hash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_htable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_iplink.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_ring.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_stack.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\acl_token_tree.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\common\avl.c
# End Source File
# End Group
# Begin Group "configure"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\configure\acl_loadcfg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\configure\acl_xinetd_cfg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\configure\acl_xinetd_params.c
# End Source File
# End Group
# Begin Group "filedir"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\filedir\acl_dir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\filedir\acl_fhandle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\filedir\acl_make_dirs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\filedir\acl_sane_basename.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\filedir\acl_scan_dir.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\filedir\dir_sys_patch.h
# End Source File
# End Group
# Begin Group "iostuff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_close_on_exec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_closefrom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_doze.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_duplex_pipe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_fdtype.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_non_blocking.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_open_limit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_peekfd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_pipe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_read_wait.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_readable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_timed_read.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_timed_write.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_write_buf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\iostuff\acl_write_wait.c
# End Source File
# End Group
# Begin Group "memory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\acl_allocator.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\acl_dbuf_pool.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\acl_default_malloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\acl_malloc_glue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\acl_mem_hook.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\acl_mem_slice.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\acl_mempool.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\acl_slice.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\allocator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\malloc_vars.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\mem_pool.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\ring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\squid_allocator.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\squid_allocator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\squid_allocator_tools.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\memory\vstring_pool.c
# End Source File
# End Group
# Begin Group "string"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\acl_alldig.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\acl_basename.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\acl_concatenate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\acl_hex_code.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\acl_mystring.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\acl_split_at.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\acl_split_nameval.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\acl_str2time.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\strcasecmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\string\strcasestr.c
# End Source File
# End Group
# Begin Group "sys"

# PROP Default_Filter ""
# Begin Group "unix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_chroot_uid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_mychown.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_open_lock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_safe_open.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_sane_socketpair.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_set_eugid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_set_ugid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_timed_wait.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_transfer_fd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_username.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\acl_watchdog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\posix_signals.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\unix\posix_signals.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\acl_dll.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\acl_env.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\acl_exec_command.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\acl_process.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\acl_safe_getenv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\acl_sys_file.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\acl_sys_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\acl_unsafe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\gettimeofday.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\sleep.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\sys\snprintf.c
# End Source File
# End Group
# Begin Group "debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\debug\acl_debug_malloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\debug\debug_htable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\debug\htable.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_chunk_chain.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_debug.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_file.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_getopt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_meter_time.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_msg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_myflock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_mylog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_readline.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_vbuf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_vbuf_print.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_vsprintf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_vstream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_vstream_popen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_vstring.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\acl_vstring_vstream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\charmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\stdlib\getopt.c
# End Source File
# End Group
# Begin Group "aio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\aio\acl_aio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\aio\acl_aio_connect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\aio\acl_aio_listen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\aio\acl_aio_read.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\aio\acl_aio_stream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\aio\acl_aio_write.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\aio\aio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\aio\aio_callback.c
# End Source File
# End Group
# Begin Group "code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\acl_base64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\acl_gbcode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\acl_urlcode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\acl_vstring_base64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\charsetfunc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\charsetfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\gb_ft2jt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\gb_jt2ft.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\gbandutf8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\gbandutf8.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\utf8andunicode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\code\utf8andunicode.h
# End Source File
# End Group
# Begin Group "db"

# PROP Default_Filter ""
# Begin Group "memdb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\acl_mdb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\acl_mdt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\acl_mdt_avl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\acl_mdt_binhash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\acl_mdt_hash.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\define.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\mdb_private.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\ring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\memdb\struct.h
# End Source File
# End Group
# Begin Group "mysql"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\mysql\acl_dbmysql.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\mysql\acl_dbmysql.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\mysql\acl_dbpool_mysql.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\mysql\acl_dbpool_mysql.h
# End Source File
# End Group
# Begin Group "null"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\null\acl_dbnull.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\null\acl_dbnull.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\null\acl_dbpool_null.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\null\acl_dbpool_null.h
# End Source File
# End Group
# Begin Group "zdb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\zdb\zdb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\zdb\zdb_dat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\zdb\zdb_dat_iter.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\zdb\zdb_dat_walk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\zdb\zdb_io.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\zdb\zdb_key.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\zdb\zdb_key_walk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\zdb\zdb_private.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\acl_dbpool.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\db\acl_dbsql.c
# End Source File
# End Group
# Begin Group "event"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\acl_events.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\acl_timer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_alloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_define.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_devpoll.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_dog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_dog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_epoll.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_fdtable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_fdtable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_iocp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_iocp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_kernel.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_kernel2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_kernel3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_kernel_thr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_kqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_poll.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_poll_thr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_select.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_select_thr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_timer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\events_timer_thr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\fdmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\event\fdmap.h
# End Source File
# End Group
# Begin Group "experiment"

# PROP Default_Filter ""
# End Group
# Begin Group "ioctl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\ioctl\acl_ioctl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\ioctl\ioctl_internal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\ioctl\ioctl_proc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\ioctl\ioctl_thr.c
# End Source File
# End Group
# Begin Group "master"

# PROP Default_Filter ""
# Begin Group "daemon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\daemon\master.c
# End Source File
# End Group
# Begin Group "framework"

# PROP Default_Filter ""
# Begin Group "trigger"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\trigger\fifo_trigger.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\trigger\inet_trigger.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\trigger\stream_trigger.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\trigger\trigger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\trigger\unix_trigger.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_avail.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_conf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_ent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_listen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_pathname.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_pathname.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_service.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_sig.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_spawn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_status.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\framework\master_wakeup.c
# End Source File
# End Group
# Begin Group "template"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\template\acl_aio_app_main.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\template\acl_ioctl_app_main.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\template\acl_listener_server.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\template\acl_multi_server.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\template\acl_single_server.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\template\acl_trigger_server.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\template\aio_msg.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\master_flow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\master_flow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\master_params.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\master_params.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\master_proto.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\master_proto.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\master\master_vars.c
# End Source File
# End Group
# Begin Group "msg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\msg\acl_aqueue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\msg\acl_msgio.c
# End Source File
# End Group
# Begin Group "net"

# PROP Default_Filter ""
# Begin Group "connect"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\connect\acl_inet_connect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\connect\acl_sane_connect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\connect\acl_stream_connect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\connect\acl_timed_connect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\connect\acl_unix_connect.c
# End Source File
# End Group
# Begin Group "dns"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\dns\acl_dns.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\dns\acl_netdb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\dns\acl_netdb_cache.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\dns\acl_res.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\dns\res_util.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\dns\rfc1035.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\dns\rfc1035.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\dns\util.h
# End Source File
# End Group
# Begin Group "listen"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\listen\acl_fifo_listen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\listen\acl_inet_listen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\listen\acl_sane_accept.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\listen\acl_unix_listen.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_access.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_host_port.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_ifconf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_mask_addr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_sane_inet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_sane_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_tcp_ctl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_valid_hostname.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\acl_vstream_net.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\Ipifcons.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\net\IPTypes.h
# End Source File
# End Group
# Begin Group "proctl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\proctl\acl_proctl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\proctl\proctl_child.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\proctl\proctl_internal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\proctl\proctl_monitor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\proctl\proctl_service.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\proctl\proctl_utils.c
# End Source File
# End Group
# Begin Group "svr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\svr\acl_workq.c
# End Source File
# End Group
# Begin Group "thread"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\thread\acl_pthread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\thread\acl_pthread_cond.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\thread\acl_pthread_mutex.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\thread\acl_pthread_pool.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\thread\acl_pthread_rwlock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\thread\acl_sem.c
# End Source File
# End Group
# Begin Group "unit_test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_cfg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_cfg_general.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_inner.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_loop.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_misc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_outer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_runner.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\unit_test\acl_test_token.c
# End Source File
# End Group
# Begin Group "init"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\init\acl_init.c
# End Source File
# End Group

# Begin Group "xml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\xml\acl_xml.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\xml\acl_xml_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\xml\acl_xml_util.c
# End Source File
# End Group

# Begin Group "private"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\private.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\private_array.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\private_array.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\private_fifo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\private_fifo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\sem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\sem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\src\private\thread_mutex.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "aio No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\aio\acl_aio.h
# End Source File
# End Group
# Begin Group "code No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\code\acl_base64.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\code\acl_code.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\code\acl_gbcode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\code\acl_urlcode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\code\acl_vstring_base64.h
# End Source File
# End Group
# Begin Group "db No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\db\acl_db.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\db\acl_dberr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\db\acl_dbpool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\db\acl_dbsql.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\db\acl_mdb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\db\zdb.h
# End Source File
# End Group
# Begin Group "event No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\event\acl_events.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\event\acl_timer.h
# End Source File
# End Group
# Begin Group "experiment No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\experiment\experiment.h
# End Source File
# End Group
# Begin Group "ioctl No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\ioctl\acl_ioctl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\ioctl\acl_myioctl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\ioctl\acl_spool.h
# End Source File
# End Group
# Begin Group "master No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_aio_params.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_app_main.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_ioctl_params.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_listener_params.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_master.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_master_conf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_master_type.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_multi_params.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_server_api.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_single_params.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\master\acl_trigger_params.h
# End Source File
# End Group
# Begin Group "msg No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\msg\acl_aqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\msg\acl_msgio.h
# End Source File
# End Group
# Begin Group "net No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_access.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_connect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_dns.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_host_port.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_ifconf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_listen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_mask_addr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_net.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_netdb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_res.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_sane_inet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_sane_socket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_tcp_ctl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_valid_hostname.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\net\acl_vstream_net.h
# End Source File
# End Group
# Begin Group "proctl No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\proctl\acl_proctl.h
# End Source File
# End Group
# Begin Group "stdlib No. 1"

# PROP Default_Filter ""
# Begin Group "unix No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_chroot_uid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_mychown.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_open_lock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_safe_open.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_sane_socketpair.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_set_eugid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_set_ugid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_timed_wait.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_transfer_fd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_unix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_username.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\unix\acl_watchdog.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_allocator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_argv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_array.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_binhash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_bits_map.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_btree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_cache.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_cache2.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_cfg_macro.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_chunk_chain.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_dbuf_pool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_debug_malloc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_define.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_define_bsd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_define_linux.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_define_sunx86.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_define_unix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_define_win32.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_dir.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_dlink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_dll.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_env.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_exec_command.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_fhandle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_fifo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_file.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_getopt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_hash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_hex_code.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_htable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_iostuff.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_iplink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_iterator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_loadcfg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_make_dirs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_malloc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_mem_hook.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_mem_slice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_meter_time.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_myflock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_mylog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_mymalloc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_mystring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_process.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_readline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_ring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_safe.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_sane_basename.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_scan_dir.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_slice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_split_at.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_stack.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_stdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_stringops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_sys_patch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_timeops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_token_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_vbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_vbuf_print.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_vsprintf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_vstream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_vstream_popen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_vstring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_vstring_vstream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\acl_xinetd_cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\avl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\stdlib\avl_impl.h
# End Source File
# End Group
# Begin Group "svr No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\svr\acl_svr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\svr\acl_task.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\svr\acl_workq.h
# End Source File
# End Group
# Begin Group "thread No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\thread\acl_pthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\thread\acl_pthread_pool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\thread\acl_pthread_rwlock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\thread\acl_sem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\thread\acl_thread.h
# End Source File
# End Group
# Begin Group "unit_test No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\unit_test\acl_test_global.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\unit_test\acl_test_macro.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\unit_test\acl_test_struct.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\unit_test\acl_test_var.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\unit_test\acl_unit_test.h
# End Source File
# End Group
# Begin Group "init No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\init\acl_init.h
# End Source File
# End Group

# Begin Group "xml No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_acl\include\xml\acl_xml.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_acl\include\lib_acl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_acl\include\lib_util.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
