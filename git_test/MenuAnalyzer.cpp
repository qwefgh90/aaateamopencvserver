#include "StdAfx.h"
#include "MenuAnalyzer.h"

MenuAnalyzer* MenuAnalyzer::singleton=NULL;

MenuAnalyzer::MenuAnalyzer(void)
{

}

MenuAnalyzer::~MenuAnalyzer(void)
{

}
//parse menu to structures and call assemble packet function
//param
//@data received from mobile 
//return
//@buffer and size to send to mobile
Memory MenuAnalyzer::MenuSelector(Memory in_memory)
{
	Memory out_memory;		//memory to mobile

	u_char* out_buf;		//buffer to mobile
	u_int out_len;			//buffer length to mobile

	u_char* in_buf;			//buffer from mobile
	u_int in_len;			//buffer length from mobile

	u_char proto_type;		//proto_type

	proto_type = *((u_char*)(in_buf+4));
	switch(proto_type)
	{
	case LOGIN:
		IN_Login in;	//로그인 요청 구조체
		OUT_Login out;	//로그인 응답 구조체
		break;
	case SIGNUP:
		break;
	case LOGOUT:
		break;
	case LEAVE:
		break;
	case SEARCH:
		break;
	case MORE:
		break;
	case WRITE_COMMENT:
		break;
	case MODIFY_COMMENT:
		break;
	case DELETE_COMMENT:
		break;
	case LIKE:
		break;
	case DISLIKE:
		break;
	case REPORT:
		break;

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