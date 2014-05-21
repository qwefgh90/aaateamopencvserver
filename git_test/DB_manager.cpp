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
	if(sqlsvrpool->CreatePool(10, 100)<=0){
		cout<<"Error creating database pool\n";
		cout<<sqlsvrpool->GetLastSystemError()<<endl;	//If it's asystem error
	}

	/*Dispaly the pool information*/
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
	sprintf_s(sql, "insert into member values ('%s',HASHBYTES('sha2_512','%s'),'%s')", in_signup.ID, in_signup.pass, in_signup.nick);
	
	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLCloseCursor(sqlstatementhandle);


		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);
		
		/*Dispaly the pool information*/
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
	sprintf_s(sql, "select * from member where ID='%s' and pass=HASHBYTES('sha2_512','%s')", in_login.ID, in_login.pass);

	if(Sql_run(sql, sqlstatementhandle))
	{
		//���̵�� ����� ��ġ�ϴ°� ���� ���
		//SELECT�� DATA�� ���̵�� ��й�ȣ, �г����� ��ȯ
		SQLFetch(sqlstatementhandle);
		SQLGetData(sqlstatementhandle, 1, SQL_C_CHAR, db_login.ID, 30, NULL);
		SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, db_login.pass, 65, NULL);
		SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, nick, 16, NULL);
		
		SQLCloseCursor(sqlstatementhandle);


		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);
		
		/*Dispaly the pool information*/
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
		
		SQLCloseCursor(sqlstatementhandle);


		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		return true;
	}
	else
	{
		return false;
	}
}
bool DB_manager::Query_changepw(IN_Chpw in_chpw)
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
	sprintf_s(sql, "update member set pass=HASHBYTES('sha2_512','%s') where ID='%s'", in_chpw.newPass, in_chpw.ID);
	if(Sql_run(sql, sqlstatementhandle))
	{	
		
		SQLCloseCursor(sqlstatementhandle);


		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		return true;
	}
	else
	{
		return false;
	}

}

//�̹��� �˻� ����
bool DB_manager::Query_images(IN_Search in_search, vector<Imagelist> &Imagevector, vector<ImageBufferElement> &imageVector)
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
	char not[200];
	//������ ��ȣ�� �����ϱ� ���� ����
	char c[5];
	//���� �ʱ�ȭ
	memset(not,0,200);
	memset(buf,0,50);
	//�̹��� ����Ʈ�� ����
	Imagelist Image_list;

	//���� ������ ����� ���� for��
	for ( int i = 0; i < filter_no ; i++)
		if( in_search.filter & filter[i])
		{
			strcat_s(buf,"'");
			_itoa_s(filter[i],c,10);
			strcat_s(buf, c);
			strcat_s(buf,"'");
			strcat_s(buf,",");
		}
		if(*buf==0)
		{
			printf("filter value is not exist\n");
			return false;
		}
		*(strrchr(buf,','))=NULL;
		strcat_s(not, "-1,");
		for( int i = 0; i < (int)imageVector.size() ; i++)
		{
			_itoa_s(imageVector[i].store_code,c,10);
			strcat_s(not, c);
			strcat_s(not, ",");
		}
		*(strrchr(not,','))=NULL;
		//���Ͱ��� �浵/������ �̿��� �����ڵ� �޾ƿ���
		sprintf_s(sql, "select store_code, store_key,store_filter from STORE where store_filter in(%s) and store_code not in (%s) and 2000 > dbo.fn_distance(gps_Latitude,gps_Longitude,'%f','%f')",
			buf, not, in_search.store.latitude, in_search.store.longitude);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				//�̹��� ���Ϳ� �ֱ� ���� ����
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &Image_list.store_code, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, Image_list.store_path, 256, NULL);
				//�̹��� ��� ������ �̿��� ������ READ�ؼ� ���Ϳ� ����
				Imagevector.push_back(Image_list);
			}

			SQLCloseCursor(sqlstatementhandle);


			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			return true;
		}

		return false;
}

//�̹��� ��� ����
bool DB_manager::Query_image_register(IN_Report in_report, OUT_Report &out_report)
{
	char nick[17]={0,};
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
	sprintf_s(sql, "insert into STORE(store_key,gps_Longitude,gps_Latitude,store_filter) values (%s,%f,%f,%d)", in_report.store.image.buf, in_report.store.longitude, in_report.store.latitude,in_report.filter);

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
			SQLGetData(sqlstatementhandle, 1, SQL_C_CHAR, nick, 16, NULL);
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


			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			out_report.code = store_code;

			memcpy_s(out_report.opi[0].comment,sizeof(out_report.opi[0].comment),in_report.comment,sizeof(in_report.comment));
			//printf("src: %s dest: %s\n",in_report.comment, out_report.opi[0].comment);
			
			out_report.opi[0].dislike_cnt = 0;
			out_report.opi[0].like_cnt = 0;
			strcpy_s(out_report.opi[0].nick,16,nick);
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

//�ǰ� �˻� ����
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
	if(in_more.sort == 0 ) // 0�� ���ƿ��, 1�� �ֽż�
		sort_no = "good";
	else
		sort_no = "date";
	

	int i=0;
	//�ǰ� �˻� ����
	sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by \"%s\" desc OFFSET %d ROWS FETCH NEXT 10 ROWS ONLY;", in_more.code, sort_no, in_more.comment_count);

	if(Sql_run(sql, sqlstatementhandle))
	{
		while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
		{
			SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_more.opi[i].sns_id, 4, NULL);
			SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_more.opi[i].nick, 16, NULL);
			SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_more.opi[i].comment, 400, NULL);
			SQLGetData(sqlstatementhandle, 4, SQL_INTEGER, &out_more.opi[i].like_cnt, 4, NULL);
			SQLGetData(sqlstatementhandle, 5, SQL_INTEGER, &out_more.opi[i].dislike_cnt, 4, NULL);
			i++;
		}
		
		SQLCloseCursor(sqlstatementhandle);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		out_more.opi_cnt = i;
		printf("opi_cnt %d\n",i);
	}
	else
	{
		//�˻��� �ȵ�.
		out_more.result = 0;
		return false;
	}

	sprintf_s(sql, "select AVG(cast(score as Float)) avg from SNS where store_code=%d", in_more.code);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLFetch(sqlstatementhandle);
		SQLGetData(sqlstatementhandle, 1, SQL_C_FLOAT, &out_more.score, sizeof(float), NULL);
		SQLCloseCursor(sqlstatementhandle);


		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);
		
		/*Dispaly the pool information*/
		out_more.result = 1;
		return true;
	}

	out_more.result = 0;
	return false;
}

bool DB_manager::Query_opi_register(IN_Write_comment in_write_opi, OUT_Write_comment &out_write_opi)
{
	char nick[41];
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
	
	//�ڵ��� ���� �Ŀ� ���̵� �̿��� �г��� ������

	sprintf_s(sql,"select nick from member where ID='%s'",in_write_opi.ID);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLFetch(sqlstatementhandle);
		SQLGetData(sqlstatementhandle, 1, SQL_C_CHAR, nick, 40, NULL);
		SQLCloseCursor(sqlstatementhandle);
	}

	int sns_id = 0;

	sprintf_s(sql, "insert into SNS(nick, store_code, sns_con,score) values ('%s',%d,'%s',%d)", nick, in_write_opi.code, in_write_opi.comment,in_write_opi.comment_score);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLCloseCursor(sqlstatementhandle);

		char* sort_no = "date";
		if(in_write_opi.sort == 0 )
			sort_no = "good";
		else
			sort_no = "date";


		int i=0;
		//�ǰ� �˻� ����
		sprintf_s(sql, "select TOP 10 sns_id, nick, sns_con, good, bed from SNS where store_code=%d order by %s desc;", in_write_opi.code, sort_no);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_write_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_write_opi.opi[i].nick, 16, NULL);
				SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_write_opi.opi[i].comment, 400, NULL);
				SQLGetData(sqlstatementhandle, 4, SQL_INTEGER, &out_write_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(sqlstatementhandle, 5, SQL_INTEGER, &out_write_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}
			SQLCloseCursor(sqlstatementhandle);
			out_write_opi.opi_cnt = i;
		}
		else
		{
			out_write_opi.result = -1;
			return false;
		}
		
		sprintf_s(sql, "select AVG(cast(score as Float)) from SNS a where a.store_code=%d", in_write_opi.code);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLFetch(sqlstatementhandle);

			SQLGetData(sqlstatementhandle, 1, SQL_C_FLOAT, &out_write_opi.score, sizeof(float), NULL);

			SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle );


			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			out_write_opi.result = 1;
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
		char* sort_no = "good";
		if(in_mod_opi.sort == 1 )
			sort_no = "good";
		else
			sort_no = "date";


		int i=0;
		//�ǰ� �˻� ����
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%s' desc OFFSET %d ROWS FETCH NEXT 10 ROWS ONLY;", in_mod_opi.code, sort_no, in_mod_opi.comment_count);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_mod_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_mod_opi.opi[i].nick, 16, NULL);
				SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_mod_opi.opi[i].comment, 400, NULL);
				SQLGetData(sqlstatementhandle, 4, SQL_INTEGER, &out_mod_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(sqlstatementhandle, 5, SQL_INTEGER, &out_mod_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}
			SQLCloseCursor(sqlstatementhandle);
			out_mod_opi.opi_cnt = i;
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


			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
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

		char* sort_no = "good";
		if(in_del_opi.sort == 1 )
			sort_no = "good";
		else
			sort_no = "date";


		int i=0;
		//�ǰ� �˻� ����
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%s' desc OFFSET %d ROWS FETCH NEXT 10 ROWS ONLY;", in_del_opi.code, sort_no, in_del_opi.comment_count);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_del_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_del_opi.opi[i].nick, 16, NULL);
				SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_del_opi.opi[i].comment, 400, NULL);
				SQLGetData(sqlstatementhandle, 4, SQL_INTEGER, &out_del_opi.opi[i].like_cnt, 4, NULL);
				SQLGetData(sqlstatementhandle, 5, SQL_INTEGER, &out_del_opi.opi[i].dislike_cnt, 4, NULL);
				i++;
			}
			SQLCloseCursor(sqlstatementhandle);
			out_del_opi.opi_cnt = i;
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
			
			SQLCloseCursor(sqlstatementhandle);


			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
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

	sprintf_s(sql, "insert into member_sns values('%s',%d,%d)", in_like_opi.ID, in_like_opi.num, in_like_opi.like);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLCloseCursor(sqlstatementhandle);

		if(in_like_opi.like == 1)
		{
			sprintf_s(sql, "UPDATE SNS SET good = good + 1 WHERE sns_id = %d;", in_like_opi.num);

			if(Sql_run(sql, sqlstatementhandle))
			{
				SQLCloseCursor(sqlstatementhandle);
				/*Release the connection back into the pool*/
				sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

				/*Dispaly the pool information*/
				out_like_opi.result = 1;
				return true;
			}
		}
		else if(in_like_opi.like == 2)
		{
			sprintf_s(sql, "UPDATE SNS SET bed = bed + 1 WHERE sns_id = %d;", in_like_opi.num);

			if(Sql_run(sql, sqlstatementhandle))
			{
				SQLCloseCursor(sqlstatementhandle);
				/*Release the connection back into the pool*/
				sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

				/*Dispaly the pool information*/
				out_like_opi.result = 1;
				return true;
			}
		}

	}
	else
	{
		out_like_opi.result = 6;
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
//�浵, ����, ���� ĳ�� ����ü
bool DB_manager::Query_Image_cache(float longitude, float latitude, vector<ImageBufferElement>& Ibev)
{
	//�浵 ������ �̿��� ���� ���� �ȿ��ִ� �������� �ڵ�, ��θ��� �޾ƿ���
	
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

	sprintf_s(sql, "select store_code,store_key,gps_Latitude,gps_Longitude,store_filter from STORE where 1500 > dbo.fn_distance(gps_Latitude,gps_Longitude,'%f','%f')", latitude, longitude);

	//ĳ�� ����ü ����
	ImageBufferElement Ibe;
	int filter = 0;
	if(Sql_run(sql, sqlstatementhandle))
	{
		//�˻��Ȱ� �ϴ� ����
		for(int i= 0; i <50 ; i++)
			if(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				//�̹��� ���Ϳ� �ֱ� ���� ����
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &Ibe.store_code, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, Ibe.store_path, 256, NULL);
				SQLGetData(sqlstatementhandle, 3, SQL_C_FLOAT, &Ibe.latitude, 4, NULL);
				SQLGetData(sqlstatementhandle, 4, SQL_C_FLOAT, &Ibe.longitude, 4, NULL);
				SQLGetData(sqlstatementhandle, 5, SQL_INTEGER, &filter, 4, NULL);
				Ibe.filter = (u_char)filter;
				printf("cache filter ; %d\n",Ibe.filter);
				//�Ÿ� ���ϴ� �Լ� ���
				
				Ibe.distance = distance(Ibe.latitude, Ibe.longitude, latitude, longitude);
				Ibev.push_back(Ibe);
			}
		//���� ��ġ�� ���� ��ġ���� �Ÿ� �� �� qsort�̿�
			//Compare�Լ��� stdafx.cpp������
			sort(Ibev.begin(), Ibev.end(), Compare);
			//ĳ�ÿ� ������ ����� 21���̻��̰� i�� 30������ ������ ���� -> 50������ ������ ĳ�̵� ��� 20�������� ����� ����, 50���� ��� ĳ�� �ɰ�� 30���� �����
			for(int i= 0; ((int)Ibev.size() > 20) && (i < 30); i++)
				Ibev.erase(Ibev.begin()+10);

			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/

		return true;
	}
	//����ü�� ����
	return false;
}
//�浵 ������ km�� ȯ�� �Ͽ� ���ؾ���

//�� ��ǥ�� �Ÿ� ���ϱ�
float DB_manager::distance(float P1_latitude, float P1_longitude, float P2_latitude, float P2_longitude)
{
	if ((P1_latitude == P2_latitude) && (P1_longitude == P2_longitude)) {
		return 0;
	}
	float e10 = P1_latitude * M_PI / 180;
	float e11 = P1_longitude * M_PI / 180;
	float e12 = P2_latitude * M_PI / 180;
	float e13 = P2_longitude * M_PI / 180;
	/* Ÿ��ü GRS80 */
	float c16 = 6356752.314140910;
	float c15 = 6378137.000000000;
	float c17 = 0.0033528107;
	float f15 = c17 + c17 * c17;
	float f16 = f15 / 2;
	float f17 = c17 * c17 / 2;
	float f18 = c17 * c17 / 8;
	float f19 = c17 * c17 / 16;
	float c18 = e13 - e11;
	float c20 = (1 - c17) * tan(e10);
	float c21 = atan(c20);
	float c22 = sin(c21);
	float c23 = cos(c21);
	float c24 = (1 - c17) * tan(e12);
	float c25 = atan(c24);
	float c26 = sin(c25);
	float c27 = cos(c25);
	float c29 = c18;
	float c31 = (c27 * sin(c29) * c27 * sin(c29))
		+ (c23 * c26 - c22 * c27 * cos(c29))
		* (c23 * c26 - c22 * c27 * cos(c29));
	float c33 = (c22 * c26) + (c23 * c27 * cos(c29));
	float c35 = sqrt(c31) / c33;
	float c36 = atan(c35);
	float c38 = 0;
	if (c31 == 0) {
		c38 = 0;
	} else {
		c38 = c23 * c27 * sin(c29) / sqrt(c31);
	}
	float c40 = 0;
	if ((cos(asin(c38)) * cos(asin(c38))) == 0) {
		c40 = 0;
	} else {
		c40 = c33 - 2 * c22 * c26
			/ (cos(asin(c38)) * cos(asin(c38)));
	}
	float c41 = cos(asin(c38)) * cos(asin(c38))
		* (c15 * c15 - c16 * c16) / (c16 * c16);
	float c43 = 1 + c41 / 16384
		* (4096 + c41 * (-768 + c41 * (320 - 175 * c41)));
	float c45 = c41 / 1024 * (256 + c41 * (-128 + c41 * (74 - 47 * c41)));
	float c47 = c45
		* sqrt(c31)
		* (c40 + c45
		/ 4
		* (c33 * (-1 + 2 * c40 * c40) - c45 / 6 * c40
		* (-3 + 4 * c31) * (-3 + 4 * c40 * c40)));
	float c50 = c17
		/ 16
		* cos(asin(c38))
		* cos(asin(c38))
		* (4 + c17
		* (4 - 3 * cos(asin(c38))
		* cos(asin(c38))));
	float c52 = c18
		+ (1 - c50)
		* c17
		* c38
		* (acos(c33) + c50 * sin(acos(c33))
		* (c40 + c50 * c33 * (-1 + 2 * c40 * c40)));
	float c54 = c16 * c43 * (atan(c35) - c47);
	// return distance in meter
	return c54;
}