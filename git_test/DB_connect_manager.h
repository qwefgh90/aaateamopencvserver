#pragma once

#include "StdAfx.h"

using namespace std;

class DB_connect_manager
{
private:
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret;

public:
	DB_connect_manager(void);
	virtual ~DB_connect_manager(void);
	void Connect_check();
};
