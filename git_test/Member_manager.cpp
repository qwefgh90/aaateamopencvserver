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
	dbm = DB_manager::GetDB_manager();
	startChecking();
}

//멤버 매니저 소멸자
Member_manager::~Member_manager(void)
{
	stopChecking();
}

//로그인 함수
bool Member_manager::Login(IN_Login &in_login, OUT_Login &out_login)
{
	//ID와 PASS를 함께 저장하여 해쉬 구조체 만들 배열
	char to_hash[64];

	//DB에서 받아올 구조체
	IN_Login db_login;
	char nick[40];
	//멤버세션에 저장할 구조체
	MemberSession mem;
	//로그인 쿼리 실행
	if(dbm->Query_login(in_login, db_login,nick))
	{
		//쿼리가 정상적으로 실행 되었을 경우
		//입력 받은 ID와 DB에있는 아이디 비교(DB에서는 대소문자 비교가 안되므로)
		if(strcmp(in_login.ID,db_login.ID) == 0)
		{
			strcpy_s(out_login.nick,strlen(nick)+1,nick);

			cout<<db_login.ID<<" "<<db_login.pass<<" "<<nick<<endl;
			cout<<db_login.ID<<" 님 로그인 성공"<<endl;

			sprintf_s(to_hash,"%s%s",db_login.ID,db_login.pass);
			sha512((unsigned char*)to_hash,strlen(to_hash),out_login.cookie,0);
			memcpy_s(mem.cookie,64,out_login.cookie,64);
			memcpy_s(mem.ID,strlen(mem.ID)+1,db_login.ID,strlen(db_login.ID)+1);
			time(&mem.valid_stamp);				//쿠키 유효시간 발급
			
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

bool Member_manager::Leave(IN_Leave& in_leave, OUT_Leave &out_leave)
{
	for(int i = 0; i<(int)hash.size(); i++)
	{
		if(memcmp(in_leave.cookie, hash[i].cookie,64) == 0)
		{
			if(dbm->Query_leave(hash[i].ID))
			{
				cout<<hash[i].ID<<" 님의 회원 탈퇴가 완료되었습니다."<<endl;
				hash.erase(hash.begin() + i);

				out_leave.result = 1;
				return true;
			}
		}
	}
	cout<<"회원탈퇴에서 오류가 발생하였습니다."<<endl;

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
			time(&hash[i].valid_stamp);	//쿠키 유효기간 갱신
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