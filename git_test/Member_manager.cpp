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
	dbm = new DB_manager();
}

//��� �Ŵ��� �Ҹ���
Member_manager::~Member_manager(void)
{
	//DB_manager ����(DB���� ����)
	dbm->~DB_manager();
}

//�α��� �Լ�
bool Member_manager::Login(IN_Login in_login, OUT_Login &out_login)
{
	//DB���� �޾ƿ� ����ü
	IN_Login db_login;
	//������ǿ� ������ ����ü
	MemberSession mem;
	//�α��� ���� ����
	if(dbm->Query_login(in_login, db_login))
	{
		//������ ���������� ���� �Ǿ��� ���
		//�Է� ���� ID�� DB���ִ� ���̵� ��(DB������ ��ҹ��� �񱳰� �ȵǹǷ�)
		if(strcmp(in_login.ID,db_login.ID) == 0)
		{
			cout<<db_login.ID<<" "<<db_login.pass<<" "<<endl;
			cout<<db_login.ID<<" �� �α��� ����"<<endl;

			sha512((unsigned char*)db_login.ID,strlen(db_login.ID),out_login.cookie,0);
			memcpy_s(mem.cookie,64,out_login.cookie,64);
			strcpy_s(mem.ID,db_login.ID);
			
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

bool Member_manager::Logout(IN_Logout in_logout, OUT_Logout &out_logout)
{
	for(int i = 0; i<(int)hash.size(); i++)
	{
		if(hash[i].cookie == in_logout.cookie)
		{
			hash.erase(hash.begin() + i);

			out_logout.result = 1;
			return true;
		}
	}
	
	out_logout.result = 0;
	return false;
}

bool Member_manager::Signup(IN_Signup in_signup, OUT_Signup &out_signup)
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

bool Member_manager::Leave(IN_Leave in_leave, OUT_Leave &out_leave)
{
	for(int i = 0; i<(int)hash.size(); i++)
	{
		if(hash[i].cookie == in_leave.cookie)
		{
			if(dbm->Query_leave(hash[i].ID))
			{
				cout<<hash[i].ID<<" ���� ȸ�� Ż�� �Ϸ�Ǿ����ϴ�."<<endl;
				hash.erase(hash.begin() + i);

				out_leave.result = 1;
				return true;
			}
			else
			{
				cout<<"������ �߻��Ͽ����ϴ�."<<endl;

				out_leave.result = 0;
				return false;
			}
		}
	}

	out_leave.result = -1;
	return false;
}
