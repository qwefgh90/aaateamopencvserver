#include "stdafx.h"
#include "MenuAnalyzer.h"

const u_int LOGIN_PACKET_SIZE_BUT_NICKNAME = (4)+(1)+(64)+4;
	//len + result + cookie + (exclude nick)
bool MenuAnalyzer::packetFromLogin(__out Memory& out, __in OUT_Login& in)
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
	u_char* result_login_ptr=((u_char*)bytelen_ptr)+4 ;
	u_char* cookie_ptr=result_login_ptr+1 ;
	u_char* nick_ptr=cookie_ptr+64 ;
	//copy struct to new buf
	
	(*bytelen_ptr) = bytelen;

	(*result_login_ptr) = result_login;

	memcpy(cookie_ptr,cookie,64);

	memcpy(nick_ptr,nick,nick_len);
	memcpy(((u_char*)nick_ptr)+nick_len,spliter_end.c_str(),4);	//\r\n\r\n spliter_end

	result = true;
	return result;
}
const u_int SIGNUP_PACKET_SIZE = (4)+(1)+4;
	//len + result
bool MenuAnalyzer::packetFromSignup(__out Memory& out, __in OUT_Signup& in)
{
	bool result = false;

	u_int bytelen=0;
	u_char result_login=0;

	bytelen = SIGNUP_PACKET_SIZE;
	result_login = in.result;
	
	out.len = bytelen;
	out.buf = new u_char[bytelen];

	u_int* bytelen_ptr=(u_int*)out.buf;
	u_char* result_login_ptr=((u_char*)bytelen_ptr)+4;
	u_char* end_spliter = result_login_ptr+1;
	
	(*bytelen_ptr) = bytelen;

	(*result_login_ptr) = result_login;
	memcpy(end_spliter,spliter_end.c_str(),4);	//\r\n\r\n spliter_end
	
	result = true;
	return result;
}
bool MenuAnalyzer::packetFromLogout(__out Memory& out, __in OUT_Logout& in)
{
	bool result = false;

	u_int bytelen=0;
	u_char result_login=0;

	bytelen = SIGNUP_PACKET_SIZE;
	result_login = in.result;
	
	out.len = bytelen;
	out.buf = new u_char[bytelen];

	u_int* bytelen_ptr=(u_int*)out.buf;
	u_char* result_login_ptr=((u_char*)bytelen_ptr)+4;
	u_char* end_spliter = result_login_ptr+1;
	
	(*bytelen_ptr) = bytelen;

	(*result_login_ptr) = result_login;
	memcpy(end_spliter,spliter_end.c_str(),4);	//\r\n\r\n spliter_end
	
	result = true;
	return result;
}
bool MenuAnalyzer::packetFromLeave(__out Memory& out, __in OUT_Leave& in)
{
	bool result = false;

	u_int bytelen=0;
	u_char result_login=0;

	bytelen = SIGNUP_PACKET_SIZE;
	result_login = in.result;
	
	out.len = bytelen;
	out.buf = new u_char[bytelen];

	u_int* bytelen_ptr=(u_int*)out.buf;
	u_char* result_login_ptr=((u_char*)bytelen_ptr)+4;
	u_char* end_spliter = result_login_ptr+1;
	
	(*bytelen_ptr) = bytelen;

	(*result_login_ptr) = result_login;
	memcpy(end_spliter,spliter_end.c_str(),4);	//\r\n\r\n spliter_end
	
	result = true;
	return result;
}
const u_int SEARCH_PACKET_SIZE_BUT_CONTENT = (4)+(1)+(4)+(4)+2;
												//마지막 \r\n하나빼고..
bool MenuAnalyzer::packetFromSearch(__out Memory& out, __in OUT_Search& in)
{
	bool result = false;

	char big_buffer[1000]={0,};							//temp buffer

	u_int content_cnt = in.opi_cnt;						//contents count
	u_int bytelen = SEARCH_PACKET_SIZE_BUT_CONTENT ;	//init byte size
	
	//content structure size values (contain "\r\n") 
	u_int num_size[10]={0,};
	u_int content_size[10]={0,};
	u_int nic_size[10]={0,};
	u_int like_cnt_size[10]={0,};
	u_int dislike_cnt_size[10]={0,};

	//calculate content byte length
	int i = 0;
	for(i=0 ; i<content_cnt ; i++){
		OUT_Opinion* opi = in.opi+i;

		char num[12]={0,};	//글번호
		sprintf(num,"%u\r\n",opi->sns_id);
		num_size[i]=strlen(num);

		char* content=opi->comment;	//글내용 (문자열)
		content_size[i]=strlen(content)+2;

		char* nic=opi->nick;		//닉네임 (문자열)
		nic_size[i]=strlen(nic)+2;

		char  like_cnt[12]={0,};	//좋아요
		sprintf(like_cnt,"%u\r\n",opi->like_cnt);
		like_cnt_size[i]=strlen(like_cnt);

		char dislike_cnt[12]={0,};//싫어요
		sprintf(dislike_cnt,"%u\r\n",opi->dislike_cnt);
		dislike_cnt_size[i]=strlen(dislike_cnt);

		bytelen += (num_size[i]+ content_size[i]+ nic_size[i]+ like_cnt_size[i]+ dislike_cnt_size[i]);
	}
	if (content_cnt == 0)
	{//if no opinion exist, add 2byte for len + spliter
		bytelen += 2;
	}
	//create buf and copy data
	out.buf = new u_char[bytelen];
	out.len = bytelen;
	//1)point memory buffer
	u_int* bytelen_ptr = (u_int*)out.buf;
	u_char* result_login_ptr = (u_char*)bytelen_ptr+4;
	u_int* code_ptr = (u_int*)(result_login_ptr+1);
	float* avg_score_ptr = (float*)((u_char*)code_ptr+4);
	//2)copy to packet buffer
	*bytelen_ptr = bytelen;
	*result_login_ptr = in.result;
	*code_ptr = in.code;
	*avg_score_ptr = in.score;

	//opinion pointer array variables 
	char* num_ptr[10]={0,};
	char* nic_ptr[10]={0,};
	char* conten_ptr[10]={0,};
	char* like_ptr[10]={0,};
	char* dislike_ptr[10]={0,};

	unsigned char* local_end_spliter = NULL;
	
	for(i=0 ; i<content_cnt ; i++){
		//char buffers...
		if(i==0)
			num_ptr[i]=((char*)avg_score_ptr)+4;
		else // not zero
			num_ptr[i]=((char*)dislike_ptr[i-1])+dislike_cnt_size[i-1];

		nic_ptr[i]=((char*)num_ptr[i])+num_size[i];
		conten_ptr[i]=((char*)nic_ptr[i])+nic_size[i];
		like_ptr[i]=((char*)conten_ptr[i])+content_size[i];
		dislike_ptr[i]=((char*)like_ptr[i])+like_cnt_size[i];
		////////////////memory copy
		sprintf(big_buffer,"%u\r\n",(in.opi+i)->sns_id);
		memcpy(num_ptr[i],big_buffer,num_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->nick);
		memcpy(nic_ptr[i],big_buffer,nic_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->comment);
		memcpy(conten_ptr[i],big_buffer,content_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->like_cnt);
		memcpy(like_ptr[i],big_buffer,like_cnt_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->dislike_cnt);
		memcpy(dislike_ptr[i],big_buffer,dislike_cnt_size[i]);
	}

	if(content_cnt != 0)
	{
		local_end_spliter = (u_char*)((u_char*)dislike_ptr[content_cnt-1]+dislike_cnt_size[content_cnt-1]);
	}else{
		local_end_spliter = ((u_char*)avg_score_ptr)+4;
	}
	memcpy(local_end_spliter,spliter_end.c_str(),4);

	result = true;
	return result;
}
const u_int MORE_PACKET_SIZE_BUT_CONTENT = (4)+(1)+(4)+2;
												//마지막 \r\n하나빼고..
bool MenuAnalyzer::packetFromMore(__out Memory& out, __in OUT_More& in)
{
		bool result = false;

	char big_buffer[1000]={0,};							//temp buffer

	u_int content_cnt = in.opi_cnt;						//contents count
	u_int bytelen = MORE_PACKET_SIZE_BUT_CONTENT ;	//init byte size
	
	//content structure size values (contain "\r\n") 
	u_int num_size[10]={0,};
	u_int content_size[10]={0,};
	u_int nic_size[10]={0,};
	u_int like_cnt_size[10]={0,};
	u_int dislike_cnt_size[10]={0,};

	//calculate content byte length
	int i = 0;
	for(i=0 ; i<content_cnt ; i++){
		OUT_Opinion* opi = in.opi+i;

		char num[12]={0,};	//글번호
		sprintf(num,"%u\r\n",opi->sns_id);
		num_size[i]=strlen(num);

		char* content=opi->comment;	//글내용 (문자열)
		content_size[i]=strlen(content)+2;

		char* nic=opi->nick;		//닉네임 (문자열)
		nic_size[i]=strlen(nic)+2;

		char  like_cnt[12]={0,};	//좋아요
		sprintf(like_cnt,"%u\r\n",opi->like_cnt);
		like_cnt_size[i]=strlen(like_cnt);

		char dislike_cnt[12]={0,};//싫어요
		sprintf(dislike_cnt,"%u\r\n",opi->dislike_cnt);
		dislike_cnt_size[i]=strlen(dislike_cnt);

		bytelen += (num_size[i]+ content_size[i]+ nic_size[i]+ like_cnt_size[i]+ dislike_cnt_size[i]);
	}
	if (content_cnt == 0)
	{//if no opinion exist, add 2byte for len + spliter
		bytelen += 2;
	}
	//create buf and copy data
	out.buf = new u_char[bytelen];
	out.len = bytelen;
	//1)point memory buffer
	u_int* bytelen_ptr = (u_int*)out.buf;
	u_char* result_login_ptr = (u_char*)bytelen_ptr+4;
	float* avg_score_ptr = (float*)(result_login_ptr+1);
	//2)copy to packet buffer
	*bytelen_ptr = bytelen;
	*result_login_ptr = in.result;
	*avg_score_ptr = in.score;

	//opinion pointer array variables 
	char* num_ptr[10]={0,};
	char* nic_ptr[10]={0,};
	char* conten_ptr[10]={0,};
	char* like_ptr[10]={0,};
	char* dislike_ptr[10]={0,};

	unsigned char* local_end_spliter = NULL;
	
	for(i=0 ; i<content_cnt ; i++){
		//char buffers...
		if(i==0)
			num_ptr[i]=((char*)avg_score_ptr)+4;
		else // not zero
			num_ptr[i]=((char*)dislike_ptr[i-1])+dislike_cnt_size[i-1];

		nic_ptr[i]=((char*)num_ptr[i])+num_size[i];
		conten_ptr[i]=((char*)nic_ptr[i])+nic_size[i];
		like_ptr[i]=((char*)conten_ptr[i])+content_size[i];
		dislike_ptr[i]=((char*)like_ptr[i])+like_cnt_size[i];
		////////////////memory copy
		sprintf(big_buffer,"%u\r\n",(in.opi+i)->sns_id);
		memcpy(num_ptr[i],big_buffer,num_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->nick);
		memcpy(nic_ptr[i],big_buffer,nic_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->comment);
		memcpy(conten_ptr[i],big_buffer,content_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->like_cnt);
		memcpy(like_ptr[i],big_buffer,like_cnt_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->dislike_cnt);
		memcpy(dislike_ptr[i],big_buffer,dislike_cnt_size[i]);
	}

	if(content_cnt != 0)
	{
		local_end_spliter = (u_char*)((u_char*)dislike_ptr[content_cnt-1]+dislike_cnt_size[content_cnt-1]);
	}else{
		local_end_spliter = ((u_char*)avg_score_ptr)+4;
	}
	memcpy(local_end_spliter,spliter_end.c_str(),4);

	result = true;
	return result;
}
bool MenuAnalyzer::packetFromWriteComment(__out Memory& out, __in OUT_Write_comment& in)
{
			bool result = false;

	char big_buffer[1000]={0,};							//temp buffer

	u_int content_cnt = in.opi_cnt;						//contents count
	u_int bytelen = MORE_PACKET_SIZE_BUT_CONTENT ;	//init byte size
	
	//content structure size values (contain "\r\n") 
	u_int num_size[10]={0,};
	u_int content_size[10]={0,};
	u_int nic_size[10]={0,};
	u_int like_cnt_size[10]={0,};
	u_int dislike_cnt_size[10]={0,};

	//calculate content byte length
	int i = 0;
	for(i=0 ; i<content_cnt ; i++){
		OUT_Opinion* opi = in.opi+i;

		char num[12]={0,};	//글번호
		sprintf(num,"%u\r\n",opi->sns_id);
		num_size[i]=strlen(num);

		char* content=opi->comment;	//글내용 (문자열)
		content_size[i]=strlen(content)+2;

		char* nic=opi->nick;		//닉네임 (문자열)
		nic_size[i]=strlen(nic)+2;

		char  like_cnt[12]={0,};	//좋아요
		sprintf(like_cnt,"%u\r\n",opi->like_cnt);
		like_cnt_size[i]=strlen(like_cnt);

		char dislike_cnt[12]={0,};//싫어요
		sprintf(dislike_cnt,"%u\r\n",opi->dislike_cnt);
		dislike_cnt_size[i]=strlen(dislike_cnt);

		bytelen += (num_size[i]+ content_size[i]+ nic_size[i]+ like_cnt_size[i]+ dislike_cnt_size[i]);
	}
	if (content_cnt == 0)
	{//if no opinion exist, add 2byte for len + spliter
		bytelen += 2;
	}
	//create buf and copy data
	out.buf = new u_char[bytelen];
	out.len = bytelen;
	//1)point memory buffer
	u_int* bytelen_ptr = (u_int*)out.buf;
	u_char* result_login_ptr = (u_char*)bytelen_ptr+4;
	float* avg_score_ptr = (float*)(result_login_ptr+1);
	//2)copy to packet buffer
	*bytelen_ptr = bytelen;
	*result_login_ptr = in.result;
	*avg_score_ptr = in.score;

	//opinion pointer array variables 
	char* num_ptr[10]={0,};
	char* nic_ptr[10]={0,};
	char* conten_ptr[10]={0,};
	char* like_ptr[10]={0,};
	char* dislike_ptr[10]={0,};

	unsigned char* local_end_spliter = NULL;
	
	for(i=0 ; i<content_cnt ; i++){
		//char buffers...
		if(i==0)
			num_ptr[i]=((char*)avg_score_ptr)+4;
		else // not zero
			num_ptr[i]=((char*)dislike_ptr[i-1])+dislike_cnt_size[i-1];

		nic_ptr[i]=((char*)num_ptr[i])+num_size[i];
		conten_ptr[i]=((char*)nic_ptr[i])+nic_size[i];
		like_ptr[i]=((char*)conten_ptr[i])+content_size[i];
		dislike_ptr[i]=((char*)like_ptr[i])+like_cnt_size[i];
		////////////////memory copy
		sprintf(big_buffer,"%u\r\n",(in.opi+i)->sns_id);
		memcpy(num_ptr[i],big_buffer,num_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->nick);
		memcpy(nic_ptr[i],big_buffer,nic_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->comment);
		memcpy(conten_ptr[i],big_buffer,content_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->like_cnt);
		memcpy(like_ptr[i],big_buffer,like_cnt_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->dislike_cnt);
		memcpy(dislike_ptr[i],big_buffer,dislike_cnt_size[i]);
	}

	if(content_cnt != 0)
	{
		local_end_spliter = (u_char*)((u_char*)dislike_ptr[content_cnt-1]+dislike_cnt_size[content_cnt-1]);
	}else{
		local_end_spliter = ((u_char*)avg_score_ptr)+4;
	}
	memcpy(local_end_spliter,spliter_end.c_str(),4);

	result = true;
	return result;
}
bool MenuAnalyzer::packetFromModifyComment(__out Memory& out, __in OUT_Modify_comment& in)
{
			bool result = false;

	char big_buffer[1000]={0,};							//temp buffer

	u_int content_cnt = in.opi_cnt;						//contents count
	u_int bytelen = MORE_PACKET_SIZE_BUT_CONTENT ;	//init byte size
	
	//content structure size values (contain "\r\n") 
	u_int num_size[10]={0,};
	u_int content_size[10]={0,};
	u_int nic_size[10]={0,};
	u_int like_cnt_size[10]={0,};
	u_int dislike_cnt_size[10]={0,};

	//calculate content byte length
	int i = 0;
	for(i=0 ; i<content_cnt ; i++){
		OUT_Opinion* opi = in.opi+i;

		char num[12]={0,};	//글번호
		sprintf(num,"%u\r\n",opi->sns_id);
		num_size[i]=strlen(num);

		char* content=opi->comment;	//글내용 (문자열)
		content_size[i]=strlen(content)+2;

		char* nic=opi->nick;		//닉네임 (문자열)
		nic_size[i]=strlen(nic)+2;

		char  like_cnt[12]={0,};	//좋아요
		sprintf(like_cnt,"%u\r\n",opi->like_cnt);
		like_cnt_size[i]=strlen(like_cnt);

		char dislike_cnt[12]={0,};//싫어요
		sprintf(dislike_cnt,"%u\r\n",opi->dislike_cnt);
		dislike_cnt_size[i]=strlen(dislike_cnt);

		bytelen += (num_size[i]+ content_size[i]+ nic_size[i]+ like_cnt_size[i]+ dislike_cnt_size[i]);
	}
	if (content_cnt == 0)
	{//if no opinion exist, add 2byte for len + spliter
		bytelen += 2;
	}
	//create buf and copy data
	out.buf = new u_char[bytelen];
	out.len = bytelen;
	//1)point memory buffer
	u_int* bytelen_ptr = (u_int*)out.buf;
	u_char* result_login_ptr = (u_char*)bytelen_ptr+4;
	float* avg_score_ptr = (float*)(result_login_ptr+1);
	//2)copy to packet buffer
	*bytelen_ptr = bytelen;
	*result_login_ptr = in.result;
	*avg_score_ptr = in.score;

	//opinion pointer array variables 
	char* num_ptr[10]={0,};
	char* nic_ptr[10]={0,};
	char* conten_ptr[10]={0,};
	char* like_ptr[10]={0,};
	char* dislike_ptr[10]={0,};

	unsigned char* local_end_spliter = NULL;
	
	for(i=0 ; i<content_cnt ; i++){
		//char buffers...
		if(i==0)
			num_ptr[i]=((char*)avg_score_ptr)+4;
		else // not zero
			num_ptr[i]=((char*)dislike_ptr[i-1])+dislike_cnt_size[i-1];

		nic_ptr[i]=((char*)num_ptr[i])+num_size[i];
		conten_ptr[i]=((char*)nic_ptr[i])+nic_size[i];
		like_ptr[i]=((char*)conten_ptr[i])+content_size[i];
		dislike_ptr[i]=((char*)like_ptr[i])+like_cnt_size[i];
		////////////////memory copy
		sprintf(big_buffer,"%u\r\n",(in.opi+i)->sns_id);
		memcpy(num_ptr[i],big_buffer,num_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->nick);
		memcpy(nic_ptr[i],big_buffer,nic_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->comment);
		memcpy(conten_ptr[i],big_buffer,content_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->like_cnt);
		memcpy(like_ptr[i],big_buffer,like_cnt_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->dislike_cnt);
		memcpy(dislike_ptr[i],big_buffer,dislike_cnt_size[i]);
	}

	if(content_cnt != 0)
	{
		local_end_spliter = (u_char*)((u_char*)dislike_ptr[content_cnt-1]+dislike_cnt_size[content_cnt-1]);
	}else{
		local_end_spliter = ((u_char*)avg_score_ptr)+4;
	}
	memcpy(local_end_spliter,spliter_end.c_str(),4);

	result = true;
	return result;
}
bool MenuAnalyzer::packetFromDeleteComment(__out Memory& out, __in OUT_Delete_comment& in)
{
			bool result = false;

	char big_buffer[1000]={0,};							//temp buffer

	u_int content_cnt = in.opi_cnt;						//contents count
	u_int bytelen = MORE_PACKET_SIZE_BUT_CONTENT ;	//init byte size
	
	//content structure size values (contain "\r\n") 
	u_int num_size[10]={0,};
	u_int content_size[10]={0,};
	u_int nic_size[10]={0,};
	u_int like_cnt_size[10]={0,};
	u_int dislike_cnt_size[10]={0,};

	//calculate content byte length
	int i = 0;
	for(i=0 ; i<content_cnt ; i++){
		OUT_Opinion* opi = in.opi+i;

		char num[12]={0,};	//글번호
		sprintf(num,"%u\r\n",opi->sns_id);
		num_size[i]=strlen(num);

		char* content=opi->comment;	//글내용 (문자열)
		content_size[i]=strlen(content)+2;

		char* nic=opi->nick;		//닉네임 (문자열)
		nic_size[i]=strlen(nic)+2;

		char  like_cnt[12]={0,};	//좋아요
		sprintf(like_cnt,"%u\r\n",opi->like_cnt);
		like_cnt_size[i]=strlen(like_cnt);

		char dislike_cnt[12]={0,};//싫어요
		sprintf(dislike_cnt,"%u\r\n",opi->dislike_cnt);
		dislike_cnt_size[i]=strlen(dislike_cnt);

		bytelen += (num_size[i]+ content_size[i]+ nic_size[i]+ like_cnt_size[i]+ dislike_cnt_size[i]);
	}
	if (content_cnt == 0)
	{//if no opinion exist, add 2byte for len + spliter
		bytelen += 2;
	}
	//create buf and copy data
	out.buf = new u_char[bytelen];
	out.len = bytelen;
	//1)point memory buffer
	u_int* bytelen_ptr = (u_int*)out.buf;
	u_char* result_login_ptr = (u_char*)bytelen_ptr+4;
	float* avg_score_ptr = (float*)(result_login_ptr+1);
	//2)copy to packet buffer
	*bytelen_ptr = bytelen;
	*result_login_ptr = in.result;
	*avg_score_ptr = in.score;

	//opinion pointer array variables 
	char* num_ptr[10]={0,};
	char* nic_ptr[10]={0,};
	char* conten_ptr[10]={0,};
	char* like_ptr[10]={0,};
	char* dislike_ptr[10]={0,};

	unsigned char* local_end_spliter = NULL;
	
	for(i=0 ; i<content_cnt ; i++){
		//char buffers...
		if(i==0)
			num_ptr[i]=((char*)avg_score_ptr)+4;
		else // not zero
			num_ptr[i]=((char*)dislike_ptr[i-1])+dislike_cnt_size[i-1];

		nic_ptr[i]=((char*)num_ptr[i])+num_size[i];
		conten_ptr[i]=((char*)nic_ptr[i])+nic_size[i];
		like_ptr[i]=((char*)conten_ptr[i])+content_size[i];
		dislike_ptr[i]=((char*)like_ptr[i])+like_cnt_size[i];
		////////////////memory copy
		sprintf(big_buffer,"%u\r\n",(in.opi+i)->sns_id);
		memcpy(num_ptr[i],big_buffer,num_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->nick);
		memcpy(nic_ptr[i],big_buffer,nic_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->comment);
		memcpy(conten_ptr[i],big_buffer,content_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->like_cnt);
		memcpy(like_ptr[i],big_buffer,like_cnt_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->dislike_cnt);
		memcpy(dislike_ptr[i],big_buffer,dislike_cnt_size[i]);
	}

	if(content_cnt != 0)
	{
		local_end_spliter = (u_char*)((u_char*)dislike_ptr[content_cnt-1]+dislike_cnt_size[content_cnt-1]);
	}else{
		local_end_spliter = ((u_char*)avg_score_ptr)+4;
	}
	memcpy(local_end_spliter,spliter_end.c_str(),4);

	result = true;
	return result;
}
const u_int LIKE_PACKET_SIZE = 5;
												//마지막 \r\n하나빼고..
bool MenuAnalyzer::packetFromLike(__out Memory& out, __in OUT_Like& in)
{
	bool result = false;
	out.buf = new u_char[LIKE_PACKET_SIZE];
	out.len = LIKE_PACKET_SIZE;
	u_int* size = (u_int*)(out.buf);
	*size = LIKE_PACKET_SIZE;
	*((u_char*)(size+4)) = in.result;
	result = true;
	return result;
}
bool MenuAnalyzer::packetFromReport(__out Memory& out, __in _OUT_Report& in)
{
		bool result = false;

	char big_buffer[1000]={0,};							//temp buffer

	u_int content_cnt = in.opi_cnt;						//contents count
	u_int bytelen = SEARCH_PACKET_SIZE_BUT_CONTENT ;	//init byte size //마지막 \r\n 제외
	
	//content structure size values (contain "\r\n") 
	u_int num_size[10]={0,};
	u_int nic_size[10]={0,};
	u_int content_size[10]={0,};
	u_int like_cnt_size[10]={0,};
	u_int dislike_cnt_size[10]={0,};

	//calculate content byte length
	int i = 0;
	for(i=0 ; i<content_cnt ; i++){
		OUT_Opinion* opi = in.opi+i;

		char num[12]={0,};	//글번호
		sprintf(num,"%u\r\n",opi->sns_id);
		num_size[i]=strlen(num);

		char* content=opi->comment;	//글내용 (문자열)
		content_size[i]=strlen(content)+2;

		char* nic=opi->nick;		//닉네임 (문자열)
		nic_size[i]=strlen(nic)+2;

		char  like_cnt[12]={0,};	//좋아요
		sprintf(like_cnt,"%u\r\n",opi->like_cnt);
		like_cnt_size[i]=strlen(like_cnt);

		char dislike_cnt[12]={0,};//싫어요
		sprintf(dislike_cnt,"%u\r\n",opi->dislike_cnt);
		dislike_cnt_size[i]=strlen(dislike_cnt);

		bytelen += (num_size[i]+ content_size[i]+ nic_size[i]+ like_cnt_size[i]+ dislike_cnt_size[i]);
	}
	if (content_cnt == 0)
	{//if no opinion exist, add 2byte for len + spliter
		bytelen += 2;
	}
	//create buf and copy data
	out.buf = new u_char[bytelen];
	out.len = bytelen;
	//1)point memory buffer
	u_int* bytelen_ptr = (u_int*)out.buf;
	u_char* result_login_ptr = (u_char*)bytelen_ptr+4;
	u_int* code_ptr = (u_int*)(result_login_ptr+1);
	float* avg_score_ptr = (float*)((u_char*)code_ptr+4);
	//2)copy to packet buffer
	*bytelen_ptr = bytelen;
	*result_login_ptr = in.result;
	*code_ptr = in.code;
	*avg_score_ptr = in.score;

	//opinion pointer array variables 
	char* num_ptr[10]={0,};
	char* nic_ptr[10]={0,};
	char* conten_ptr[10]={0,};
	char* like_ptr[10]={0,};
	char* dislike_ptr[10]={0,};

	unsigned char* local_end_spliter = NULL;
	
	for(i=0 ; i<content_cnt ; i++){
		//char buffers...
		if(i==0)
			num_ptr[i]=((char*)avg_score_ptr)+4;
		else // not zero
			num_ptr[i]=((char*)dislike_ptr[i-1])+dislike_cnt_size[i-1];

		nic_ptr[i]=((char*)num_ptr[i])+num_size[i];
		conten_ptr[i]=((char*)nic_ptr[i])+nic_size[i];
		like_ptr[i]=((char*)conten_ptr[i])+content_size[i];
		dislike_ptr[i]=((char*)like_ptr[i])+like_cnt_size[i];
		////////////////memory copy
		sprintf(big_buffer,"%u\r\n",(in.opi+i)->sns_id);
		memcpy(num_ptr[i],big_buffer,num_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->nick);
		memcpy(nic_ptr[i],big_buffer,nic_size[i]);

		sprintf(big_buffer,"%s\r\n",(in.opi+i)->comment);
		memcpy(conten_ptr[i],big_buffer,content_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->like_cnt);
		memcpy(like_ptr[i],big_buffer,like_cnt_size[i]);

		sprintf(big_buffer,"%u\r\n",(in.opi+i)->dislike_cnt);
		memcpy(dislike_ptr[i],big_buffer,dislike_cnt_size[i]);
	}

	if(content_cnt != 0)
	{
		local_end_spliter = (u_char*)((u_char*)dislike_ptr[content_cnt-1]+dislike_cnt_size[content_cnt-1]);
	}else{
		local_end_spliter = ((u_char*)avg_score_ptr)+4;
	}
	memcpy(local_end_spliter,spliter_end.c_str(),4);

	result = true;
	return result;
}