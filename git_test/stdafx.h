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

#endif