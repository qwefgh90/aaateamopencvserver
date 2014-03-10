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
	sqlsvrpool = new SQLSvrPool(	"117.16.244.38",	//hostnam
		"AAA",	//database 
		"AAA",			//username
		"ghkdlxld",	//password
		300000,		//keepalive timeout (milliseconds)
		"SELECT top 1 name FROM dbo.sysobjects");	//keepalive statement

	/*Create a pool that will have 3 cached connections and will swell upto a 
	total of 5 connections. Returns the number of cached connections or -1 on error
	*/
	if(sqlsvrpool->CreatePool(3, 5)<=0){
		cout<<"Error creating database pool\n";
		cout<<sqlsvrpool->GetLastSystemError()<<endl;	//If it's asystem error
	}

	/*Dispaly the pool information*/
	cout<<(*sqlsvrpool);
}

//DB �ڿ�����
DB_manager::~DB_manager(void)
{
	/*Destroy the database pool*/
	if(sqlsvrpool->DestroyPool()>0){
		cout<<"There are still some un-released connections in the pool\n";
	}

	delete sqlsvrpool;
}

//ȸ������ ����
bool DB_manager::Query_signup(IN_Signup in_signup)
{
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

	//���̵�, ���, �г����� DB���̺� ����
	sprintf_s(sql, "insert into member values ('%s','%s','%s')", in_signup.ID, in_signup.pass, in_signup.nick);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);
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
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

	//���̵�, ����� �̿��� TABLE���ִ� DATA SELECT
	sprintf_s(sql, "select * from member where ID='%s' and pass='%s'", in_login.ID, in_login.pass);

	if(Sql_run(sql, sqlstatementhandle))
	{
		//���̵�� ����� ��ġ�ϴ°� ���� ���
		//SELECT�� DATA�� ���̵�� ��й�ȣ, �г����� ��ȯ
		SQLFetch(sqlstatementhandle);
		SQLGetData(sqlstatementhandle, 1, SQL_C_CHAR, db_login.ID, 20, NULL);
		SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, db_login.pass, 20, NULL);
		SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, nick, 40, NULL);

		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);
		return true;
	}

	return false;
}

//ȸ�� Ż�� ����
bool DB_manager::Query_leave(char* ID)
{
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

	//�̹� �α������̱� ������ ID�� �޾ƿͼ� DB TABLE�� �ִ� ID�� ��ġ�ϴ� DATA ���� ����
	sprintf_s(sql, "delete from member where ID='%s'", ID);
	if(Sql_run(sql, sqlstatementhandle))
	{	

		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);
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
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

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
		sprintf_s(sql, "select store_code, store_key from STORE where store_filter in(%s) and gps_Longitude between '%f' and '%f' and gps_Latitude between '%f' and '%f'",
			buf,
			in_search.store.longitude - ERRORRANGE, in_search.store.longitude + ERRORRANGE,
			in_search.store.latitude - ERRORRANGE, in_search.store.latitude + ERRORRANGE);

		if(Sql_run(sql, sqlstatementhandle))
		{
			if(SQLFetch(sqlstatementhandle))
			{
				//�̹��� ���Ϳ� �ֱ� ���� ����
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &Image_list.store_code, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, Image_list.store_path, 40, NULL);
				//�̹��� ��� ������ �̿��� ������ READ�ؼ� ���Ϳ� ����
				Imagevector.push_back(Image_list);
			}

			SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);

			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);
			return true;
		}

		return false;
}

//�̹��� ��� ����
bool DB_manager::Query_image_register(IN_Report in_report, OUT_Report &out_report)
{
	char nick[41];
	int sns_id;
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

	int filter_id;
	int store_code;
	//��Ʈ AND������ ���� ����� ������ ���Ͱ��� �˾Ƴ���
	for ( int i = 0; i < filter_no ; i++)
		if( in_report.filter & filter[i])
		{
			filter_id = i;
			break;
		}
			//���� ���
	sprintf_s(sql, "insert into STORE(store_key,gps_longitude,gps_latitude,store_filter) values (%s,%f,%f,%d)", in_report.store.image.buf, in_report.store.longitude, in_report.store.latitude,in_report.filter);

	if(Sql_run(sql, sqlstatementhandle))
	{
		//�ڵ��� ���� �Ŀ� ���� Ű���� �̿��� ���� �ڵ尪�� �˾Ƴ���
		SQLCloseCursor(sqlstatementhandle);
		sprintf_s(sql, "select store_code from STORE where store_key='%s'", in_report.store.image.buf);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLFetch(sqlstatementhandle);
			SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &store_code, 4, NULL);
			SQLCloseCursor(sqlstatementhandle);
		}
		//�ڵ��� ���� �Ŀ� ���̵� �̿��� �г��� ������

		sprintf_s(sql,"select nick from member where ID='%s'",in_report.ID);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLFetch(sqlstatementhandle);
			SQLGetData(sqlstatementhandle, 1, SQL_C_CHAR, nick, 40, NULL);
			SQLCloseCursor(sqlstatementhandle);
		}
		//�����ڵ带 �̿��� �ǰ��� INSERT
		sprintf_s(sql, "insert into SNS(nick, store_code, sns_con, score) values ('%s',%d,'%s',%d)", nick, store_code, in_report.comment, in_report.comment_score);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLCloseCursor(sqlstatementhandle);
			
			sprintf_s(sql,"select sns_id from SNS where store_code=%d",store_code);

			if(Sql_run(sql, sqlstatementhandle))
			{
				SQLFetch(sqlstatementhandle);
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &sns_id, 4, NULL);
				SQLCloseCursor(sqlstatementhandle);
			}

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);

			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);
			out_report.code = store_code;
			memcpy_s(out_report.opi[0].comment,sizeof(out_report.opi[0].comment),in_report.comment,sizeof(in_report.comment));
			out_report.opi[0].dislike_cnt = 0;
			out_report.opi[0].like_cnt = 0;
			strcpy_s(out_report.opi[0].nick,40,nick);
			out_report.opi[0].sns_id = sns_id;
			out_report.opi[0].dislike_cnt = 0;
			out_report.opi_cnt = 1;
			out_report.result = 1;
			out_report.score = (float)in_report.comment_score;
			return true;
		}
		else
		{
			out_report.result = 0;
			return false;
		}
	}
	return false;

}

bool DB_manager::Query_opi_search(IN_More in_more, OUT_More &out_more)
{
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}
	char* sort_no = "good";
	if(in_more.sort == 1 )
		sort_no = "good";
	else
		sort_no = "date";


	int i=0;
	//�ǰ� �˻� ����
	sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%c' desc limit 5 offset '%c'", in_more.code, sort_no, in_more.comment_count);

	if(Sql_run(sql, sqlstatementhandle))
	{
		while(SQLFetch(sqlstatementhandle))
		{
			SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_more.opi[i].sns_id, 4, NULL);
			SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_more.opi[i].nick, 30, NULL);
			SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_more.opi[i].comment, 400, NULL);
			SQLGetData(sqlstatementhandle, 2, SQL_INTEGER, &out_more.opi[i].like_cnt, 4, NULL);
			SQLGetData(sqlstatementhandle, 2, SQL_INTEGER, &out_more.opi[i].dislike_cnt, 4, NULL);
			i++;
		}

		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);
		out_more.opi_cnt = i+1;
	}
	else
	{
		//�˻��� �ȵ�.
		out_more.result = 0;
		return false;
	}

	sprintf_s(sql, "select AVG(b.score) from SNS a, score b where a.sns_id=b.sns_id and a.store_code='%s'", in_more.code);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_more.score, 4, NULL);

		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);
		out_more.result = 1;
		return true;
	}

	out_more.result = 0;
	return false;
}

bool DB_manager::Query_opi_register(IN_Write_comment in_write_opi, OUT_Write_comment &out_write_opi)
{
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

	int sns_id = 0;

	sprintf_s(sql, "insert into SNS(nick, store_code, sns_con,score) values (%s,%d,%s)", in_write_opi.ID, in_write_opi.code, in_write_opi.comment,in_write_opi.comment_score);

	if(Sql_run(sql, sqlstatementhandle))
		SQLCloseCursor(sqlstatementhandle);
	else 
	{
		out_write_opi.result = -1;
		return false;
	}
	sprintf_s(sql, "select top 1 sns_id from SNS order by desc");

	if(Sql_run(sql, sqlstatementhandle))
		SQLCloseCursor(sqlstatementhandle);
	else 
	{
		out_write_opi.result = -1;
		return false;
	}

	if(SQLFetch(sqlstatementhandle))
	{
		SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &sns_id, 4, NULL);

		SQLCloseCursor(sqlstatementhandle);
	}

	sprintf_s(sql, "insert into member_sns(mem_ID,sns_id) values (%s,%d)", in_write_opi.ID, sns_id);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLCloseCursor(sqlstatementhandle);

		int i=0;
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%c' desc limit 5 offset '%c'", in_write_opi.code, in_write_opi.sort, in_write_opi.comment_count);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle))
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_write_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_write_opi.opi[i].nick, 30, NULL);
				SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_write_opi.opi[i].comment, 400, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_INTEGER, &out_write_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_INTEGER, &out_write_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}

			SQLCloseCursor(sqlstatementhandle);
			out_write_opi.opi_cnt = i+1;
		}
		else
		{
			out_write_opi.result = -1;
			return false;
		}

		sprintf_s(sql, "select AVG(b.score) from SNS a, score b where a.sns_id=b.sns_id and a.store_code='%s'", in_write_opi.code);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_write_opi.score, 4, NULL);

			SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);

			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);
			return true;
		}
		out_write_opi.result = -1;
		return false;
	}
	//���� �ڵ�, �ǰ�, ����, ID�� DB�� ����Ѵ�.
	//�ǰߵ�, �ǰ� ����, ��������� �����Ѵ�.

	out_write_opi.result = 0;
	return false;
}

bool DB_manager::Query_opi_modify(IN_Modify_comment in_mod_opi, OUT_Modify_comment &out_mod_opi)
{
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

	sprintf_s(sql, "update SNS set sns_con='%s', score='%d' where sns_id='%d'", in_mod_opi.comment, in_mod_opi.comment_score, in_mod_opi.comment_num);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLCloseCursor(sqlstatementhandle);
		int i=0;

		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%c' desc limit 5 offset '%c'", in_mod_opi.code, in_mod_opi.sort, in_mod_opi.comment_count);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle))
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_mod_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_mod_opi.opi[i].nick, 30, NULL);
				SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_mod_opi.opi[i].comment, 400, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_INTEGER, &out_mod_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_INTEGER, &out_mod_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}

			SQLCloseCursor(sqlstatementhandle);
			out_mod_opi.opi_cnt = i+1;
		}
		else
		{
			out_mod_opi.result = -1;
			return false;
		}

		sprintf_s(sql, "select AVG(b.score) from SNS a, score b where a.sns_id=b.sns_id and a.store_code='%s'", in_mod_opi.code);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_mod_opi.score, 4, NULL);

			SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);

			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);
			out_mod_opi.result = 1;
			return true;
		}
		
		out_mod_opi.result = -1;
		return false;
	}

	//�۹�ȣ�� �̿��� �۳���, ������ ����
	//�ǰߵ�, �ǰ߰���, ��������� ����
	out_mod_opi.result = 0;
	return false;
}

bool DB_manager::Query_opi_delete(IN_Delete_comment in_del_opi, OUT_Delete_comment &out_del_opi)
{
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

	sprintf_s(sql, "delete from SNS where sns_id='%s'", in_del_opi.comment_num);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLCloseCursor(sqlstatementhandle);
		int i=0;

		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%c' desc limit 5 offset '%c'", in_del_opi.code, in_del_opi.sort, in_del_opi.comment_count);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle))
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_del_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_del_opi.opi[i].nick, 30, NULL);
				SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_del_opi.opi[i].comment, 400, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_INTEGER, &out_del_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_INTEGER, &out_del_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}

			SQLCloseCursor(sqlstatementhandle);
			out_del_opi.opi_cnt = i+1;
		}
		else
		{
			out_del_opi.result = -1;
			return false;
		}

		sprintf_s(sql, "select AVG(b.score) from SNS a, score b where a.sns_id=b.sns_id and a.store_code='%s'", in_del_opi.code);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_del_opi.score, 4, NULL);

			SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);

			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);
			out_del_opi.result = 1;
			return true;
		}
		
		out_del_opi.result = -1;
		return false;
	}

	//�۹�ȣ�� �̿��� �ǰ��� ����
	//�ǰߵ�, �ǰ߰���, ��������� ����
	out_del_opi.result = 0;
	return false;
}

bool DB_manager::Query_opi_like(IN_Like in_like_opi, OUT_Like &out_like_opi)
{
	SQLHANDLE* psqlconnectionhandle;
	SQLHANDLE  sqlstatementhandle;

	/*Get a connection from the pool*/
	if((psqlconnectionhandle=sqlsvrpool->GetConnectionFromPool())==0){
		cout<<"You have reached the maximum amout allowed - 5 in this example\n";
	}

	/*Get a statement handle from the connection*/
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_STMT, *psqlconnectionhandle, &sqlstatementhandle)){
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_DBC, *psqlconnectionhandle);
	}

	sprintf_s(sql, "update member_sns set val='%c' where mem_ID='%s' and sns_id='%s'", in_like_opi.like, in_like_opi.ID, in_like_opi.num);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);
		out_like_opi.result = 1;
		return true;
	}
	out_like_opi.result = 0;
	return false;
}

bool DB_manager::Sql_run(char* sql, SQLHANDLE &hStmt)
{
	if(SQL_SUCCESS!=SQLExecDirect(hStmt, (SQLCHAR*)sql, SQL_NTS))
	{
		sqlsvrpool->ShowSQLError(cout, SQL_HANDLE_STMT, hStmt);
		return false;
	}
	return true;
}