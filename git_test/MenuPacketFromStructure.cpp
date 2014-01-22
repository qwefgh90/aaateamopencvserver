#include "stdafx.h"
#include "MenuAnalyzer.h"

const u_int LOGIN_PACKET_SIZE_BUT_NICKNAME = (4+2)+(1+2)+(64+2)+4;
	//len + result + cookie + (exclude nick)
bool packetFromLogin(__out Memory& out, __in OUT_Login& in)
{
	bool result = false;
	u_int bytelen=0;
	u_char result_login=0;
	u_char *cookie=NULL;
	u_char *nick=NULL;
	u_int nick_len = strlen(in.nick);

	bytelen = LOGIN_PACKET_SIZE_BUT_NICKNAME + nick_len;
	result_login = in.result;
	cookie=in.cookie;
	nick=(u_char*)in.nick;
	
	out.len = bytelen;
	out.buf = new u_char[bytelen];
	u_int* bytelen_ptr=(u_int*)out.buf;
	u_char* result_login_ptr=((u_char*)bytelen_ptr)+4 +2;
	u_char* cookie_ptr=result_login_ptr+1 +2;
	u_char* nick_ptr=cookie_ptr+64 +2;
	//copy struct to new buf
	
	(*bytelen_ptr) = bytelen;
	memcpy(((u_char*)result_login_ptr)-2,spliter.c_str(),2);	//\r\n

	(*result_login_ptr) = result_login;
	memcpy(((u_char*)cookie_ptr)-2,spliter.c_str(),2);			//\r\n

	memcpy(cookie_ptr,cookie,64);
	memcpy(((u_char*)nick_ptr)-2,spliter.c_str(),2);			//\r\n

	memcpy(nick_ptr,nick,nick_len);
	memcpy(((u_char*)nick_ptr)+nick_len,spliter_end.c_str(),4);	//\r\n\r\n spliter_end

	result = true;
	return result;
}
const u_int SIGNUP_PACKET_SIZE = (4+2)+(1+4);
	//len + result
bool packetFromSignup(__out Memory& out, __in OUT_Signup& in)
{
	bool result = false;

	u_int bytelen=0;
	u_char result_login=0;

	bytelen = SIGNUP_PACKET_SIZE;
	result_login = in.result;
	
	out.len = bytelen;
	out.buf = new u_char[bytelen];

	u_int* bytelen_ptr=(u_int*)out.buf;
	u_char* result_login_ptr=((u_char*)bytelen_ptr)+4 +2;

	
	(*bytelen_ptr) = bytelen;
	memcpy(((u_char*)result_login_ptr)-2,spliter.c_str(),2);			//\r\n

	(*result_login_ptr) = result_login;
	memcpy(((u_char*)result_login_ptr)+1,spliter_end.c_str(),4);		//\r\n\r\n spliter_end
	
	result = true;
	return result;
}
bool packetFromLogout(__out Memory& out, __in OUT_Logout& in)
{
	bool result = false;

	u_int bytelen=0;
	u_char result_login=0;

	bytelen = SIGNUP_PACKET_SIZE;
	result_login = in.result;
	
	out.len = bytelen;
	out.buf = new u_char[bytelen];

	u_int* bytelen_ptr=(u_int*)out.buf;
	u_char* result_login_ptr=((u_char*)bytelen_ptr)+4 +2;

	
	(*bytelen_ptr) = bytelen;
	memcpy(((u_char*)result_login_ptr)-2,spliter.c_str(),2);			//\r\n

	(*result_login_ptr) = result_login;
	memcpy(((u_char*)result_login_ptr)+1,spliter_end.c_str(),4);		//\r\n\r\n spliter_end
	
	result = true;
	return result;
}
bool packetFromLeave(__out Memory& out, __in OUT_Leave& in)
{
	bool result = false;

	u_int bytelen=0;
	u_char result_login=0;

	bytelen = SIGNUP_PACKET_SIZE;
	result_login = in.result;
	
	out.len = bytelen;
	out.buf = new u_char[bytelen];

	u_int* bytelen_ptr=(u_int*)out.buf;
	u_char* result_login_ptr=((u_char*)bytelen_ptr)+4 +2;

	
	(*bytelen_ptr) = bytelen;
	memcpy(((u_char*)result_login_ptr)-2,spliter.c_str(),2);			//\r\n

	(*result_login_ptr) = result_login;
	memcpy(((u_char*)result_login_ptr)+1,spliter_end.c_str(),4);		//\r\n\r\n spliter_end
	
	result = true;
	return result;
}
const u_int SEARCH_PACKET_SIZE_BUT_CONTENT = (4+2)+(1+2)+(4+2)+(4+2)+4;
bool packetFromSearch(__out Memory& out, __in OUT_Search& in)
{
	bool result = false;

	result = true;
	return result;
}
bool packetToMore(__out Memory& out, __in OUT_More& in)
{
	bool result = false;

	result = true;
	return result;
}
bool packetFromSWriteComment(__out Memory& out, __in OUT_Write_comment& in)
{
	bool result = false;

	result = true;
	return result;
}
bool packetFromModifyComment(__out Memory& out, __in OUT_Modify_comment& in)
{
	bool result = false;

	result = true;
	return result;
}
bool packetFromDeleteComment(__out Memory& out, __in OUT_Delete_comment& in)
{
	bool result = false;

	result = true;
	return result;
}
bool packetFromLike(__out Memory& out, __in OUT_Like& in)
{
	bool result = false;

	result = true;
	return result;
}
bool packetFromReport(__out Memory& out, __in _OUT_Report& in)
{
	bool result = false;

	result = true;
	return result;
}