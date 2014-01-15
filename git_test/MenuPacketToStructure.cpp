#include "stdafx.h"
#include "MenuAnalyzer.h"

bool MenuAnalyzer::packetToLogin(__out IN_Login& out, __in Memory& memory)
{
	bool result = false;
	
	string str((char*)(memory.buf+5));
	vector<string> v= split(str,spliter);
	
	strcpy(out.ID,v[0].c_str());
	strcpy(out.pass,v[1].c_str());
	result = true;
	return result;
}
bool MenuAnalyzer::packetToSignup(__out IN_Signup& out, __in Memory& memory )
{
	bool result = false;
	
	string str((char*)(memory.buf+5));
	vector<string> v= split(str,spliter);
	
	strcpy(out.ID,v[0].c_str());
	strcpy(out.pass,v[1].c_str());
	strcpy(out.nick,v[2].c_str());
	result = true;
	return result;
}
bool MenuAnalyzer::packetToLogout(__out IN_Logout& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5;

	memcpy(out.cookie,cookie,64);
	result = true;
	return result;

}
bool MenuAnalyzer::packetToLeave(__out IN_Leave& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5;

	memcpy(out.cookie,cookie,64);
	result = true;
	return result;
}
bool MenuAnalyzer::packetToSearch(__out IN_Search& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5;		//쿠키
	u_char* filter = cookie+64;			//필터
	u_char* latitude = filter+1;		//위도
	u_char* longitude = latitude+4;		//경도
	u_char* image_buf = longitude+4;	//image
	#define SIZE_BUTIMAGE 64+4+1+1+4+4
	u_int	image_size = *((u_int*)memory.buf)-SIZE_BUTIMAGE;	//image size
	memcpy(out.cookie,cookie,64);
	out.filter = *filter;
	out.latitude = latitude[0]<<24+latitude[1]<<16+latitude[2]<<8+latitude[3];
	out.longitude = longitude[0]<<24+longitude[1]<<16+longitude[2]<<8+longitude[3];
	memcpy(out.image.buf,image_buf,image_size);
	out.image.len = image_size;

	result = true;
	return result;
}

bool MenuAnalyzer::packetToMore(__out IN_More& out, __in Memory& memory )
{
	bool result = false;
	return result;

}
bool MenuAnalyzer::packetToWriteComment(__out IN_Write_comment& out, __in Memory& memory )
{
	bool result = false;
	return result;

}
bool MenuAnalyzer::packetToModifyComment(__out IN_Modify_comment& out, __in Memory& memory )
{
	bool result = false;
	return result;

}
bool MenuAnalyzer::packetToDeleteComment(__out IN_Delete_comment& out, __in Memory& memory )
{
	bool result = false;
	return result;

}
bool MenuAnalyzer::packetToLike(__out IN_Like& out, __in Memory& memory )
{
	bool result = false;
	return result;

}
bool MenuAnalyzer::packetToDislike(__out IN_Dislike& out, __in Memory& memory )
{
	bool result = false;
	return result;

}
bool MenuAnalyzer::packetToReport(__out IN_Report& out, __in Memory& memory )
{
	bool result = false;
	return result;

}
