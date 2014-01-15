#include "stdafx.h"
typedef enum{LOGIN=1,SIGNUP=2,LOGOUT=3,LEAVE=4,SEARCH=5,MORE=6,
	WRITE_COMMENT=7,MODIFY_COMMENT=8,DELETE_COMMENT=9,LIKE=10,DISLIKE=11,REPORT=12}PROTO_TYPE;
//메모리구조체
typedef struct _Memory{
	u_char* buf;
	u_int len;
}Memory;
//멤버세션
typedef struct _MemberSession{
	unsigned char cookie[64];
	char ID[20];
}MemberSession;


typedef struct _IN_Login{
	char ID[20];
	char pass[20];
}IN_Login;

typedef struct _IN_Signup{
	char ID[20];
	char pass[20];
	char nick[40];
}IN_Signup;

typedef struct _IN_Logout{
	unsigned char cookie[64];
}IN_Logout;

typedef struct _IN_Leave{
	unsigned char cookie[64];
}IN_Leave;

typedef struct _IN_Search{
	unsigned char cookie[64];
	u_char filter;
	float latitude;	//위도
	float longitude;//경도
	Memory image;	//image
}IN_Search;

typedef struct _IN_More{
}IN_More;

typedef struct _IN_Write_comment{
}IN_Write_comment;

typedef struct _IN_Modify_comment{
}IN_Modify_comment;

typedef struct _IN_Delete_comment{
}IN_Delete_comment;

typedef struct _IN_Like{
}IN_Like;

typedef struct _IN_Dislike{
}IN_Dislike;

typedef struct _IN_Report{
}IN_Report;

typedef struct _OUT_Login{
	unsigned char result;
	unsigned char cookie[64];
}OUT_Login;

typedef struct _OUT_Signup{
	unsigned char result;
}OUT_Signup;

typedef struct _OUT_Logout{
}OUT_Logout;

typedef struct _OUT_Leave{
}OUT_Leave;

typedef struct _OUT_Search{
}OUT_Search;

typedef struct _OUT_More{
}OUT_More;

typedef struct _OUT_Write_comment{
}OUT_Write_comment;

typedef struct _OUT_Modify_comment{
}OUT_Modify_comment;

typedef struct _OUT_Delete_comment{
}OUT_Delete_comment;

typedef struct _OUT_Like{
}OUT_Like;

typedef struct _OUT_Dislike{
}OUT_Dislike;

typedef struct _OUT_Report{
}OUT_Report;

