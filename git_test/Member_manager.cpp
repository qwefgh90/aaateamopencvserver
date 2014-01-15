#include "stdafx.h"
#include "Member_manager.h"


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
	//�α��� ���� ����
	if(dbm->Query_login(in_login, db_login))
	{
		//������ ���������� ���� �Ǿ��� ���
		//�Է� ���� ID�� DB���ִ� ���̵� ��(DB������ ��ҹ��� �񱳰� �ȵǹǷ�)
		if(strcmp(in_login.ID,db_login.ID) == 0)
		{
			cout<<db_login.ID<<" "<<db_login.pass<<" "<<endl;
			cout<<db_login.ID<<" �� �α��� ����"<<endl;

			sha512((unsigned char*)db_login.ID,strlen(db_login.ID),out_login.coockie,0);
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

void Member_manager::Logout()
{

}

bool Member_manager::Signup(IN_Signup in_signup, OUT_Signup &out_signup)
{
	if(dbm->Query_signup(in_signup, out_signup))
	{
		cout<<in_signup.ID<<" "<<in_signup.pass<<" "<<in_signup.nick<<endl;
		cout<<"���ԿϷ�"<<endl;

		return true;
	}
	else
	{
		cout<<"���Խ���"<<endl;

		return false;
	}
}

void Member_manager::Leave()
{
}

void Member_manager::ParseToMember()
{
}
