#include"stdafx.h"
#include "DB_manager.h"

DB_manager* DB_manager::singleton=NULL;

//�̱���
DB_manager* DB_manager::GetDB_manager()
{
	if(DB_manager::singleton == NULL)
	{
		singleton = new DB_manager();
	}
	return singleton;
}

//DB�ڿ� �Ҵ�
DB_manager::DB_manager(void)
{
	int a;
	ret = SQL_SUCCESS;
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	ret = SQLConnect(hDbc,(SQLCHAR *)"AAA", SQL_NTS, (SQLCHAR *)"AAA", SQL_NTS, (SQLCHAR *)"ghkdlxld", SQL_NTS);
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	if(ret == SQL_SUCCESS)
	{
		//���ῡ ���� ���� ���
		cout<<"Connection:success\n";
	}
	else
	{
		//���ῡ �������� ���
		cout<<"Connection:fail\n"<<ret;
	}
}

//DB �ڿ�����
DB_manager::~DB_manager(void)
{
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt );
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

//ȸ������ ����
bool DB_manager::Query_signup(IN_Signup in_signup)
{
	//���̵�, ���, �г����� DB���̺� ����
	sprintf_s(sql, "insert into member values ('%s','%s','%s')", in_signup.ID, in_signup.pass, in_signup.nick);
	Sql_run(sql);

	if(ret==SQL_SUCCESS)
	{
		//Ŀ�� ����
		SQLCloseCursor(hStmt);
		return true;
	}
	else
	{
		return false;
	}
}

//�α��� ����
bool DB_manager::Query_login(IN_Login in_login, IN_Login &db_login,char* nick)
{
	//���̵�, ����� �̿��� TABLE���ִ� DATA SELECT
	sprintf_s(sql, "select * from member where ID='%s' and pass='%s'", in_login.ID, in_login.pass);
	Sql_run(sql);
	if(ret==SQL_SUCCESS)
	{
		//���̵�� ����� ��ġ�ϴ°� ���� ���
		//SELECT�� DATA�� ���̵�� ��й�ȣ, �г����� ��ȯ
		SQLFetch(hStmt);
		SQLGetData(hStmt, 1, SQL_C_CHAR, db_login.ID, 20, NULL);
		SQLGetData(hStmt, 2, SQL_C_CHAR, db_login.pass, 20, NULL);
		SQLGetData(hStmt, 3, SQL_C_CHAR, nick, 40, NULL);
		SQLCloseCursor(hStmt);
		return true;
	}

	return false;
}

//ȸ�� Ż�� ����
bool DB_manager::Query_leave(char* ID)
{
	//�̹� �α������̱� ������ ID�� �޾ƿͼ� DB TABLE�� �ִ� ID�� ��ġ�ϴ� DATA ���� ����
	sprintf_s(sql, "delete from member where ID='%s'", ID);
	Sql_run(sql);
	if(ret==SQL_SUCCESS)
	{	
		SQLCloseCursor(hStmt);
		return true;
	}
	else
	{
		return false;
	}
}

//�̹��� �˻� ����
bool DB_manager::Query_images(IN_Search in_search, vector<Imagelist> &Imagevector)
{
	//���͸� ������� ���ۻ���
	char buf[50];
	//������ ��ȣ�� �����ϱ� ���� ����
	char c[2];
	//���� �ʱ�ȭ
	memset(buf,0,50);
	//�̹��� ����Ʈ�� ����
	Imagelist Image_list;

	//���� ������ ����� ���� for��
	for ( int i = 0; i < filter_no ; i++)
		if( in_search.filter & filter[i])
		{
			strcat_s(buf,"'");
			_itoa_s(i,c,10);
			strcat_s(buf, c);
			strcat_s(buf,"'");
			strcat_s(buf,",");
		}
		//?
		*(strrchr(buf,','))=NULL;
		//���Ͱ��� �浵/������ �̿��� �����ڵ� �޾ƿ���
		sprintf_s(sql, "select store_code, store_key from STORE where store_filter in(%s) and gps_Longitude between '%s' and '%s' and gps_Latitude between '%s' and '%s'",
			buf,
			in_search.store.longitude - ERRORRANGE, in_search.store.longitude + ERRORRANGE,
			in_search.store.latitude - ERRORRANGE, in_search.store.latitude + ERRORRANGE);

		Sql_run(sql);

		if(ret==SQL_SUCCESS)
		{
			while(SQLFetch(hStmt))
			{
				//�̹��� ���Ϳ� �ֱ� ���� ����
				SQLGetData(hStmt, 1, SQL_INTEGER, &Image_list.store_code, 4, NULL);
				SQLGetData(hStmt, 2, SQL_C_CHAR, Image_list.store_path, 40, NULL);
				//�̹��� ��� ������ �̿��� ������ READ�ؼ� ���Ϳ� ����
				Imagevector.push_back(Image_list);
			}
			
			SQLCloseCursor(hStmt);
			return true;
		}
		
		return false;
}

//�̹��� ��� ����
bool DB_manager::Query_image_register(IN_Report in_report, OUT_Report &out_report)
{
	int filter_id;
	int store_code;
	//��Ʈ AND������ ���� ����� ������ ���Ͱ��� �˾Ƴ���
	for ( int i = 0; i < filter_no ; i++)
		if( in_report.filter & filter[i])
			filter_id = i;
		//���� ���
		sprintf_s(sql, "insert into STORE(store_key,gps_longitude,gps_latitude,store_filter) values (%s,%f,%f,%d)", in_report.store.image, in_report.store.longitude, in_report.store.latitude,in_report.filter);

		Sql_run(sql);

		if(ret == SQL_SUCCESS)
		{
			//�ڵ��� ���� �Ŀ� ���� Ű���� �̿��� ���� �ڵ尪�� �˾Ƴ���
			SQLCloseCursor(hStmt);
			sprintf_s(sql, "select store_code from STORE where store_key='%s'", in_report.store.image);

			Sql_run(sql);

			if(ret==SQL_SUCCESS)
			{
				SQLFetch(hStmt);
				SQLGetData(hStmt, 1, SQL_INTEGER, &store_code, 4, NULL);
				SQLCloseCursor(hStmt);
			}
			//�����ڵ带 �̿��� �ǰ��� INSERT
			sprintf_s(sql, "insert into SNS(nick, store_code, sns_con) values (%s,%d,%s)", in_report.ID, store_code, in_report.comment);

			Sql_run(sql);
			
			if(ret==SQL_SUCCESS)
			{
				SQLCloseCursor(hStmt);
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
		
		SQLCloseCursor(hStmt);
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

		SQLCloseCursor(hStmt);
		return true;
	}

	return false;
}

bool DB_manager::Query_opi_register(IN_Write_comment in_write_opi, OUT_Write_comment &out_write_opi)
{
	int sns_id = 0;

	sprintf_s(sql, "insert into SNS(nick, store_code, sns_con,score) values (%s,%d,%s)", in_write_opi.ID, in_write_opi.code, in_write_opi.comment,in_write_opi.comment_score);
	
	Sql_run(sql);
	
	SQLCloseCursor(hStmt);

	sprintf_s(sql, "select top 1 sns_id from SNS order by desc");

	Sql_run(sql);
	
	if(SQLFetch(hStmt))
	{
		SQLGetData(hStmt, 1, SQL_INTEGER, &sns_id, 4, NULL);
		
		SQLCloseCursor(hStmt);
	}

	sprintf_s(sql, "insert into member_sns(mem_ID,sns_id) values (%s,%d)", in_write_opi.ID, sns_id);
	
	Sql_run(sql);

	if(ret==SQL_SUCCESS)
	{
		SQLCloseCursor(hStmt);

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
			
			SQLCloseCursor(hStmt);
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
			
			SQLCloseCursor(hStmt);
			return true;
		}

		return false;
	}
	//���� �ڵ�, �ǰ�, ����, ID�� DB�� ����Ѵ�.
	//�ǰߵ�, �ǰ� ����, ��������� �����Ѵ�.

	return false;
}

bool DB_manager::Query_opi_modify(IN_Modify_comment in_mod_opi, OUT_Modify_comment &out_mod_opi)
{
	sprintf_s(sql, "update SNS set sns_con='%s', score='%d' where sns_id='%d'", in_mod_opi.comment, in_mod_opi.comment_score, in_mod_opi.comment_num);

	Sql_run(sql);
	
	if(ret==SQL_SUCCESS)
	{
		SQLCloseCursor(hStmt);
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
			
			SQLCloseCursor(hStmt);
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
			
			SQLCloseCursor(hStmt);
			return true;
		}

		return false;
	}

	//�۹�ȣ�� �̿��� �۳���, ������ ����
	//�ǰߵ�, �ǰ߰���, ��������� ����
	return false;
}

bool DB_manager::Query_opi_delete(IN_Delete_comment in_del_opi, OUT_Delete_comment &out_del_opi)
{
	sprintf_s(sql, "delete from SNS where sns_id='%s'", in_del_opi.comment_num);

	Sql_run(sql);
	
	if(ret==SQL_SUCCESS)
	{
		SQLCloseCursor(hStmt);
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
			
			SQLCloseCursor(hStmt);
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
			
			SQLCloseCursor(hStmt);
			return true;
		}

		return false;
	}
	
	//�۹�ȣ�� �̿��� �ǰ��� ����
	//�ǰߵ�, �ǰ߰���, ��������� ����
	return false;
}

bool DB_manager::Query_opi_like(IN_Like in_like_opi, OUT_Like &out_like_opi)
{
	sprintf_s(sql, "update member_sns set val='%c' where mem_ID='%s' and sns_id='%s'", in_like_opi.like, in_like_opi.ID, in_like_opi.num);

	Sql_run(sql);

	if(ret == SQL_SUCCESS)
	{
		SQLCloseCursor(hStmt);
		return true;
	}
	return false;
}

void DB_manager::Sql_run(char* sql)
{
	ret = SQLExecDirect(hStmt, (SQLCHAR*)sql, SQL_NTS);
	memset(sql,0,256);
}