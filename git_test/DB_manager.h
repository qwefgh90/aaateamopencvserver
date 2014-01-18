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

	bool Query_signup(IN_Signup in_signup);
	bool Query_login(IN_Login in_login, IN_Login &db_login);
	bool Query_leave(char* ID);
	bool Query_images(IN_Search in_search, vector<Imagelist*> &Imagevector);
	bool Query_image_register(IN_Report in_report, OUT_Report out_report);
	bool Query_opi_search(IN_More in_more, OUT_More out_more);
	void Query_opi_register();
	void Query_opi_delete();
};