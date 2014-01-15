#include"stdafx.h"
#include "Member_manager.h"

Member_manager* Member_manager::singleton=NULL;

//싱글톤
Member_manager* Member_manager::GetMember_manager()
{
	if(Member_manager::singleton == NULL)
	{
		singleton = new Member_manager();
	}
	return singleton;
}

//멤버 매니저 클래스 생성자
Member_manager::Member_manager(void)
{
	//DB_manager 생성(DB연결)
	dbm = new DB_manager();
}

//멤버 매니저 소멸자
Member_manager::~Member_manager(void)
{
	//DB_manager 제거(DB연결 종료)
	dbm->~DB_manager();
}

//로그인 함수
bool Member_manager::Login(IN_Login in_login, OUT_Login &out_login)
{
	//DB에서 받아올 구조체
	IN_Login db_login;
	//멤버세션에 저장할 구조체
	MemberSession mem;
	//로그인 쿼리 실행
	if(dbm->Query_login(in_login, db_login))
	{
		//쿼리가 정상적으로 실행 되었을 경우
		//입력 받은 ID와 DB에있는 아이디 비교(DB에서는 대소문자 비교가 안되므로)
		if(strcmp(in_login.ID,db_login.ID) == 0)
		{
			cout<<db_login.ID<<" "<<db_login.pass<<" "<<endl;
			cout<<db_login.ID<<" 님 로그인 성공"<<endl;

			sha512((unsigned char*)db_login.ID,strlen(db_login.ID),out_login.cookie,0);
			strcpy_s((char *)mem.cookie,strlen((char*)out_login.cookie),(char *)out_login.cookie);
			strcpy_s(mem.ID,db_login.ID);
			
			hash.push_back(mem);
			
			out_login.result = 1;
			return true;
		}
		else
		{
			cout<<"아이디 또는 비밀번호가 다릅니다."<<endl;

			out_login.result = 0;
			return false;
		}
	}
	else
	{
		//쿼리가 실행되지 않았을 경우
		cout<<"DB와의 연결이 실패했습니다."<<endl;

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
		cout<<"가입완료"<<endl;

		out_signup.result = 1;
		return true;
	}
	else
	{
		cout<<"가입실패"<<endl;

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
				cout<<hash[i].ID<<" 님의 회원 탈퇴가 완료되었습니다."<<endl;
				hash.erase(hash.begin() + i);

				out_leave.result = 1;
				return true;
			}
			else
			{
				cout<<"오류가 발생하였습니다."<<endl;

				out_leave.result = 0;
				return false;
			}
		}
	}

	out_leave.result = -1;
	return false;
}
