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
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <vector>
#include <hash_map>

//We make required headers

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

#include "protocol.h"


//Function
void dumpbyte(u_char* buf,u_int len);
int Compare( const void *arg1, const void *arg2 );

using namespace std;
using namespace stdext;
extern int a;
extern string spliter;
extern string spliter_end;
#endif