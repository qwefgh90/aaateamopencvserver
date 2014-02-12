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
	//이미지 매니저(&이미지구조체, &매트릭스, 상점코드를 담은 벡터, &인서치);
	//이미지 벡터, 이미지 리스트, 검색 구조제를 이미지매니저로 전달

	//아웃 서치에 담아서 반환

	//결과값을 분석기에 반환
	return false;
}

bool Store_manager::Store_report(IN_Report &in_report, OUT_Report &out_report)
{
	//이미지 등록
	dbm->Query_image_register(in_report, out_report);
	return false;
}

bool Store_manager::Store_more(IN_More &in_more, OUT_More &out_more)
{
	dbm->Query_opi_search(in_more, out_more);
	//상점에 대한 의견 더보기
	return false;
}

bool Store_manager::Store_opi_write(IN_Write_comment &in_write, OUT_Write_comment &out_write)
{
	dbm->Query_opi_register(in_write, out_write);
	//상점에 대한 의견 쓰기
	return false;
}

bool Store_manager::Store_opi_delete(IN_Delete_comment &in_delete, OUT_Delete_comment &out_delete)
{
	dbm->Query_opi_delete(in_delete, out_delete);
	//상점에 대한 의견 삭제하기
	return false;
}

bool Store_manager::Store_opi_modify(IN_Modify_comment &in_modify, OUT_Modify_comment &out_modify)
{
	dbm->Query_opi_modify(in_modify, out_modify);
	//상점에 대한 의견 수정하기
	return false;
}

bool Store_manager::Store_like(IN_Like &in_like, OUT_Like &out_like)
{
	dbm->Query_opi_like(in_like, out_like);
	//의견에 대한 좋아요
	return false;
}
