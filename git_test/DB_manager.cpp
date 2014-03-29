#include"stdafx.h"
#include "DB_manager.h"

DB_manager* DB_manager::singleton=NULL;

//싱글톤
DB_manager* DB_manager::GetDB_manager()
{
	if(DB_manager::singleton == NULL)
	{
		singleton = new DB_manager();
	}
	return singleton;
}

//DB자원 할당
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

//DB 자원해제
DB_manager::~DB_manager(void)
{
	/*Destroy the database pool*/
	if(sqlsvrpool->DestroyPool()>0){
		cout<<"There are still some un-released connections in the pool\n";
	}

	delete sqlsvrpool;
}

//회원가입 쿼리
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

	//아이디, 비번, 닉네임을 DB테이블에 삽입
	sprintf_s(sql, "insert into member values ('%s','%s','%s')", in_signup.ID, in_signup.pass, in_signup.nick);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLCloseCursor(sqlstatementhandle);

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

//로그인 쿼리
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

	//아이디, 비번을 이용해 TABLE에있는 DATA SELECT
	sprintf_s(sql, "select * from member where ID='%s' and pass='%s'", in_login.ID, in_login.pass);

	if(Sql_run(sql, sqlstatementhandle))
	{
		//아이디와 비번이 일치하는게 있을 경우
		//SELECT한 DATA의 아이디와 비밀번호, 닉네임을 반환
		SQLFetch(sqlstatementhandle);
		SQLGetData(sqlstatementhandle, 1, SQL_C_CHAR, db_login.ID, 20, NULL);
		SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, db_login.pass, 20, NULL);
		SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, nick, 40, NULL);
		
		SQLCloseCursor(sqlstatementhandle);

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

//회원 탈퇴 쿼리
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

	//이미 로그인중이기 때문에 ID만 받아와서 DB TABLE에 있는 ID가 일치하는 DATA 행을 삭제
	sprintf_s(sql, "delete from member where ID='%s'", ID);
	if(Sql_run(sql, sqlstatementhandle))
	{	
		
		SQLCloseCursor(sqlstatementhandle);

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

//이미지 검색 쿼리
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

	//필터를 담기위한 버퍼생성
	char buf[50];
	//필터의 번호를 저장하기 위한 버퍼
	char c[5];
	//버퍼 초기화
	memset(buf,0,50);
	//이미지 리스트를 생성
	Imagelist Image_list;

	//필터 쿼리를 만들기 위한 for문
	for ( int i = 0; i < filter_no ; i++)
		if( in_search.filter & filter[i])
		{
			strcat_s(buf,"'");
			_itoa_s(filter[i],c,10);
			strcat_s(buf, c);
			strcat_s(buf,"'");
			strcat_s(buf,",");
		}
		*(strrchr(buf,','))=NULL;
		//필터값과 경도/위도를 이용해 상점코드 받아오기
		sprintf_s(sql, "select store_code, store_key from STORE where store_filter in(%s) and gps_Longitude between '%f' and '%f' and gps_Latitude between '%f' and '%f'",
			buf,
			in_search.store.longitude - ERRORRANGE, in_search.store.longitude + ERRORRANGE,
			in_search.store.latitude - ERRORRANGE, in_search.store.latitude + ERRORRANGE);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				//이미지 벡터에 넣기 위해 선언
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &Image_list.store_code, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, Image_list.store_path, 256, NULL);
				//이미지 경로 변수를 이용해 파일을 READ해서 벡터에 저장
				Imagevector.push_back(Image_list);
			}
			
			SQLCloseCursor(sqlstatementhandle);

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

//이미지 등록 쿼리
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
	//비트 AND연산을 통해 등록할 상점의 필터값을 알아내기
	for ( int i = 0; i < filter_no ; i++)
		if( in_report.filter & filter[i])
		{
			filter_id = i;
			break;
		}
			//상점 등록
	sprintf_s(sql, "insert into STORE(store_key,gps_longitude,gps_latitude,store_filter) values (%s,%f,%f,%d)", in_report.store.image.buf, in_report.store.longitude, in_report.store.latitude,in_report.filter);

	if(Sql_run(sql, sqlstatementhandle))
	{
		//핸들을 닫은 후에 상점 키값을 이용해 상점 코드값을 알아내기
		SQLCloseCursor(sqlstatementhandle);
		sprintf_s(sql, "select store_code from STORE where store_key='%s'", in_report.store.image.buf);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLFetch(sqlstatementhandle);
			SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &store_code, 4, NULL);
			SQLCloseCursor(sqlstatementhandle);
		}
		//핸들을 닫은 후에 아이디를 이용해 닉네임 얻어오기

		sprintf_s(sql,"select nick from member where ID='%s'",in_report.ID);

		if(Sql_run(sql, sqlstatementhandle))
		{
			SQLFetch(sqlstatementhandle);
			SQLGetData(sqlstatementhandle, 1, SQL_C_CHAR, nick, 40, NULL);
			SQLCloseCursor(sqlstatementhandle);
		}
		//상점코드를 이용해 의견을 INSERT
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

//의견 검색 쿼리
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
	//의견 검색 쿼리
	sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%s' desc OFFSET %d ROWS FETCH NEXT 10 ROWS ONLY;", in_more.code, sort_no, in_more.comment_count);

	if(Sql_run(sql, sqlstatementhandle))
	{
		while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
		{
			SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_more.opi[i].sns_id, 4, NULL);
			SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_more.opi[i].nick, 30, NULL);
			SQLGetData(sqlstatementhandle, 3, SQL_C_CHAR, out_more.opi[i].comment, 400, NULL);
			SQLGetData(sqlstatementhandle, 4, SQL_INTEGER, &out_more.opi[i].like_cnt, 4, NULL);
			SQLGetData(sqlstatementhandle, 5, SQL_INTEGER, &out_more.opi[i].dislike_cnt, 4, NULL);
			i++;
		}
		
		SQLCloseCursor(sqlstatementhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);
		out_more.opi_cnt = i;
		printf("opi_cnt %d\n",i);
	}
	else
	{
		//검색이 안됨.
		out_more.result = 0;
		return false;
	}

	sprintf_s(sql, "select AVG(cast(score as Float)) avg from SNS where store_code=%d", in_more.code);

	if(Sql_run(sql, sqlstatementhandle))
	{
		SQLFetch(sqlstatementhandle);
		SQLGetData(sqlstatementhandle, 1, SQL_C_FLOAT, &out_more.score, sizeof(float), NULL);
		SQLCloseCursor(sqlstatementhandle);

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
	
	//핸들을 닫은 후에 아이디를 이용해 닉네임 얻어오기

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

		char* sort_no = "good";
		if(in_write_opi.sort == 1 )
			sort_no = "good";
		else
			sort_no = "date";


		int i=0;
		//의견 검색 쿼리
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code=%d order by %s desc OFFSET %d ROWS FETCH NEXT 10 ROWS ONLY;", in_write_opi.code, sort_no, in_write_opi.comment_count);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_write_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_write_opi.opi[i].nick, 30, NULL);
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

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);

			/*Release the connection back into the pool*/
			sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

			/*Dispaly the pool information*/
			cout<<(*sqlsvrpool);
			out_write_opi.result = 1;
			return true;
		}
		out_write_opi.result = -1;
		return false;
	}
	//상점 코드, 의견, 평점, ID를 DB에 등록한다.
	//의견들, 의견 개수, 평균평점을 리턴한다.

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
		//의견 검색 쿼리
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%s' desc OFFSET %d ROWS FETCH NEXT 10 ROWS ONLY;", in_mod_opi.code, sort_no, in_mod_opi.comment_count);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_mod_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_mod_opi.opi[i].nick, 30, NULL);
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

	//글번호를 이용해 글내용, 평점을 수정
	//의견들, 의견개수, 평균평점을 리턴
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
		//의견 검색 쿼리
		sprintf_s(sql, "select sns_id, nick, sns_con, good, bed from SNS where store_code='%d' order by '%s' desc OFFSET %d ROWS FETCH NEXT 10 ROWS ONLY;", in_del_opi.code, sort_no, in_del_opi.comment_count);

		if(Sql_run(sql, sqlstatementhandle))
		{
			while(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &out_del_opi.opi[i].sns_id, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, out_del_opi.opi[i].nick, 30, NULL);
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

	//글번호를 이용해 의견을 삭제
	//의견들, 의견개수, 평균평점을 리턴
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

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);

		/*Release the connection back into the pool*/
		sqlsvrpool->ReleaseConnectionToPool(psqlconnectionhandle);

		/*Dispaly the pool information*/
		cout<<(*sqlsvrpool);
		out_like_opi.result = 1;
		return true;
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
//경도, 위도, 받을 캐시 구조체
bool DB_manager::Query_Image_cache(float longitude, float latitude, vector<ImageBufferElement>& Ibev)
{
	//경도 위도를 이용해 일점 범위 안에있는 상점들의 코드, 경로명을 받아오기
	
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

	sprintf_s(sql, "select store_code,store_key,gps_Latitude,gps_Longitude from STORE where gps_Longitude between '%f' and '%f' and gps_Latitude between '%f' and '%f'",
		longitude - ERRORRANGE, longitude + ERRORRANGE,
		latitude - ERRORRANGE, latitude + ERRORRANGE);

	//캐시 구조체 선언
	ImageBufferElement Ibe;
	//위도,경도 거리 임시저장할 변수 선언
	float t_long;
	float t_lati;
	if(Sql_run(sql, sqlstatementhandle))
	{
		//검색된거 일단 저장
		for(int i= 0; i <50 ; i++)
			if(SQLFetch(sqlstatementhandle) == SQL_SUCCESS)
			{
				//이미지 벡터에 넣기 위해 선언
				SQLGetData(sqlstatementhandle, 1, SQL_INTEGER, &Ibe.store_code, 4, NULL);
				SQLGetData(sqlstatementhandle, 2, SQL_C_CHAR, Ibe.store_path, 256, NULL);
				SQLGetData(sqlstatementhandle, 3, SQL_C_FLOAT, &Ibe.latitude, 4, NULL);
				SQLGetData(sqlstatementhandle, 4, SQL_C_FLOAT, &Ibe.longitude, 4, NULL);
				//받아온 위도,경도를 현재 위치와 빼서 거리를 저장
				t_long =  abs(longitude - Ibe.longitude);
				t_lati =  abs(latitude - Ibe.latitude);
				Ibe.longitude = t_long*40000/360*cos(Ibe.latitude);
				Ibe.latitude = t_lati*40000/360;
				Ibe.distance = sqrt(pow(Ibe.longitude,2) + pow(Ibe.latitude,2));
				Ibev.push_back(Ibe);
			}
		//현재 위치와 상점 위치간의 거리 비교 후 qsort이용
			//Compare함수는 stdafx.cpp에있음
			sort(Ibev.begin(), Ibev.end(), Compare);
			for(int i= 0; ((int)Ibev.size() > 10) && (i < 40); i++)
				Ibev.erase(Ibev.begin()+10);
		return true;
	}
	//구조체에 저장
	return false;
}
//경도 위도를 km로 환산 하여 구해야함

//db에서 받아온 좌표를 벡터에 임시 저장하고 현재 좌표 - db에서 받아온 좌표를 절대값으로 설정한 후
//벡터를 sort해서 오름차순으로 정렬한다. 그 후 최대 10개를 이미지 벡터에 저장
