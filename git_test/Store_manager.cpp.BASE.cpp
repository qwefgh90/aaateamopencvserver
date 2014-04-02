#include"stdafx.h"
#include "Store_manager.h"

Store_manager* Store_manager::singleton=NULL;

//싱글톤
Store_manager* Store_manager::GetStore_manager()
{
	if(Store_manager::singleton == NULL)
	{
		singleton = new Store_manager();
	}
	return singleton;
}

Store_manager::Store_manager(void)
{
	dbm = DB_manager::GetDB_manager();
	im = ImageManager::getImageManager();
}


Store_manager::~Store_manager(void)
{

}

bool Store_manager::Store_Search(IN_Search &in_search, OUT_Search &out_search)
{
	//이미지 구조체를 선언
	Imagelist imagelist;
	//이미지벡터를 지역변수로 선언
	vector<Imagelist> Imagevector;
	dbm->Query_images(in_search, Imagevector);
	//이미지 벡터에 담긴 이미지 경로를 Fetch하여 읽은 매트릭스값을 새로 만든 매트릭스 벡터에 저장
	//이미지 벡터, 이미지 리스트, 검색 구조제를 이미지매니저로 전달
	if(im->matchingImage(imagelist, in_search.store.image, Imagevector))
	{
		//이미지 매니저(&이미지구조체, &매트릭스, 상점코드를 담은 벡터, &인서치);
		out_search.code = imagelist.store_code;
		//검색된 상점에 대한 의견검색을 위해 선언
		IN_More in_more;
		OUT_More out_more;
		//구조체에 검색한 상점의 인자들을 저장
		in_more.code = imagelist.store_code;
		in_more.comment_count = 0;
		memcpy_s(in_more.cookie,64,in_search.cookie,64);
		in_more.sort = 1;
		dbm->Query_opi_search(in_more, out_more);
		//아웃 서치에 담아서 반환		//개수가 고려 안되있었음 (수정완)
		memcpy_s(out_search.opi, out_more.opi_cnt*sizeof(out_more.opi[1]), 
			out_more.opi, out_more.opi_cnt*sizeof(out_more.opi[1]));
		out_search.opi_cnt = out_more.opi_cnt;
		out_search.result = out_more.result;
		out_search.score = out_more.score;
		
		//결과값을 분석기에 반환
		return true;
	}

	return false;
}

bool Store_manager::Store_report(IN_Report &in_report, OUT_Report &out_report)
{
	//이미지 구조체를 선언
	//Imagelist imagelist;
	//등록하기 전 검색해서 이미 있는 건물인지 찾아보기 위해 선언 
	IN_Search in_search;
	OUT_Search out_search;
	//이미지벡터를 지역변수로 선언
	vector<Imagelist> Imagevector;
	char save_path[255]={0,};
	time_t   current_time;
	time( &current_time);
	sprintf_s(save_path,"%ld",current_time);
	

	memcpy_s(in_search.cookie,64,in_report.cookie,64);
	in_search.filter = in_report.filter;
	
	//memcpy_s(in_search.ID,21,in_report.ID,21);
	//ID는 복사할 필요가 없음. 1)UNION 이라서 2)회원관리 모듈에서 ID -> COOKIE 변환 작업이일어남
	
	in_search.store.image.buf = in_report.store.image.buf;
	in_search.store.image.len = in_report.store.image.len;
	in_search.store.latitude = in_report.store.latitude;
	in_search.store.longitude = in_report.store.longitude;
	
	
	//우선 등록된 이미지가 있는지 검색
	if(!Store_Search(in_search, out_search))
	{
		//키생성
		if(im->storeKey(in_search.store.image,save_path))
		{
			//검색후 등록 안되있는 경우 이미지,sns 등록
			if(dbm->Query_image_register(in_report, out_report))
			{
				out_report.result = 1;
				return true;
			}
		}
	}
	else
	{
		out_report.code = out_search.code;
		memcpy_s(out_report.opi, out_search.opi_cnt*sizeof(out_search.opi[1]), 
			out_search.opi, out_search.opi_cnt*sizeof(out_search.opi[1]));
		out_report.opi_cnt = out_search.opi_cnt;
		out_report.score = out_search.score;
		out_report.result = 4;
	}
	//등록 되어있는 경우 등록되어있다고 알림
	return false;
}

bool Store_manager::Store_more(IN_More &in_more, OUT_More &out_more)
{
	if(dbm->Query_opi_search(in_more, out_more))
		return true;
	//상점에 대한 의견 더보기
	return false;
}

bool Store_manager::Store_opi_write(IN_Write_comment &in_write, OUT_Write_comment &out_write)
{
	if(dbm->Query_opi_register(in_write, out_write))
		return true;
	//상점에 대한 의견 쓰기
	return false;
}

bool Store_manager::Store_opi_delete(IN_Delete_comment &in_delete, OUT_Delete_comment &out_delete)
{
	if(dbm->Query_opi_delete(in_delete, out_delete))
		return true;
	//상점에 대한 의견 삭제하기
	return false;
}

bool Store_manager::Store_opi_modify(IN_Modify_comment &in_modify, OUT_Modify_comment &out_modify)
{
	if(dbm->Query_opi_modify(in_modify, out_modify))
		return true;
	//상점에 대한 의견 수정하기
	return false;
}

bool Store_manager::Store_like(IN_Like &in_like, OUT_Like &out_like)
{
	if(dbm->Query_opi_like(in_like, out_like))
		return true;
	//의견에 대한 좋아요
	return false;
}
