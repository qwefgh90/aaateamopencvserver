#include "stdafx.h"
#include "MenuAnalyzer.h"

bool MenuAnalyzer::packetToLogin(__out IN_Login& out, __in Memory& memory)
{
	bool result = false;
	
	string str((char*)(memory.buf+5 +2+2));
	vector<string> v= split(str,spliter);
	
	u_char* end_ptr = (u_char*)strstr((char*)(memory.buf+5 +2+2),spliter_end.c_str());
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		end_ptr[2]=NULL;
		end_ptr[3]=NULL;
	}

	strcpy(out.ID,v[0].c_str());
	strcpy(out.pass,v[1].c_str());
	result = true;
	return result;
}
bool MenuAnalyzer::packetToSignup(__out IN_Signup& out, __in Memory& memory )
{
	bool result = false;
	
	string str((char*)(memory.buf+5 +2+2));
	vector<string> v= split(str,spliter);
	
	u_char* end_ptr = (u_char*)strstr((char*)(memory.buf+5 +2+2),spliter_end.c_str());
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		end_ptr[2]=NULL;
		end_ptr[3]=NULL;
	}

	strcpy(out.ID,v[0].c_str());
	strcpy(out.pass,v[1].c_str());
	strcpy(out.nick,v[2].c_str());
	result = true;
	return result;
}
bool MenuAnalyzer::packetToLogout(__out IN_Logout& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;

	memcpy(out.cookie,cookie,64);
	result = true;
	return result;

}
bool MenuAnalyzer::packetToLeave(__out IN_Leave& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;

	memcpy(out.cookie,cookie,64);
	result = true;
	return result;
}

const u_int SEARCH_SIZE_BUTIMAGE = (4+2)+ (1+2) + (64+2)+(1+2)+(4+2)+(4+2)+4;
//데이터길이 + 타입 + 쿠키 + 필터 + 위도 + 경도 + 각각 구분자 + 마지막 구분자 +(이미지 크기 제외)
bool MenuAnalyzer::packetToSearch(__out IN_Search& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;		//쿠키
	u_char* filter = cookie+64 +2;			//필터
	u_char* latitude = filter+1 +2;		//위도
	u_char* longitude = latitude+4 +2;		//경도
	u_char* image_buf = longitude+4 +2;	//image
	u_int	image_size = *((u_int*)memory.buf)-SEARCH_SIZE_BUTIMAGE;	//image size // 이미지 끝의 구분자는 없음(마지막)

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
	string str;
	vector<string> v;

	memcpy(out.cookie,cookie,64);
	memcpy(&(out.code),code,4);
	//char* pointer of comment
	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,spliter.c_str());
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		out.comment_score = *((u_char*)(end_ptr+2));	//등록 평점
	}else
	{
		goto END;
	}
	str = string((char*)opinion);
	v= split(str,spliter);//spliter를 기준으로 자른다 마지막엔 NULL이 있어야 하지만 존재 하지 않음, 임의로 NULL로 채워줌
	strcpy(out.comment,v[0].c_str());
	
	result = true;
	END:
	return result;
}
bool MenuAnalyzer::packetToModifyComment(__out IN_Modify_comment& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;
	u_int* num = ((u_int*)cookie)+64 +2;		//글 번호
	u_char* opnion_cnt = ((u_char*)num)+4 +2;	//글 개수
	u_char* opinion = opnion_cnt+1 +2;			//의견

	string str;
	vector<string> v;

	memcpy(out.cookie,cookie,64);
	memcpy(&(out.num),num,4);					//글번호
	memcpy(&(out.comment_count),opnion_cnt,1);	//글갯수
	
	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,"\r\n");
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		out.comment_score = *((u_char*)(end_ptr+2));	//수정 평점
	}else
	{
		goto END;
	}
	str= string((char*)opinion);
	v= split(str,spliter);	//spliter를 기준으로 자른다 마지막엔 NULL이 있어야 하지만 존재 하지 않음, 임의로 NULL로 채워줌
	strcpy(out.comment,v[0].c_str());		//글 내용

	result = true;
	END:
	return result;
}
bool MenuAnalyzer::packetToDeleteComment(__out IN_Delete_comment& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;
	u_int* num = ((u_int*)cookie)+64 +2;		//글 번호
	u_char* opnion_cnt = ((u_char*)num)+4 +2;	//글 개수
	
	memcpy(out.cookie,cookie,64);
	memcpy(&(out.num),num,4);					//글번호
	memcpy(&(out.comment_count),opnion_cnt,1);	//글갯수
	
	result = true;
	return result;
}
bool MenuAnalyzer::packetToLike(__out IN_Like& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 +2+2;
	u_int* num = ((u_int*)cookie)+64 +2;		//글 번호
	u_char* like = ((u_char*)num)+4 +2;			//글 개수
	
	memcpy(out.cookie,cookie,64);
	memcpy(&(out.num),num,4);					//글번호
	memcpy(&(out.like),like,1);					//좋아요 , 싫어요
	
	result = true;
	return result;
}
const u_int SEARCH_SIZE_BUTIMAGE_OPINION = (4+2)+ (1+2) + (64+2)+(1+2)+(4+2)+(4+2) +4;
//데이터길이 + 타입 + 쿠키 + 필터 + 위도 + 경도 + 각각 구분자 + 마지막 구분자 + (의견+2/점수+2/이미지 제외)
bool MenuAnalyzer::packetToReport(__out IN_Report& out, __in Memory& memory )
{
	bool result = false;
	string str;
	vector<string> v;

	u_char* cookie = memory.buf+5 +2+2;
	u_char* filter = cookie+64 +2;
	float*	latitude =((float*)(filter+1  +2));
	float*	longitude= (float*)(((u_char*)latitude)+4 +2);
	u_char* opinion = ((u_char*)longitude)+4 +2;

	u_int	opinion_size = 0;
	u_char* opinion_socre = NULL;
	u_char* image = NULL;
	u_int	image_size = 0;

	memcpy(out.cookie,cookie,64);
	memcpy(&(out.filter),filter,1);				//필터
	memcpy(&(out.latitude),latitude,4);			//위도
	memcpy(&(out.longitude),longitude,4);		//경도

	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,"\r\n");	//\r\n포인터
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		opinion_size = ((u_char*)end_ptr-opinion);	//의견 길이

		opinion_socre = ((u_char*)(end_ptr+2));		//평점
		image = opinion_socre+1 +2;					//이미지 포인터
		//데이터의 길이는 little endian 으로 저장되어 있어야함.
		image_size = *((u_int*)memory.buf) - (SEARCH_SIZE_BUTIMAGE_OPINION + (opinion_size +2   +1 +2));//의견+점수 포함
	}else
	{
		goto END;
	}

	str = string((char*)opinion);
	v = split(str,spliter);	//spliter를 기준으로 자른다 마지막엔 NULL이 있어야 하지만 존재 하지 않음, 임의로 NULL로 채워줌
	strcpy(out.comment,v[0].c_str());		//글 내용 (현재 프로토콜에선 글 내용만 0번 인덱스로 나오게 된다)
	out.comment_score = *opinion_socre;		//글 평점

	//이미지 복사
	out.image.buf=new byte[image_size];		//추후 처리 후 할당해제 필요
	memcpy(out.image.buf,image,image_size);
	out.image.len = image_size;
	
	result = true;
	END:
	return result;

}
