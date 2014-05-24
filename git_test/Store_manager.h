#pragma once

#include"stdafx.h"
#include"DB_manager.h"
#include "ImageManager.h"
#include "Member_manager.h"
#include "SiftEngine.h"
#include "ImageCacheFactory.h"
#include "ImageCache.h"

class Store_manager
{
private:
	static Store_manager* singleton;	
	
	DB_manager* dbm;
	ImageManager* im;
	Member_manager* mm;
	SiftEngine* se;
	ImageCacheFactory* Icf;

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
	void swap(float* a, float* b){
		u_int tmp=*a;
		*a=*b;
		*b=tmp;
	}

	void bubble_sort( vector<OUT_List> &list){
		int i,j;
		int isSwaped;

		for(i=0; i<list.size()-1; i++){
			isSwaped=FALSE;
			for(j=0; j<list.size()-1-i; j++){
				if(list[j].matching<list[j+1].matching){
					swap(&list[j].matching,&list[j+1].matching);
					isSwaped=TRUE;
				}
			}
			if(isSwaped==FALSE) {
				break;
			}
		}
	}
	//위도 경도 쿠키
	bool Create_cache(IN_Cache in_cache);

};

