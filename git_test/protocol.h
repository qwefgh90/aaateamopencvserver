#include "stdafx.h"

#define filter_no 6
typedef enum{LOGIN=1,SIGNUP=2,LOGOUT=3,LEAVE=4,SEARCH=5,MORE=6,
	WRITE_COMMENT=7,MODIFY_COMMENT=8,DELETE_COMMENT=9,LIKE=10,REPORT=11,CACHE=12}PROTO_TYPE;

#define COFFEE 0x01
#define FOOD 0x02
#define RESTAURANT 0x04
#define HAIR 0x08
#define BAKERY 0x10
#define ETC 0x20
#define ERRORRANGE 0.001
const unsigned char filter[filter_no] = { COFFEE, FOOD, RESTAURANT, HAIR, BAKERY, ETC};

//메모리구조체
typedef struct _Memory{
	u_char* buf;
	u_int len;
}Memory;

//DB->이미지모듈로 넘겨주는 구조체(상점 코드, 상점 키값)
typedef struct _Imagelist{
	u_int store_code;
	char store_path[256];

	//매트릭스 객체 추가
}Imagelist;

//캐시->이미지모듈로 넘겨주는 구조체(상점코드,상점xml경로,xml버퍼) 
typedef struct _ImageBufferElement{
	u_int store_code;
	char store_path[256];
	cv::Mat key_xml;
	float latitude;	//위도
	float longitude;//경도
	float distance;//거리
	//매트릭스 객체 추가
}ImageBufferElement;



typedef struct _Store{
	float latitude;	//위도
	float longitude;//경도
	Memory image;	//image
}Store;


//멤버세션
typedef struct _MemberSession{
	char ID[20+1];
	u_int valid_stamp;
	unsigned char cookie[64];
}MemberSession;

typedef struct _IN_Login{
	char ID[20+1];
	char pass[20+1];
}IN_Login;

typedef struct _IN_Signup{
	char ID[20+1];
	char pass[20+1];
	char nick[40+1];
}IN_Signup;

typedef struct _IN_Logout{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
}IN_Logout;

typedef struct _IN_Leave{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
}IN_Leave;

typedef struct _IN_Search{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
	u_char filter;
	Store store;	//상점 구조체
//	float latitude;	//위도
//	float longitude;//경도
//	Memory image;	//image
}IN_Search;

typedef struct _IN_More{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
	u_int code;
	u_char comment_count;
	u_char sort;
}IN_More;

typedef struct _IN_Write_comment{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
	u_int code;
	u_char comment_count;
	u_char sort;
	char comment[400+1];
	u_char comment_score;
}IN_Write_comment;

typedef struct _IN_Modify_comment{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
	u_int code;
	u_int comment_num;
	u_char comment_count;
	u_char sort;
	u_char comment_score;
	char comment[400+1];
}IN_Modify_comment;

typedef struct _IN_Delete_comment{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
	u_int code;
	u_int comment_num;
	u_char comment_count;
	u_char sort;
}IN_Delete_comment;

typedef struct _IN_Like{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
	u_int num;
	u_char like;
}IN_Like;

typedef struct _IN_Report{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
	u_char filter;
	Store store;		//상점 구조체
	//float latitude;	//위도
	//float longitude;//경도
	//Memory image;	//image
	char comment[400+1];
	u_char comment_score;
}IN_Report;
typedef struct _IN_Cache{
	union{
		char ID[20+1];
		unsigned char cookie[64];
	};
	float latitude;	//위도
	float longitude;//경도
}IN_Cache;

//Opinion mini structure is contained 
typedef struct _OUT_Opinion{
	unsigned int sns_id;
	char nick[41+1];
	char comment[400+1];
	u_int like_cnt;
	u_int dislike_cnt;
}OUT_Opinion;

typedef struct _OUT_Login{
	unsigned char result;
	char nick[16+1];
	unsigned char cookie[64];
}OUT_Login;

typedef struct _OUT_Signup{
	unsigned char result;
}OUT_Signup;

typedef struct _OUT_Logout{
	unsigned char result;
}OUT_Logout;

typedef struct _OUT_Leave{
	unsigned char result;
}OUT_Leave;

typedef struct _OUT_Search{
	unsigned char result;
	u_int code;
	float score;
	OUT_Opinion opi[10];
	u_int opi_cnt;
}OUT_Search;

typedef struct _OUT_More{
	unsigned char result;
	float score;
	OUT_Opinion opi[10];
	u_int opi_cnt;
}OUT_More;

typedef struct _OUT_Write_comment{
	unsigned char result;
	float score;
	OUT_Opinion opi[10];
	u_int opi_cnt;
}OUT_Write_comment;

typedef struct _OUT_Modify_comment{
	unsigned char result;
	float score;
	OUT_Opinion opi[10];
	u_int opi_cnt;
}OUT_Modify_comment;

typedef struct _OUT_Delete_comment{
	unsigned char result;
	float score;
	OUT_Opinion opi[10];
	u_int opi_cnt;
}OUT_Delete_comment;

typedef struct _OUT_Like{
	unsigned char result;
}OUT_Like, OUT_Cache;	//캐시 출력구조체도 포함

typedef struct _OUT_Report{
	unsigned char result;
	u_int code;
	float score;
	OUT_Opinion opi[10];
	u_int opi_cnt;
}OUT_Report;
