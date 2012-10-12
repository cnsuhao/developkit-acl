// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once

//#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�

#include "acl_cpp_define.hpp"

#ifdef WIN32
#include "acl/lib_acl.h"
#include "protocol/lib_protocol.h"
#else
#include "lib_acl.h"
#include "lib_protocol.h"
#endif

#include "malloc.hpp"

#ifdef WIN32
# include "snprintf.hpp"
# ifdef _DEBUG
#  ifndef _CRTDBG_MAP_ALLOC
#   define _CRTDBG_MAP_ALLOC
#   include <crtdbg.h>
#   include <stdlib.h>
#  endif
#   undef NEW
#   define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
# else
#   undef NEW
#   define NEW new
# endif // _DEBUG
#else
# define NEW new
#endif

// ��������һ�п��Լӿ��� VC �µı����ٶ�
#ifdef WIN32
#include "lib_acl.hpp"
#endif
