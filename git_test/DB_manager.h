#pragma once

#include"stdafx.h"
#include"sha512.h"
#include "SQLSvrPool.h"


class DB_manager
{
private:
	static DB_manager* singleton;	
	
	SQLSvrPool* sqlsvrpool;

	TCHAR sql[1000];

	unsigned char shacode[64];

public:
	DB_manager(void);
	~DB_manager(void);
	
	static DB_manager* GetDB_manager();

	bool Sql_run(TCHAR* sql, SQLHANDLE &hStmt);

	bool Query_signup(IN_Signup in_signup);
	bool Query_login(IN_Login in_login, IN_Login &db_login,char* nick);
	bool Query_leave(char* ID);
	bool Query_images(IN_Search in_search, vector<Imagelist> &Imagevector, vector<ImageBufferElement> &imageVector);
	bool Query_image_register(IN_Report in_report, OUT_Report &out_report);
	bool Query_opi_search(IN_More in_more, OUT_More &out_more);
	bool Query_opi_register(IN_Write_comment in_write_opi, OUT_Write_comment &out_write_opi);
	bool Query_opi_modify(IN_Modify_comment in_mod_opi, OUT_Modify_comment &out_mod_opi);
	bool Query_opi_delete(IN_Delete_comment in_del_opi, OUT_Delete_comment &out_del_opi);
	bool Query_opi_like(IN_Like in_like_opi, OUT_Like &out_like_opi);
	bool Query_Image_cache(float 경도, float 위도, vector<ImageBufferElement>& Ibev);
};