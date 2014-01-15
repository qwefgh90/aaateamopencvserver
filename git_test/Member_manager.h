#pragma once

#include "DB_manager.h"
#include "sha512.h"


class Member_manager
{
private:
	DB_manager* dbm;

public:
	Member_manager(void);
	~Member_manager(void);

	bool Login(IN_Login in_login, OUT_Login &out_login);
	void Logout();
	bool Signup(IN_Signup in_signup, OUT_Signup &out_signup);
	void Leave();
	void ParseToMember();

};

