#include "DB_connect_manager.h"


DB_connect_manager::DB_connect_manager(void)
{
	ret = SQL_SUCCESS;
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	ret = SQLConnect(hDbc,(SQLCHAR *)"AAA", SQL_NTS, (SQLCHAR *)"AAA", SQL_NTS, (SQLCHAR *)"ghkdlxld", SQL_NTS);
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
}


DB_connect_manager::~DB_connect_manager(void)
{
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt );
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void DB_connect_manager::Connect_check()
{
	int a;

	if((ret !=SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		cout<<"Connection:fail\n"<<ret;
		cin>>a;
	}
	else
	{
		cout<<"Connection:success\n";
		char name[64];
		char address[64];
		char id[64];
		SQLExecDirect(hStmt,(SQLCHAR*)"select * from member",SQL_NTS);
		while(SQLFetch(hStmt)==SQL_SUCCESS){
			SQLGetData(hStmt, 1, SQL_C_CHAR, id, 64, NULL);
			SQLGetData(hStmt, 2, SQL_C_CHAR, name, 64, NULL);
			SQLGetData(hStmt, 3, SQL_C_CHAR, address, 64, NULL);
			cout<<id<<" "<<name<<" "<<address<<endl;
		}
		cin>>a;
	}
}