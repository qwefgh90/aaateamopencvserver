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
//�����ͱ��� + Ÿ�� + ��Ű + ���� + ���� + �浵 + ���� ������ + ������ ������ +(�̹��� ũ�� ����)
bool MenuAnalyzer::packetToSearch(__out IN_Search& out, __in Memory& memory )
{    
	bool result = false;
	u_char* cookie = memory.buf+5 ;		//��Ű
	u_char* filter = cookie+64 ;			//����
	u_char* sort = (filter+1);
	float* latitude = (float*)(sort+1) ;		//����
	float* longitude = (float*)((u_char*)latitude+4) ;		//�浵
	u_char* image_buf = (u_char*)longitude+8 ;	//image
	u_int	image_size = *((u_int*)memory.buf)-SEARCH_SIZE_BUTIMAGE;	//image size // �̹��� ���� �����ڴ� ����(������)
	    
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
	u_char* opi_cnt = opi_cnt +1 ;
	
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
	u_int* num = (u_int*)(cookie+64) ;		//�� ��ȣ
	u_char* like = ((u_char*)num)+4 ;			//�� ����
	
	memcpy_s(out.cookie,64,cookie,64);				//��Ű
	memcpy_s(&(out.num),4,num,4);					//�۹�ȣ
	memcpy_s(&(out.like),1,like,1);					//���ƿ� , �Ⱦ�� ���а�
	
	result = true;
	return result;
}
const u_int SEARCH_SIZE_BUTIMAGE_OPINION = (4)+ (1) + (64)+(1)+(4)+(4) +(1) +4 +2;
//�����ͱ��� + Ÿ�� + ��Ű + ���� + ���� + �浵 + �ǰ����� + ������ ������ + (�ǰ�/�̹��� ����)
bool MenuAnalyzer::packetToReport(__out IN_Report& out, __in Memory& memory )
{
	bool result = false;
	string str;

	u_char* cookie = memory.buf+5 ;
	u_char* filter = cookie+64 ;
	float*	latitude = (float*)(filter+1 );
	float*	longitude= (float*)(((u_char*)latitude)+4 );
	printf("[REPORT]GPS Latitude : %f, Longitude : %f\n",*latitude,*longitude);

	u_char* opinion_score = ((u_char*)longitude)+4;
	u_char* opinion = opinion_score+1 +2;//������ \r\n ����
	
	//��ȿ�� üũ : �ǰ��ʵ�
	if(*(char*)opinion==0 ||( *(char*)opinion == '\r' && *(char*)(opinion+1) == '\n'))
	{
		printf("%s '%c'\n","Opinion error : input value : 0x%x",(int)(*(char*)opinion));
		goto END;
	}


	u_int opinion_size=0;
	u_int image_size=0;
	u_char* image=NULL;

	memcpy(out.cookie,cookie,64);
	memcpy(&(out.filter),filter,1);				//����
	memcpy(&(out.store.latitude),latitude,4);			//����
	memcpy(&(out.store.longitude),longitude,4);		//�浵

	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,spliter_end.c_str());	//\r\n������
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		end_ptr[2]=NULL;
		end_ptr[3]=NULL;

		opinion_size = ((u_char*)end_ptr-opinion);	//�ǰ� ����
		printf("Opinion length : %ld\n",opinion_size);
		image = ((u_char*)(end_ptr+4));				//�̹��� ������
		//�������� ���̴� little endian ���� ����Ǿ� �־����.
		image_size = *((u_int*)memory.buf) - (SEARCH_SIZE_BUTIMAGE_OPINION + (opinion_size));//�ǰ�+���� ����
		printf("Image length : %ld\n",image_size);
	}else
	{
		goto END;
	}

	strcpy_s(out.comment,strlen((char*)opinion)+1,(char*)opinion);		//�� ���� (���� �������ݿ��� �� ���븸 0�� �ε����� ������ �ȴ�)
	out.comment_score = *opinion_score;		//�� ����

	//�̹��� ����
	out.store.image.buf=new byte[image_size];		//���� ó�� �� �Ҵ����� �ʿ�
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
	//����
	out.latitude = *latitude;
	out.longitude = * longitude;

	result = true;
	END:
	return result;
}