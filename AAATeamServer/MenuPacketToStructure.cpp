#include "stdafx.h"
#include "MenuAnalyzer.h"

bool MenuAnalyzer::packetToLogin(__out IN_Login& out, __in Memory& memory)
{
	bool result = false;
	vector<string*> v;
	u_int packetlen = memory.len;
	/*
	if(memory.len<=5)
	{
		printf("%s","[ERR_packetToLogin] : INVALID ID/PASSWD PACKET\n");
		goto END;
	}
	*/
	
	//2)final spliter
	u_char* end_ptr = NULL;
	end_ptr = (u_char*)strstr((char*)(memory.buf+5),spliter_end.c_str());
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		end_ptr[2]=NULL;
		end_ptr[3]=NULL;
	}

	string str((char*)(memory.buf+5));
	//1)i d / p a s s w d
	split(v,str,spliter);

	if(v.size()<2)
	{
		printf("%s","[ERR_packetToLogin] : INVALID ID/PASSWD DATA\n");
		goto END;
	}
	strcpy(out.ID,v[0]->c_str());
	delete v[0];
	strcpy(out.pass,v[1]->c_str());
	delete v[1];
	v.clear();
	result = true;
	END:
	return result;
}
bool MenuAnalyzer::packetToSignup(__out IN_Signup& out, __in Memory& memory )
{
	bool result = false;
	vector<string*> v;
	
	//1)i d / p a s s w d
	
	//2)end spliter
	u_char* end_ptr = NULL;
	end_ptr = (u_char*)strstr((char*)(memory.buf+5),spliter_end.c_str());
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		end_ptr[2]=NULL;
		end_ptr[3]=NULL;
	}
	
	string str((char*)(memory.buf+5));
	//1)i d / p a s s w d
	split(v,str,spliter);

	if(v.size()<3)
	{
		printf("%s","[ERR_packetToSignup] packet data error\n");
		goto END;
	}

	strcpy(out.ID,v[0]->c_str());
	delete v[0];
	strcpy(out.pass,v[1]->c_str());
	delete v[1];
	strcpy(out.nick,v[2]->c_str());
	delete v[2];
	v.clear();
	result = true;
	END:
	return result;
}
bool MenuAnalyzer::packetToLogout(__out IN_Logout& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 ;

	memcpy(out.cookie,cookie,64);
	result = true;
	return result;

}
bool MenuAnalyzer::packetToLeave(__out IN_Leave& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 ;

	memcpy(out.cookie,cookie,64);
	result = true;
	return result;
}

const u_int SEARCH_SIZE_BUTIMAGE = (4)+ (1) + (64)+(1)+(4)+(4)  +4;
//데이터길이 + 타입 + 쿠키 + 필터 + 위도 + 경도 + 각각 구분자 + 마지막 구분자 +(이미지 크기 제외)
bool MenuAnalyzer::packetToSearch(__out IN_Search& out, __in Memory& memory )
{    
	bool result = false;
	u_char* cookie = memory.buf+5 ;		//쿠키
	u_char* filter = cookie+64 ;			//필터
	u_char* sort = (filter+1);
	float* latitude = (float*)(sort+1) ;		//위도
	float* longitude = (float*)((u_char*)latitude+4) ;		//경도
	u_char* image_buf = (u_char*)longitude+8 ;	//image
	u_int	image_size = *((u_int*)memory.buf)-SEARCH_SIZE_BUTIMAGE;	//image size // 이미지 끝의 구분자는 없음(마지막)
	    
	printf("[SEARCH]GPS Latitude : %f, Longitude : %f\n",*latitude,*longitude);
	memcpy(out.cookie,cookie,64);

	out.sort = *sort;
	out.filter = *filter;
	memcpy(&(out.store.latitude),latitude,4);
	memcpy(&(out.store.longitude),longitude,4);
	out.store.image.buf = new byte[image_size];
	out.store.image.len = image_size;
	memcpy(out.store.image.buf,image_buf,image_size);
	//__OK__BY Chang
	result = true;
	return result;
}

bool MenuAnalyzer::packetToMore(__out IN_More& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 ;
	u_int* code = (u_int*)(cookie+64 );
	u_char* sort = ((u_char*)code)+4 ;
	u_char* opi_cnt = sort +1 ;
	
	memcpy(out.cookie,cookie,64);
	memcpy(&(out.code),code,4);
	memcpy(&(out.sort),sort,1);
	memcpy(&(out.comment_count),opi_cnt,1);

	result = true;
	return result;
}
bool MenuAnalyzer::packetToWriteComment(__out IN_Write_comment& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 ;
	u_int* code = (u_int*)(cookie+64) ;
	u_char* sort = ((u_char*)code)+4;

	u_char* opi_score = ((u_char*)sort)+1;
	u_char* opinion = opi_score+3;
	string str;
	vector<string> v;

	memcpy_s(out.cookie,64,cookie,64);
	memcpy_s(&(out.code),4,code,4);
	memcpy(&(out.sort),sort,1);
	memcpy(&(out.comment_score),opi_score,1);

	//char* pointer of comment
	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,spliter_end.c_str());
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
	strcpy_s(out.comment,strlen((char*)opinion)+1,(char*)opinion);//400bytes buffer
	
	result = true;
	END:
	return result;
}
bool MenuAnalyzer::packetToModifyComment(__out IN_Modify_comment& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 ;
	u_int* code = (u_int*)(cookie+64) ;
	u_int* sort = ((u_int*)code)+4 ;
	u_char* comment_count = ((u_char*)sort)+1;
	u_char* comment_num = comment_count+1;

	u_char* opi_score = ((u_char*)comment_num)+1;
	u_char* opinion = opi_score+1;
	string str;
	vector<string> v;

	memcpy(out.cookie,cookie,64);
	memcpy(&(out.code),code,4);
	memcpy(&(out.comment_num),comment_num,4);
	memcpy(&(out.comment_count),comment_count,1);
	memcpy(&(out.sort),sort,1);

	memcpy(&(out.comment_score),opi_score,1);

	//char* pointer of comment
	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,spliter_end.c_str());
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
	strcpy_s(out.comment,strlen((char*)opinion)+1,(char*)opinion);//400bytes buffer
	
	result = true;
	END:
	return result;
}
bool MenuAnalyzer::packetToDeleteComment(__out IN_Delete_comment& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 ;
	u_int* code = (u_int*)(cookie+64) ;
	u_int* sort = ((u_int*)code)+4 ;
	u_char* comment_count = ((u_char*)sort)+1;
	u_char*  comment_num = comment_count+1;

	memcpy(out.cookie,cookie,64);
	memcpy(&(out.code),code,4);
	memcpy(&(out.sort),sort,1);
	memcpy(&(out.comment_count),comment_count,1);
	memcpy(&(out.comment_num),comment_num,4);

	result = true;
	END:
	return result;
}
bool MenuAnalyzer::packetToLike(__out IN_Like& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5 ;
	u_int* num = (u_int*)(cookie+64) ;		//글 번호
	u_char* like = ((u_char*)num)+4 ;			//글 개수
	
	memcpy_s(out.cookie,64,cookie,64);				//쿠키
	memcpy_s(&(out.num),4,num,4);					//글번호
	memcpy_s(&(out.like),1,like,1);					//좋아요 , 싫어요 구분값
	
	result = true;
	return result;
}
const u_int SEARCH_SIZE_BUTIMAGE_OPINION = (4)+ (1) + (64)+(1)+(4)+(4) +(1) +4 +2 +2;
//데이터길이 + 타입 + 쿠키 + 필터 + 위도 + 경도 + 의견점수 + 상점명 구분자  + 마지막 구분자 + (의견/이미지 제외)
bool MenuAnalyzer::packetToReport(__out IN_Report& out, __in Memory& memory )
{
	bool result = false;
	string str;

	u_char* cookie = memory.buf+5 ;
	u_char* filter = cookie+64 ;
	u_char* sort = (filter+1);
	float*	latitude = (float*)(sort+1 );
	float*	longitude= (float*)(((u_char*)latitude)+4 );
	printf("[REPORT]GPS Latitude : %f, Longitude : %f\n",*latitude,*longitude);

	char* storename = ((char*)longitude)+4;	//storename
	char* storename_splitor= strstr(storename,spliter.c_str());
	printf("storename length : %d",(int)(storename_splitor-storename));
	int storename_length = (int)(storename_splitor - storename);	//storename


	u_char* opinion_score = ((u_char*)storename)+storename_length+2;	//storename
	u_char* opinion = opinion_score+1 +2;//구분자 \r\n 포함
	
	//유효성 체크 : 의견필드
	if(*(char*)opinion==0 ||( *(char*)opinion == '\r' && *(char*)(opinion+1) == '\n'))
	{
		printf("%s '%c'\n","Opinion error : input value : 0x%x",(int)(*(char*)opinion));
		goto END;
	}


	u_int opinion_size=0;
	u_int image_size=0;
	u_char* image=NULL;
	
	memcpy(out.cookie,cookie,64);
	memcpy(&(out.filter),filter,1);				//필터
	out.sort = *sort;							//정렬값
	memcpy(&(out.store.latitude),latitude,4);			//위도
	memcpy(&(out.store.longitude),longitude,4);		//경도
	memcpy_s(out.store_name,storename_length,storename,storename_length);	//storename
	printf("storename : %s",(int)(out.store_name));

	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,spliter_end.c_str());	//\r\n포인터
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		end_ptr[2]=NULL;
		end_ptr[3]=NULL;

		opinion_size = ((u_char*)end_ptr-opinion);	//의견 길이
		printf("Opinion length : %ld\n",opinion_size);
		image = ((u_char*)(end_ptr+4));				//이미지 포인터
		//데이터의 길이는 little endian 으로 저장되어 있어야함.
		image_size = *((u_int*)memory.buf) - (SEARCH_SIZE_BUTIMAGE_OPINION + (opinion_size) + storename_length);//의견+점수 포함//storename
		printf("Image length : %ld\n",image_size);
	}else
	{
		goto END;
	}

	strcpy_s(out.comment,strlen((char*)opinion)+1,(char*)opinion);		//글 내용 (현재 프로토콜에선 글 내용만 0번 인덱스로 나오게 된다)
	out.comment_score = *opinion_score;		//글 평점

	//이미지 복사
	out.store.image.buf=new byte[image_size];		//추후 처리 후 할당해제 필요
	memcpy(out.store.image.buf,image,image_size);
	out.store.image.len = image_size;
	
	result = true;
	END:
	return result;

}
bool MenuAnalyzer::packetToCache(__out IN_Cache& out, __in Memory& memory){
	bool result = false;
	u_char* cookie = memory.buf+5;
	float* latitude = (float*)(cookie + 64);
	float* longitude = (float*)((u_char*)latitude + 4);
	
	if(memcpy_s(out.cookie,64*sizeof(u_char),cookie,64*sizeof(u_char)))
	{
		printf_s("s\n","memcpy_s() is failed");
		goto END;
	}else{}
	//성공
	out.latitude = *latitude;
	out.longitude = * longitude;

	result = true;
END:
	return result;
}
bool MenuAnalyzer::packetToChangePass(__out IN_Chpw&, __in Memory& memory)
{
		bool result = false;
	return result;
}