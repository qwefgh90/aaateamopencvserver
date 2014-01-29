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

bool DB_manager::Query_image_register(IN_Report in_report, OUT_Report &out_report)
{
	int filter_id;
	int store_code;

	for ( int i = 0; i < filter_no ; i++)
		if( in_report.filter & filter[i])
		{
			filter_id = i;
		}
		sprintf_s(sql, "insert into STORE(store_key,gps_longitude,gps_latitude,store_filter) values (%s,%f,%f,%d)", in_report.image, in_report.longitude, in_report.latitude,in_report.filter);

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

			sprintf_s(sql, "insert into SNS(nick, store_code, sns_con) values (%s,%d,%s)", in_report.ID, store_code, in_report.comment);

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

bool DB_manager::Query_opi_search(IN_More in_more, OUT_More &out_more)
{
	int i=0;
	sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%c' desc limit 5 offset '%c'", in_more.code, in_more.sort, in_more.comment_count);

	Sql_run(sql);

	if(ret==SQL_SUCCESS)
	{
		while(SQLFetch(hStmt))
		{
			SQLGetData(hStmt, 1, SQL_INTEGER, &out_more.opi[i].sns_id, 4, NULL);
			SQLGetData(hStmt, 2, SQL_C_CHAR, out_more.opi[i].nick, 30, NULL);
			SQLGetData(hStmt, 3, SQL_C_CHAR, out_more.opi[i].comment, 400, NULL);
			SQLGetData(hStmt, 2, SQL_INTEGER, &out_more.opi[i].like_cnt, 4, NULL);
			SQLGetData(hStmt, 2, SQL_INTEGER, &out_more.opi[i].dislike_cnt, 4, NULL);
			i++;
		}
		
		out_more.opi_cnt = i+1;
	}
	else
	{
		return false;
	}
	
	sprintf_s(sql, "select AVG(b.score) from SNS a, score b where a.sns_id=b.sns_id and a.store_code='%s'", in_more.code);
	
	Sql_run(sql);

	if(ret==SQL_SUCCESS)
	{
		SQLGetData(hStmt, 1, SQL_INTEGER, &out_more.score, 4, NULL);

		return true;
	}

	return false;
}

bool DB_manager::Query_opi_register(IN_Write_comment in_write_opi, OUT_Write_comment &out_write_opi)
{
	int sns_id = 0;

	sprintf_s(sql, "insert into SNS(nick, store_code, sns_con,score) values (%s,%d,%s)", in_write_opi.ID, in_write_opi.code, in_write_opi.comment,in_write_opi.comment_score);
	
	Sql_run(sql);
	
	sprintf_s(sql, "select top 1 sns_id from SNS order by desc");

	Sql_run(sql);
	
	if(SQLFetch(hStmt))
	{
		SQLGetData(hStmt, 1, SQL_INTEGER, &sns_id, 4, NULL);
	}

	sprintf_s(sql, "insert into member_sns(mem_ID,sns_id) values (%s,%d)", in_write_opi.ID, sns_id);
	
	Sql_run(sql);

	if(ret==SQL_SUCCESS)
	{
		int i=0;
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%c' desc limit 5 offset '%c'", in_write_opi.code, in_write_opi.sort, in_write_opi.comment_count);

		Sql_run(sql);

		if(ret==SQL_SUCCESS)
		{
			while(SQLFetch(hStmt))
			{
				SQLGetData(hStmt, 1, SQL_INTEGER, &out_write_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(hStmt, 2, SQL_C_CHAR, out_write_opi.opi[i].nick, 30, NULL);
				SQLGetData(hStmt, 3, SQL_C_CHAR, out_write_opi.opi[i].comment, 400, NULL);
				SQLGetData(hStmt, 2, SQL_INTEGER, &out_write_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(hStmt, 2, SQL_INTEGER, &out_write_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}

			out_write_opi.opi_cnt = i+1;
		}
		else
		{
			return false;
		}

		sprintf_s(sql, "select AVG(b.score) from SNS a, score b where a.sns_id=b.sns_id and a.store_code='%s'", in_write_opi.code);

		Sql_run(sql);

		if(ret==SQL_SUCCESS)
		{
			SQLGetData(hStmt, 1, SQL_INTEGER, &out_write_opi.score, 4, NULL);

			return true;
		}

		return false;
	}
	//상점 코드, 의견, 평점, ID를 DB에 등록한다.
	//의견들, 의견 개수, 평균평점을 리턴한다.

	return false;
}

bool DB_manager::Query_opi_modify(IN_Modify_comment in_mod_opi, OUT_Modify_comment &out_mod_opi)
{
	sprintf_s(sql, "update SNS set sns_con='%s', score='%d' where sns_id='%d'", in_mod_opi.comment, in_mod_opi.comment_score, in_mod_opi.comment_num);

	Sql_run(sql);
	
	if(ret==SQL_SUCCESS)
	{
		int i=0;
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%c' desc limit 5 offset '%c'", in_mod_opi.code, in_mod_opi.sort, in_mod_opi.comment_count);

		Sql_run(sql);

		if(ret==SQL_SUCCESS)
		{
			while(SQLFetch(hStmt))
			{
				SQLGetData(hStmt, 1, SQL_INTEGER, &out_mod_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(hStmt, 2, SQL_C_CHAR, out_mod_opi.opi[i].nick, 30, NULL);
				SQLGetData(hStmt, 3, SQL_C_CHAR, out_mod_opi.opi[i].comment, 400, NULL);
				SQLGetData(hStmt, 2, SQL_INTEGER, &out_mod_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(hStmt, 2, SQL_INTEGER, &out_mod_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}

			out_mod_opi.opi_cnt = i+1;
		}
		else
		{
			return false;
		}

		sprintf_s(sql, "select AVG(b.score) from SNS a, score b where a.sns_id=b.sns_id and a.store_code='%s'", in_mod_opi.code);

		Sql_run(sql);

		if(ret==SQL_SUCCESS)
		{
			SQLGetData(hStmt, 1, SQL_INTEGER, &out_mod_opi.score, 4, NULL);

			return true;
		}

		return false;
	}

	//글번호를 이용해 글내용, 평점을 수정
	//의견들, 의견개수, 평균평점을 리턴
	return false;
}

bool DB_manager::Query_opi_delete(IN_Delete_comment in_del_opi, OUT_Delete_comment &out_del_opi)
{
	sprintf_s(sql, "delete from SNS where sns_id='%s'", in_del_opi.comment_num);

	Sql_run(sql);
	
	if(ret==SQL_SUCCESS)
	{
		int i=0;
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%c' desc limit 5 offset '%c'", in_del_opi.code, in_del_opi.sort, in_del_opi.comment_count);

		Sql_run(sql);

		if(ret==SQL_SUCCESS)
		{
			while(SQLFetch(hStmt))
			{
				SQLGetData(hStmt, 1, SQL_INTEGER, &out_del_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(hStmt, 2, SQL_C_CHAR, out_del_opi.opi[i].nick, 30, NULL);
				SQLGetData(hStmt, 3, SQL_C_CHAR, out_del_opi.opi[i].comment, 400, NULL);
				SQLGetData(hStmt, 2, SQL_INTEGER, &out_del_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(hStmt, 2, SQL_INTEGER, &out_del_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}

			out_del_opi.opi_cnt = i+1;
		}
		else
		{
			return false;
		}

		sprintf_s(sql, "select AVG(b.score) from SNS a, score b where a.sns_id=b.sns_id and a.store_code='%s'", in_del_opi.code);

		Sql_run(sql);

		if(ret==SQL_SUCCESS)
		{
			SQLGetData(hStmt, 1, SQL_INTEGER, &out_del_opi.score, 4, NULL);

			return true;
		}

		return false;
	}
	
	//글번호를 이용해 의견을 삭제
	//의견들, 의견개수, 평균평점을 리턴
	return false;
}

bool DB_manager::Query_opi_like(IN_Like in_like_opi, OUT_Like &out_like_opi)
{
	sprintf_s(sql, "update member_sns set val='%c' where mem_ID='%s' and sns_id='%s'", in_like_opi.like, in_like_opi.ID, in_like_opi.num);

	Sql_run(sql);

	if(ret == SQL_SUCCESS)
	{
		return true;
	}
	return false;
}

void DB_manager::Sql_run(char* sql)
{
	ret = SQLExecDirect(hStmt, (SQLCHAR*)sql, SQL_NTS);
}