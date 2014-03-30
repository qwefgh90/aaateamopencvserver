#pragma once

#include "stdafx.h"
#include "DB_manager.h"
#include "sha512.h"
#include <time.h>


class Member_manager
{
private:
	static Member_manager* singleton;	

	DB_manager* dbm;
	vector<MemberSession>hash;
	//��ŰüĿ
	bool bState;
public:
	//��ŰüĿ
#define TIME_OUT 10000				//��Ű �˻� ����
#define TIME_OUT_COOKIE_SECOND 3600	//��Ű ��� �ð�
	bool startChecking();
	bool stopChecking();
	static DWORD WINAPI CheckWorkerWrap(void*);
	bool CheckWorker();

	//���
	Member_manager(void);
	~Member_manager(void);
	

	static Member_manager* GetMember_manager();

	bool Login(IN_Login& in_login, OUT_Login &out_login);
	bool Logout(IN_Logout& in_logout, OUT_Logout &out_logout);
	bool Signup(IN_Signup& in_signup, OUT_Signup &out_signup);
	bool Leave(IN_Leave& in_leave, OUT_Leave &out_leave);
	//char* Check_Member(unsigned char* cookie);
	bool cookiechk(char* ID, u_char* in_cookie);

};
