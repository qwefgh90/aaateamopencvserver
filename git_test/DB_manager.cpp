#include"stdafx.h"
#include "DB_manager.h"

DB_manager::DB_manager(void)
{
	int a;
	ret = SQL_SUCCESS;
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	ret = SQLConnect(hDbc,(SQLCHAR *)"AAA", SQL_NTS, (SQLCHAR *)"AAA", SQL_NTS, (SQLCHAR *)"ghkdlxld", SQL_NTS);
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	if((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		cout<<"Connection:fail\n"<<ret;
		cin>>a;
	}
	else
	{
		cout<<"Connection:success\n";
	}
}


DB_manager::~DB_manager(void)
{
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt );
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

bool DB_manager::Query_signup(IN_Signup in_signup)
{
	sprintf_s(sql, "insert into member values (%s,%s,%s)", in_signup.ID, in_signup.pass, in_signup.nick);
	Sql_run(sql);
	if(ret==SQL_SUCCESS)
	{	
		return true;
	}
	else
	{
		return false;
	}
}

bool DB_manager::Query_login(IN_Login in_login, IN_Login &db_login)
{
	sprintf_s(sql, "select * from member where ID='%s' and pass='%s'", in_login.ID, in_login.pass);
	Sql_run(sql);
	if(ret==SQL_SUCCESS)
	{
		SQLFetch(hStmt);
		SQLGetData(hStmt, 1, SQL_C_CHAR, db_login.ID, 20, NULL);
		SQLGetData(hStmt, 2, SQL_C_CHAR, db_login.pass, 20, NULL);
		
		return true;
	}
	else
	{
		return false;
	}
}

bool DB_manager::Query_leave(char* ID)
{
	sprintf_s(sql, "delete  from member where ID = '%s'", ID);
	Sql_run(sql);
	if(ret==SQL_SUCCESS)
	{	
		return true;
	}
	else
	{
		return false;
	}
}

bool DB_manager::Query_images(IN_Search in_search, vector<Imagelist*> &Imagevector)
{
	char buf[50];
	char c[2];
	memset(buf,0,50);
	Imagelist* Image_list;

	Image_list = new Imagelist();

	for ( int i = 0; i < filter_no ; i++)
		if( in_search.filter & filter[i])
		{
			strcat_s(buf,"'");
			_itoa_s(i,c,10);
			strcat_s(buf, c);
			strcat_s(buf,"'");
			strcat_s(buf,",");
		}
		*(strrchr(buf,','))=NULL;
		sprintf_s(sql, "select store_code, store_key from STORE where store_filter in(%s) and gps_Longitude between '%s' and '%s' and gps_Latitude between '%s' and '%s'",
			buf,
			in_search.longitude - ERRORRANGE, in_search.longitude + ERRORRANGE,
			in_search.latitude - ERRORRANGE, in_search.latitude + ERRORRANGE);

		Sql_run(sql);

		if(ret==SQL_SUCCESS)
		{
			while(SQLFetch(hStmt))
			{
				SQLGetData(hStmt, 1, SQL_INTEGER, &Image_list->store_code, 4, NULL);
				SQLGetData(hStmt, 2, SQL_C_CHAR, &Image_list->store_path, 40, NULL);

				Imagevector.push_back(Image_list);
			}
			
			return true;
		}
		return false;
}

bool DB_manager::Query_image_register(IN_Report in_report, OUT_Report out_report)
{
	int filter_id;
	int store_code;

	for ( int i = 0; i < filter_no ; i++)
		if( in_report.filter & filter[i])
		{
			filter_id = i;
		}
		sprintf_s(sql, "insert into STORE values (%s,%f,%f,%d)", in_report.image, in_report.longitude, in_report.latitude,in_report.filter);

		Sql_run(sql);

		if(ret == SQL_SUCCESS)
		{
			sprintf_s(sql, "select store_code from STORE where store_key='%s'", in_report.image);

			Sql_run(sql);

			if(ret==SQL_SUCCESS)
			{
				SQLFetch(hStmt);
				SQLGetData(hStmt, 1, SQL_INTEGER, &store_code, 4, NULL);
			}

			sprintf_s(sql, "insert into SNS(ID, store_code, store_con) values (%s,%d,%s)", in_report.ID, store_code, in_report.comment);

			Sql_run(sql);
			
			if(ret==SQL_SUCCESS)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		return false;

}

bool Query_opi_search(IN_More in_more, OUT_More out_more)
{

}

void DB_manager::Query_opi_register()
{

}

void DB_manager::Query_opi_delete()
{

}

void DB_manager::Sql_run(char* sql)
{
	ret = SQLExecDirect(hStmt, (SQLCHAR*)sql, SQL_NTS);
}