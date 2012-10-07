# Microsoft Developer Studio Project File - Name="lib_protocol_vc6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lib_protocol_vc6 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lib_protocol_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lib_protocol_vc6.mak" CFG="lib_protocol_vc6 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lib_protocol_vc6 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lib_protocol_vc6 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lib_protocol_vc6 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../lib_protocol/include" /I "../../../lib_acl/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_MT" /D "MS_VC6" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib_protocol_vc6.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy lib_protocol_vc6.lib ..\..\..\dist\lib\win32\lib_protocol_vc6.lib /Y
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lib_protocol_vc6 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /I "../../../lib_protocol/include" /I "../../../lib_acl/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MT" /D "MS_VC6" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib_protocol_vc6d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy lib_protocol_vc6d.lib ..\..\..\dist\lib\win32\lib_protocol_vc6d.lib /Y
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lib_protocol_vc6 - Win32 Release"
# Name "lib_protocol_vc6 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "http"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_chat_async.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_chat_sync.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_hdr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_hdr_build.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_hdr_req.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_hdr_res.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_req.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_res.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_rfc1123.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_status.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_tmpl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\http\http_util.c
# End Source File
# End Group
# Begin Group "icmp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_chat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_chat_aio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_chat_sio.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_host.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_ping.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_pkt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_private.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_stat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_stream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_struct.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\src\icmp\icmp_timer.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "http No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_protocol\include\http\lib_http.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\include\http\lib_http_status.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\include\http\lib_http_struct.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\include\http\lib_http_util.h
# End Source File
# End Group
# Begin Group "icmp No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib_protocol\include\icmp\lib_icmp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib_protocol\include\icmp\lib_icmp_type.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib_protocol\include\lib_protocol.h
# End Source File
# End Group
# End Target
# End Project
