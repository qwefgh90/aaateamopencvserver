#include "StdAfx.h"
#include "MenuAnalyzer.h"

MenuAnalyzer* MenuAnalyzer::singleton=NULL;

MenuAnalyzer::MenuAnalyzer(void){
}
MenuAnalyzer::~MenuAnalyzer(void){
}
//parse menu to structures and call assemble packet function
//param
//@data received from mobile 
//return
//@buffer and size to send to mobile
Memory MenuAnalyzer::MenuSelector(Memory& in_memory)
{
	Memory out_memory;		//memory to mobile

	u_char* out_buf;		//buffer to mobile
	u_int out_len;			//buffer length to mobile

	u_char* in_buf=in_memory.buf;			//buffer from mobile
	u_int in_len=in_memory.len;			//buffer length from mobile

	u_char proto_type;		//proto_type

	proto_type = *((u_char*)(in_buf+4));
	switch(proto_type)
	{
	case LOGIN:
		{
		IN_Login in;	//로그인 요청 구조체
		OUT_Login out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		//1)패킷에서 구조체 생성
		this->packetToLogin(in,in_memory);
		//2)생성된 구조체를 각 모듈에 전달
		//3)생성된 구조체에서 패킷을 생성
		//4)모바일에서 패킷 전송
		
		break;
		}
	case SIGNUP:
		{
		IN_Signup in;	//로그인 요청 구조체
		OUT_Signup out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case LOGOUT:
		{
		IN_Logout in;	//로그인 요청 구조체
		OUT_Logout out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case LEAVE:
		{
		IN_Leave in;	//로그인 요청 구조체
		OUT_Leave out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case SEARCH:
		{
		IN_Search in;	//로그인 요청 구조체
		OUT_Search out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case MORE:
		{
		IN_More in;	//로그인 요청 구조체
		OUT_More out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case WRITE_COMMENT:
		{
		IN_Write_comment in;	//로그인 요청 구조체
		OUT_Write_comment out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case MODIFY_COMMENT:
		{
		IN_Modify_comment in;	//로그인 요청 구조체
		OUT_Modify_comment out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case DELETE_COMMENT:
		{
		IN_Delete_comment in;	//로그인 요청 구조체
		OUT_Delete_comment out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));


		break;
		}
	case LIKE:
		{
		IN_Like in;	//로그인 요청 구조체
		OUT_Like out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case DISLIKE:
		{
		IN_Dislike in;	//로그인 요청 구조체
		OUT_Dislike out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}
	case REPORT:
		{
		IN_Report in;	//로그인 요청 구조체
		OUT_Report out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));

		break;
		}

	}
	out_memory.len=out_len;
	out_memory.buf=out_buf;
	return out_memory;
}

MenuAnalyzer* MenuAnalyzer::GetMenuAnalyzer()
{
	if(MenuAnalyzer::singleton==NULL)
	{
		singleton = new MenuAnalyzer();
	}
	return singleton;
}

vector<string> MenuAnalyzer::split(string str, string sep){
    char* cstr = const_cast<char*>(str.c_str());
    char* current;
    vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current != NULL){
        arr.push_back(current);
        current=strtok(NULL, sep.c_str());
    }
    return arr;
}