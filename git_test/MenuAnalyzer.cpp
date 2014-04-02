#include "StdAfx.h"
#include "MenuAnalyzer.h"

MenuAnalyzer* MenuAnalyzer::singleton=NULL;

MenuAnalyzer::MenuAnalyzer(void){
	//initialize member manager
	member_manager = Member_manager::GetMember_manager();
	store_manager = Store_manager::GetStore_manager();
}
MenuAnalyzer::~MenuAnalyzer(void){
}
//Parse menu to structures and call a function of assembling packet
//paramf
//@data received from mobile 
//return
//@buffer and size to send to mobile
bool MenuAnalyzer::MenuSelector(Memory& in_memory,Memory& out_memory)
{
	bool dwResult=FALSE;
	u_char* in_buf=in_memory.buf;			//buffer from mobile(complete buffer available releasing)
	u_int in_len=in_memory.len;				//buffer length from mobile

	u_char proto_type;		//proto_type
	u_char err_code=1;		//erorr_code
	proto_type = *((u_char*)(in_buf+4));
	printf_s("protocol type : %d\n",proto_type);
	switch(proto_type)
	{
	case LOGIN:
		{
			IN_Login in;	//�α��� ��û ����ü
			OUT_Login out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			//1)��Ŷ���� ����ü ����

			if(this->packetToLogin(in,in_memory))
			{
				printf_s("[LOGIN]id: %s\n",in.ID);
				printf_s("[LOGIN]passwd: %s\n",in.pass);
				//2)������ ����ü�� �� ��⿡ ����
				if(member_manager->Login(in,out))
				{
					//dumpbyte(out.cookie,64);
					printf("[LOGIN_RESULT]nick: %s\n",out.nick);
					printf("[LOGIN_RESULT]result: %d\n",out.result);

					//3)������ ����ü���� ��Ŷ�� ����
					if(this->packetFromLogin(out_memory,out))
					{

					}
					else{
						err_code = out.result=3;
						goto ERRORCODE;
					}
					//4)����Ͽ��� ��Ŷ ����
				}else
				{
				printf_s("%s\n","[LOGIN]Login Fail ");
					err_code = out.result;
					goto ERRORCODE;
				}
			}else
			{
				printf_s("%s\n","[LOGIN]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}

			break;
		}
	case SIGNUP:
		{
			IN_Signup in;	//�α��� ��û ����ü
			OUT_Signup out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToSignup(in,in_memory))
			{
				printf("[SIGNUP]id: %s\n",in.ID);
				printf("[SIGNUP]passwd: %s\n",in.pass);
				printf("[SIGNUP]nick: %s\n",in.nick);

				//2)������ ����ü�� �� ��⿡ ����
				if(member_manager->Signup(in,out))
				{
					printf("[SIGNUP]result: %d\n",out.result);
					//3)������ ����ü���� ��Ŷ�� ����
					if(this->packetFromSignup(out_memory,out))
					{

					}else{
						err_code = out.result=3;
						goto ERRORCODE;
					}
					//4)����Ͽ��� ��Ŷ ����
				}else{
					printf_s("%s\n","[SIGNUP]Signup Fail");
					err_code = out.result;
					goto ERRORCODE;
				}
			}else{
				printf_s("%s\n","[SIGNUP]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}
			break;
		}
	case LOGOUT://�α׾ƿ�
		{
			IN_Logout in;	//�α��� ��û ����ü
			OUT_Logout out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToLogout(in,in_memory))
			{
				if(member_manager->Logout(in,out))
				{
					printf("[LOGOUT]result: %d\n",out.result);
					if(this->packetFromLogout(out_memory,out))
					{

					}else{
						err_code = out.result=3;
						goto ERRORCODE;
					}
				}else{
					err_code = out.result=5;
					goto ERRORCODE;
				}
			}else{
				printf_s("%s\n","[LOGOUT]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}
			break;
		}
	case LEAVE://ȸ��Ż��
		{
			IN_Leave in;	//�α��� ��û ����ü
			OUT_Leave out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToLeave(in,in_memory))
			{
				//1)��Ű�˻�� Leave �Լ����� �Ѵ�.
				//if(member_manager->cookiechk(in.ID,in.cookie))
				//{
					if(member_manager->Leave(in,out))
					{
						if(this->packetFromLeave(out_memory,out))
						{

						}else{
							err_code = out.result=3;
							goto ERRORCODE;
						}
					}else{
						printf_s("%s\n","[LEAVE]Leave Fail");
						err_code = out.result;
						goto ERRORCODE;
					}
				//}else{
				//	err_code = out.result=5;
				//	goto ERRORCODE;
				//}
			}else{
				printf_s("%s\n","[LEAVE]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}

			break;
		}
	case SEARCH://���� �˻�
		{
			IN_Search in;	//�α��� ��û ����ü
			OUT_Search out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));

			if(packetToSearch(in,in_memory))
			{
				//1)��Ű�˻�
				if(member_manager->cookiechk(in.ID,in.cookie))
				{
					printf("��Ű����\n");
					if(store_manager->Store_Search(in,out))
					{
						printf("result: %d @@@\ncode : %u\nscore%f\nopi_cnt : %u\n",out.result,out.code,out.score,out.opi_cnt);
						for (int i = 0 ; i < out.opi_cnt ; i++)
						{
							printf("%u : %s,%s,%u,%u,%u\n",i,out.opi[i].comment,out.opi[i].nick,out.opi[i].dislike_cnt,out.opi[i].like_cnt,out.opi[i].sns_id);
						}
						printf("�˻�����\n");
						if(packetFromSearch(out_memory,out))
						{
						}else{
							err_code = out.result=3;
							goto ERRORCODE;
						}
					}else{
						printf_s("%s\n","[SEARCH]�˻� ����");
						err_code = out.result;
						goto ERRORCODE;
					}

				}else{
					printf_s("%s\n","[SEARCH]Not Authorized");
					err_code = out.result=5;
					goto ERRORCODE;
				}
			}else{
				printf_s("%s\n","[SEARCH]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}
			//2)�����˻�

			//3)��Ŷ����

			break;
		}
	case MORE://������
		{
			IN_More in;	//�α��� ��û ����ü
			OUT_More out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToMore(in,in_memory))
			{
				if(member_manager->cookiechk(in.ID,in.cookie))
				{
					if(store_manager->Store_more(in,out))
					{
						if(packetFromMore(out_memory,out))
						{

						}else{
							err_code = out.result=3;
							goto ERRORCODE;
						}
					}else{
						printf_s("%s\n","[MORE]Searching more data failed");
						err_code = out.result;
						goto ERRORCODE;
					}
				}else{
					printf_s("%s\n","[MORE]Not Authorized");
					err_code = out.result=5;
					goto ERRORCODE;
				}
			}else{
				printf_s("%s\n","[MORE]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}

			break;
		}
	case WRITE_COMMENT://�ǰ߳����
		{
			IN_Write_comment in;	//�α��� ��û ����ü
			OUT_Write_comment out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToWriteComment(in,in_memory))
			{
				if(member_manager->cookiechk(in.ID,in.cookie))
				{
					if(store_manager->Store_opi_write(in,out))
					{
						if(packetFromWriteComment(out_memory,out))
						{
							printf("result: %d @@@\nscore%f\nopi_cnt : %u\n",out.result,out.score,out.opi_cnt);
							for (int i = 0 ; i < out.opi_cnt ; i++)
							{
								printf("%u : %s,%s,%u,%u,%u\n",i,out.opi[i].comment,out.opi[i].nick,out.opi[i].dislike_cnt,out.opi[i].like_cnt,out.opi[i].sns_id);
							}	
						}else{
							err_code = out.result = 3;
							goto ERRORCODE;
						}
					}else
					{
						printf_s("%s\n","[WRITE_COMMENT]Write Fail");
						err_code = out.result;
						goto ERRORCODE;
					}
				}else
				{
					printf_s("%s\n","[WRITE_COMMENT]Not Authorized");
					err_code = out.result=5;
					goto ERRORCODE;
				}
			}else
			{
				printf_s("%s\n","[WRITE_COMMENT]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}

			// true==
			// store_manager()
			break;
		}
	case MODIFY_COMMENT://�ǰ߼���
		{
			IN_Modify_comment in;	//�α��� ��û ����ü
			OUT_Modify_comment out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToModifyComment(in,in_memory))
			{
				if(member_manager->cookiechk(in.ID,in.cookie))
				{
					if(store_manager->Store_opi_modify(in,out))
					{
						if(packetFromModifyComment(out_memory,out))
						{

						}else{
							err_code = out.result=3;
							goto ERRORCODE;
						}
					}else{
						printf_s("%s\n","[MODIFY_COMMENT]Modify Fail");
						err_code = out.result;
						goto ERRORCODE;
					}
				}else{
					printf_s("%s\n","[MODIFY_COMMENT]Not Authorized");
					err_code = out.result=5;
					goto ERRORCODE;
				}
			}else{
				printf_s("%s\n","[MODIFY_COMMENT]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}

			break;
		}
	case DELETE_COMMENT://�ǰ߻���
		{
			IN_Delete_comment in;	//�α��� ��û ����ü
			OUT_Delete_comment out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToDeleteComment(in,in_memory))
			{
				if(member_manager->cookiechk(in.ID,in.cookie))
				{
					if(store_manager->Store_opi_delete(in,out))
					{
						if(packetFromDeleteComment(out_memory,out))
						{

						}else{
							err_code = out.result=3;
							goto ERRORCODE;
						}
					}else{
						printf_s("%s\n","[DELETE_COMMENT]Delete Fail");
						err_code = out.result;
						goto ERRORCODE;
					}
				}else{
					printf_s("%s\n","[DELETE_COMMENT]Not Authorized");
					err_code = out.result=5;
					goto ERRORCODE;
				}
			}else{
				printf_s("%s\n","[DELETE_COMMENT]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}

			break;
		}
	case LIKE://���ƿ�
		{
			IN_Like in;	//�α��� ��û ����ü
			OUT_Like out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToLike(in,in_memory))
			{
				if(member_manager->cookiechk(in.ID,in.cookie))
				{
					if(store_manager->Store_like(in,out))
					{
						if(packetFromLike(out_memory,out))
						{

						}else{
							err_code = out.result=3;
							goto ERRORCODE;
						}
					}else{
						printf_s("%s\n","[LIKE]Like Fail");
						err_code = out.result;
						goto ERRORCODE;
					}
				}else{
					printf_s("%s\n","[LIKE]Not Authorized");
					err_code = out.result=5;
					goto ERRORCODE;
				}
			}else{
				printf_s("%s\n","[LIKE]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}

			break;
		}
	case REPORT://���� ����
		{
			IN_Report in;	//�α��� ��û ����ü
			OUT_Report out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			
			if(!packetToReport(in,in_memory))
			{
				printf_s("%s\n","[REPORT]Parse Fail");
				//�Ľ� ����
				err_code=out.code=3;	//�����ڵ� ����
				goto ERRORCODE;
			}
			//1)��Űüũ/��������
			if(member_manager->cookiechk(in.ID,in.cookie))
			{
				//����
			}else
			{
				printf_s("%s\n","[REPORT]Not Authorized");
				//���� ����
				err_code=out.code=5;	//�����ڵ� ����
				goto ERRORCODE;
			}

			//2)�������
			if(store_manager->Store_report(in,out))
			{
				//����
				printf("���� ��� ����\n");
			}else
			{
				//����
				printf("���� ��� ����\n");
				err_code=out.code;	//�����ڵ� ����
				//goto ERRORCODE;	//�����ڵ尡 �ƴ� ������ ���� ��������

			}

			//3)��Ŷ����
			packetFromReport(out_memory,out);

			printf("result: %d @@@\ncode : %u\nscore%f\nopi_cnt : %u\n",out.result,out.code,out.score,out.opi_cnt);
			for (int i = 0 ; i < out.opi_cnt ; i++)
			{
				printf("%u : %s,%s,%u,%u,%u\n",i,out.opi[i].comment,out.opi[i].nick,out.opi[i].dislike_cnt,out.opi[i].like_cnt,out.opi[i].sns_id);
			}

			break;
		}
	case CACHE:
		{
			IN_Cache in;	//�α��� ��û ����ü
			OUT_Cache out;	//�α��� ���� ����ü
			memset(&in,0,sizeof(in));
			memset(&out,0,sizeof(out));
			if(packetToCache(in,in_memory))
			{
				if(member_manager->cookiechk(in.ID,in.cookie))
				{
					if(store_manager->Create_cache(in))
					{
						if(packetFromCache(out_memory,out))
						{

						}else{
							err_code = out.result = 3;
							goto ERRORCODE;
						}
					}else{
						printf_s("%s\n","[CACHE]Cache Fail");
						err_code = out.result;
						goto ERRORCODE;
					}
				}else{
					printf_s("%s\n","[CACHE]Not Authorized");
					err_code = out.result=5;
					goto ERRORCODE;
				}
			}else{
				printf_s("%s\n","[CACHE]Parse Fail");
				err_code = out.result=3;
				goto ERRORCODE;
			}
			break;
		}
	}
	//���� ��Ŷ
	dwResult= true;
	return dwResult;

ERRORCODE:
	//���� ó�� ��Ŷ ����
	printf_s("%s","------SEND ERROR PACKET-----\n");
	packetFromError(out_memory,err_code);
	dwResult= true;

	return dwResult;
}

MenuAnalyzer* MenuAnalyzer::GetMenuAnalyzer()
{
	if(MenuAnalyzer::singleton==NULL)
	{
		singleton = new MenuAnalyzer();
	}
	return singleton;
}
/*
vector<string> MenuAnalyzer::split_(string str, string sep){
char* cstr = const_cast<char*>(str.c_str());
char* current;
vector<std::string> arr;
current=strtok(cstr,sep.c_str());
while(current != NULL){
arr.push_back(current);
current=strtok(NULL, sep.c_str());
}
return arr;
}*/

bool MenuAnalyzer::split(__in vector<string>& arr,string str, string sep){
	char* cstr = const_cast<char*>(str.c_str());
	char* current = cstr;
	char* next=  NULL;
	bool result = false;
	next=strstr(cstr,sep.c_str());
	if(next!=NULL)
	{
		int i = 0;
		for (i=0 ; i<sep.length();i++)
		{
			next[i]=NULL;
		}
		next= next+sep.length();

	}
	string temp(cstr);
	arr.push_back(temp);

	while(next != NULL){
		current = next; 
		next=strstr(next,sep.c_str());
		if(next!=NULL)
		{
			int i = 0;
			for (i=0 ; i<sep.length();i++)
			{
				next[i]=NULL;
			}
			next= next+sep.length();
		}
		string temp(current);
		arr.push_back(temp); 
	}
	result=true;
	return result;
}