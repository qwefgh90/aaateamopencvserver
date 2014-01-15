#pragma once

#include"stdafx.h"
#include "DB_manager.h"
#include "sha512.h"


class Member_manager
{
private:
	static Member_manager* singleton;	

	DB_manager* dbm;
	vector<MemberSession>hash;

public:
	Member_manager(void);
	~Member_manager(void);

	static Member_manager* GetMember_manager();

	bool Login(IN_Login in_login, OUT_Login &out_login);
	bool Logout(IN_Logout in_logout, OUT_Logout &out_logout);
	bool Signup(IN_Signup in_signup, OUT_Signup &out_signup);
	void Leave();
	void ParseToMember();

};

