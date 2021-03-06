#include "stdafx.h"
#include <time.h>

#define filter_no 6
typedef enum{LOGIN=1,SIGNUP=2,LOGOUT=3,LEAVE=4,SEARCH=5,MORE=6,
	WRITE_COMMENT=7,MODIFY_COMMENT=8,DELETE_COMMENT=9,LIKE=10,REPORT=11,CACHE=12,CHANGEPASS=13}PROTO_TYPE;
typedef enum{NEW_SORT=1,LIKE_SORT=0}SORT;

#define COFFEE 0x01
#define FOOD 0x02
#define RESTAURANT 0x04
#define HAIR 0x08
#define BAKERY 0x10
#define ETC 0x20
const unsigned char filter[filter_no] = { COFFEE, FOOD, RESTAURANT, HAIR, BAKERY, ETC};
#define IMAGE_DEBUG_MSG 1

//메모리구조체
typedef struct _Memory{
	u_char* buf;
	u_int len;
}Memory;

//DB->이미지모듈로 넘겨주는 구조체(상점 코드, 상점 키값)
typedef struct _Imagelist{
	u_int store_code;
	char store_name[256];
	char store_tel[20];
	char store_time[30];
	char store_rest[100];
	char store_path[256];
	float latitude;	//위도
	float longitude;//경도

	//매트릭스 객체 추가
}Imagelist;

//캐시->이미지모듈로 넘겨주는 구조체(상점코드,상점xml경로,xml버퍼) 
typedef struct _ImageBufferElement{
	u_int store_code;
	char store_name[256];
	char store_tel[20];
	char store_time[30];
	char store_rest[100];
	char store_path[256];
	cv::Mat key_xml;
	float latitude;	//위도
	float longitude;//경도
	float distance;//거리
	u_char filter;	//필터
	//매트릭스 객체 추가
}ImageBufferElement;



typedef struct _Store{
	float latitude;	//위도
	float longitude;//경도
	Memory image;	//image

}Store;


//멤버세션
typedef struct _MemberSession{
	char ID[30+1];
	time_t valid_stamp;
	unsigned char cookie[64];
}MemberSession;

typedef struct _IN_Login{
	char ID[30+1];
	char pass[65];
}IN_Login;

typedef struct _IN_Signup{
	char ID[30+1];
	char pass[65];
	char nick[16+1];
}IN_Signup;

typedef struct _IN_Logout{
	union{
		char ID[30+1];
		unsigned char cookie[64];
	};
}IN_Logout;

typedef struct _IN_Chpw{
	union{
		char ID[30+1];
		unsigned char cookie[64];
	};
	
	char prePass[21];
	char newPass[21];
}IN_Chpw;

typedef struct _IN_Leave{
	union{
		char ID[30+1];
		unsigned char cookie[64];
	};
}IN_Leave;

typedef struct _IN_Search{
	union{
		char ID[30+1];
		unsigned char cookie[64];
	};
	u_char sort;
	u_char filter;
	Store store;	//상점 구조체
//	float latitude;	//위도
//	float longitude;//경도
//	Memory image;	//image
}IN_Search;

typedef struct _IN_More{
	union{
		char ID[30+1];
		unsigned char cookie[64];
	};
	u_int code;
	u_char comment_count;
	u_char sort;
}IN_More;

typedef struct _IN_Write_comment{
	union{
		char ID[30+1];
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
		char ID[30+1];
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
		char ID[30+1];
		unsigned char cookie[64];
	};
	u_int code;
	u_int comment_num;
	u_char comment_count;
	u_char sort;
}IN_Delete_comment;

typedef struct _IN_Like{
	union{
		char ID[30+1];
		unsigned char cookie[64];
	};
	u_int num;
	u_char like;
}IN_Like;

typedef struct _IN_Report{
	union{
		char ID[30+1];
		unsigned char cookie[64];
	};
	u_char sort;
	u_char filter;
	char store_name[256];
	Store store;		//상점 구조체
	//float latitude;	//위도
	//float longitude;//경도
	//Memory image;	//image
	char comment[400+1];
	u_char comment_score;
}IN_Report;
typedef struct _IN_Cache{
	union{
		char ID[30+1];
		unsigned char cookie[64];
	};
	float latitude;	//위도
	float longitude;//경도
}IN_Cache;

//Opinion mini structure is contained 
typedef struct _OUT_Opinion{
	unsigned int sns_id;
	char nick[16+1];
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

typedef struct _OUT_Chpw{
	unsigned char result;
}OUT_Chpw;
typedef struct _OUT_Leave{
	unsigned char result;
}OUT_Leave;

//상점 리스트 구조체 추가
typedef struct _OUT_List{
	u_int code;
	char name[256];
	char store_tel[20];
	char store_time[30];
	char store_rest[100];
	float latitude;	//위도
	float longitude;//경도
	float matching;
}OUT_List;

typedef struct _OUT_Search{
	unsigned char result;
	//상점 갯수
	std::vector<OUT_List> out_list;
	u_int code;
	float score;
	//상호명 추가
	char name[256];
	char store_tel[20];
	char store_time[30];
	char store_rest[100];
	float latitude;	//위도
	float longitude;//경도
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
	char name[256];
	char store_tel[20];
	char store_time[30];
	char store_rest[100];
	float latitude;	//위도
	float longitude;//경도
	OUT_Opinion opi[10];
	u_int opi_cnt;
}OUT_Report;
