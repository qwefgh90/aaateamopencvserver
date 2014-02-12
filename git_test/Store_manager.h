#pragma once

#include"stdafx.h"
#include"DB_manager.h"

class Store_manager
{
private:
	static Store_manager* singleton;	
	
	DB_manager* dbm;

public:
	Store_manager(void);
	~Store_manager(void);
	
	static Store_manager* GetStore_manager();

	bool Store_Search(IN_Search &in_search, OUT_Search &out_search);
	bool Store_report(IN_Report &in_report, OUT_Report &out_report);
	bool Store_more(IN_More &in_more, OUT_More &out_more);
	bool Store_opi_write(IN_Write_comment &in_write, OUT_Write_comment &out_write);
	bool Store_opi_delete(IN_Delete_comment &in_delete, OUT_Delete_comment &out_delete);
	bool Store_opi_modify(IN_Modify_comment &in_modify, OUT_Modify_comment &out_modify);
	bool Store_like(IN_Like &in_like, OUT_Like &out_like);


};

