#include"stdafx.h"
#include "Member_manager.h"

Member_manager* Member_manager::singleton=NULL;

//�̱���
Member_manager* Member_manager::GetMember_manager()
{
	if(Member_manager::singleton == NULL)
	{
		singleton = new Member_manager();
	}
	return singleton;
}

//��� �Ŵ��� Ŭ���� ������
Member_manager::Member_manager(void)
{
	//DB_manager ����(DB����)
	dbm = DB_manager::GetDB_manager();
	startChecking();
}

//��� �Ŵ��� �Ҹ���
Member_manager::~Member_manager(void)
{
	stopChecking();
}

//�α��� �Լ�
bool Member_manager::Login(IN_Login &in_login, OUT_Login &out_login)
{
	//ID�� PASS�� �Բ� �����Ͽ� �ؽ� ����ü ���� �迭
	char to_hash[64];

	//DB���� �޾ƿ� ����ü
	IN_Login db_login;
	char nick[40];
	//������ǿ� ������ ����ü
	MemberSession mem;
	//�α��� ���� ����
	if(dbm->Query_login(in_login, db_login,nick))
	{
		//������ ���������� ���� �Ǿ��� ���
		//�Է� ���� ID�� DB���ִ� ���̵� ��(DB������ ��ҹ��� �񱳰� �ȵǹǷ�)
		if(strcmp(in_login.ID,db_login.ID) == 0)
		{
			strcpy_s(out_login.nick,strlen(nick)+1,nick);

			cout<<db_login.ID<<" "<<db_login.pass<<" "<<nick<<endl;
			cout<<db_login.ID<<" �� �α��� ����"<<endl;

			sprintf_s(to_hash,"%s%s",db_login.ID,db_login.pass);
			sha512((unsigned char*)to_hash,strlen(to_hash),out_login.cookie,0);
			memcpy_s(mem.cookie,64,out_login.cookie,64);
			memcpy_s(mem.ID,strlen(mem.ID)+1,db_login.ID,strlen(db_login.ID)+1);
			time(&mem.valid_stamp);				//��Ű ��ȿ�ð� �߱�
			
			hash.push_back(mem);

			out_login.result = 1;
			return true;
		}
		else
		{
			cout<<"���̵� �Ǵ� ��й�ȣ�� �ٸ��ϴ�."<<endl;

			out_login.result = 0;
			return false;
		}
	}
	else
	{
		//������ ������� �ʾ��� ���
		cout<<"DB���� ������ �����߽��ϴ�."<<endl;

		out_login.result = -1;
		return false;
	}
}

bool Member_manager::Logout(IN_Logout& in_logout, OUT_Logout &out_logout)
{
	for(int i = 0; i<(int)hash.size(); i++)
	{
		if(memcmp(in_logout.cookie, hash[i].cookie,64) == 0)
		{
			hash.erase(hash.begin() + i);

			out_logout.result = 1;
			return true;
		}
	}
	out_logout.result = 5;
	return false;
}

bool Member_manager::Signup(IN_Signup& in_signup, OUT_Signup &out_signup)
{
	if(dbm->Query_signup(in_signup))
	{
		cout<<in_signup.ID<<" "<<in_signup.pass<<" "<<in_signup.nick<<endl;
		cout<<"���ԿϷ�"<<endl;

		out_signup.result = 1;
		return true;
	}
	else
	{
		cout<<"���Խ���"<<endl;

		out_signup.result = 0;
		return false;
	}
}

bool Member_manager::Leave(IN_Leave& in_leave, OUT_Leave &out_leave)
{
	for(int i = 0; i<(int)hash.size(); i++)
	{
		if(memcmp(in_leave.cookie, hash[i].cookie,64) == 0)
		{
			if(dbm->Query_leave(hash[i].ID))
			{
				cout<<hash[i].ID<<" ���� ȸ�� Ż�� �Ϸ�Ǿ����ϴ�."<<endl;
				hash.erase(hash.begin() + i);

				out_leave.result = 1;
				return true;
			}
		}
	}
	cout<<"ȸ��Ż�𿡼� ������ �߻��Ͽ����ϴ�."<<endl;

	out_leave.result = 5;
	return false;
}

bool Member_manager::cookiechk(char* ID, u_char* in_cookie)
{
	for(int i = 0; i<(int)hash.size(); i++)
	{
		if(memcmp(in_cookie, hash[i].cookie,64) == 0)
		{
			strcpy_s(ID,21,hash[i].ID);
			time(&hash[i].valid_stamp);	//��Ű ��ȿ�Ⱓ ����
			return true;
		}
	}
	return false;
}
bool Member_manager::startChecking()
{
	bool result;
	HANDLE hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CheckWorkerWrap,this,0,NULL);
	if(hThread!=NULL)
	{
		CloseHandle(hThread);
	}else{
		printf_s("%s\n","Cookie startCookieChecking is failed");
		
		goto END;

	}
	bState = true;	//Thread Switch
	result =true;
	END:
	return result;
}
bool Member_manager::stopChecking()
{
	bool result;
	bState = false;	//Thread Switch

	result =TRUE;
	END:
	return result;
}
DWORD WINAPI Member_manager::CheckWorkerWrap(void * object)
{
	return ((Member_manager*)object)->CheckWorker();
}

bool Member_manager::CheckWorker()
{
	time_t current_t=0;
	while(bState)
	{
		time(&current_t);
		for (int i =0 ; i< hash.size() ; i++){
			if(current_t-hash[i].valid_stamp > TIME_OUT_COOKIE_SECOND)
			{
				printf_s("%s Cookie timeout// Erase Cookie\n",hash[i].ID);
				hash.erase(hash.begin()+i);
				i--;
			}
		}

		Sleep(TIME_OUT);
	}
	return true;
}