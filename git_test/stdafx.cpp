// stdafx.cpp : source file that includes just the standard includes
// git_test.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
int a;
string spliter("\r\n");
string spliter_end("\r\n\r\n");
void dumpbyte(u_char* buf,u_int len)
{
	int i=0;
	for (i=0 ; i<len;i++)
		printf("0x%x ",buf[i]);
	printf("\n");
}
