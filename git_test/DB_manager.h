#pragma once

#include"stdafx.h"
#include"sha512.h"


class DB_manager
{
private:
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret;

	TCHAR sql[256];

	unsigned char shacode[64];

public:
	DB_manager(void);
	~DB_manager(void);
	
	void Sql_run(TCHAR* sql);

	bool Query_signup(IN_Signup in_signup, OUT_Signup &out_signup);
	bool Query_login(IN_Login in_login, IN_Login &db_login);
	bool Query_leave();
	void Query_images();
	void Query_image_register();
	void Query_opi_search();
	void Query_opi_register();
	void Query_opi_delete();
};