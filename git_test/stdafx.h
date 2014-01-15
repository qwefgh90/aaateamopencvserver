// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
//Precompile Not Use
//we use this for headers set
#if !defined(aaa)
#define aaa

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

#include <WinSock2.h>
#include <Windows.h>
#include <list>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>

typedef struct _Memory{
	u_char* buf;
	u_int len;
}Memory;
typedef enum{LOGIN=1,SIGNUP=2,LOGOUT=3,LEAVE=4,SEARCH=5,MORE=6,
	WRITE_COMMENT=7,MODIFY_COMMENT=8,DELETE_COMMENT=9,LIKE=10,DISLIKE=11,REPORT=12}PROTO_TYPE;
u_char spliter[2]={'\r','\n'};
u_char spliter_end[4]={'\r','\n','\r','\n'};
#endif