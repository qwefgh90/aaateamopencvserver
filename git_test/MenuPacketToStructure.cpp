#include "stdafx.h"
#include "MenuAnalyzer.h"

bool MenuAnalyzer::packetToLogin(__out IN_Login& out, __in Memory& memory)
{
	bool result = false;
	
	string str((char*)(memory.buf+5+2+2));
	vector<string> v= split(str,spliter);
	
	strcpy(out.ID,v[0].c_str());
	strcpy(out.pass,v[1].c_str());
	result = true;
	return result;
}
bool MenuAnalyzer::packetToSignup(__out IN_Signup& out, __in Memory& memory )
{
	bool result = false;
	
	string str((char*)(memory.buf+5+2+2));
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
	u_char* cookie = memory.buf+5+2+2;

	memcpy(out.cookie,cookie,64);
	result = true;
	return result;

}
bool MenuAnalyzer::packetToLeave(__out IN_Leave& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5+2+2;

	memcpy(out.cookie,cookie,64);
	result = true;
	return result;
}

const u_int SEARCH_SIZE_BUTIMAGE = 64+4+1+1+4+4 +2 +2 +2 +2 +2 +2;
bool MenuAnalyzer::packetToSearch(__out IN_Search& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;		//쿠키
	u_char* filter = cookie+64 +2;			//필터
	u_char* latitude = filter+1 +2;		//위도
	u_char* longitude = latitude+4 +2;		//경도
	u_char* image_buf = longitude+4 +2;	//image
	u_int	image_size = *((u_int*)memory.buf)-SEARCH_SIZE_BUTIMAGE;	//image size

	memcpy(out.cookie,cookie,64);
	out.filter = *filter;
	memcpy(&(out.latitude),latitude,4);
	memcpy(&(out.longitude),longitude,4);
	out.image.buf = new byte[image_size];
	out.image.len = image_size;
	memcpy(out.image.buf,image_buf,image_size);

	result = true;
	return result;
}

bool MenuAnalyzer::packetToMore(__out IN_More& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;
	u_int* code = (u_int*)(cookie+64 +2);
	u_char* opi_cnt = ((u_char*)code)+4 +2;
	u_char* sort = opi_cnt +1 +2;
	
	memcpy(out.cookie,cookie,64);
	memcpy(&(out.code),code,4);
	memcpy(&(out.comment_count),opi_cnt,1);
	memcpy(&(out.sort),sort,1);

	result = true;
	return result;
}
bool MenuAnalyzer::packetToWriteComment(__out IN_Write_comment& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;
	u_int* code = ((u_int*)cookie)+64 +2;
	u_char* opinion = ((u_char*)code)+4 +2;
	
	memcpy(out.cookie,cookie,64);
	memcpy(&(out.code),code,4);
	//char* pointer of comment
	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,"\r\n\r\n");
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		end_ptr[2]=NULL;
		end_ptr[3]=NULL;
	}else
	{
		goto END;
	}
	string str((char*)opinion);
	vector<string> v= split(str,spliter);//spliter를 기준으로 자른다 마지막엔 NULL이 있어야 하지만 존재 하지 않음, 임의로 NULL로 채워줌
	strcpy(out.comment,v[0].c_str());

	result = true;
	END:
	return result;
}
bool MenuAnalyzer::packetToModifyComment(__out IN_Modify_comment& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;
	u_int* num = ((u_int*)cookie)+64 +2;
	u_char* opnion_cnt = ((u_char*)num)+4 +2;
	u_char* opinion = opnion_cnt+1 +2;
	
	memcpy(out.cookie,cookie,64);
	memcpy(&(out.num),code,4);
	memcpy(&(out.code),code,4);
	//char* pointer of comment
	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,"\r\n\r\n");
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		end_ptr[2]=NULL;
		end_ptr[3]=NULL;
	}else
	{
		goto END;
	}
	string str((char*)opinion);
	vector<string> v= split(str,spliter);//spliter를 기준으로 자른다 마지막엔 NULL이 있어야 하지만 존재 하지 않음, 임의로 NULL로 채워줌
	strcpy(out.comment,v[0].c_str());

	result = true;
	END:
	return result;


	result= true;
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
bool MenuAnalyzer::packetToReport(__out IN_Report& out, __in Memory& memory )
{
	bool result = false;
	return result;

}
