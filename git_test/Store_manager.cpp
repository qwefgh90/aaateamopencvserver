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
	mm = Member_manager::GetMember_manager();
	se = SiftEngine::getSiftEngine();
	Icf = ImageCacheFactory::GetImageCacheFactory();
}


Store_manager::~Store_manager(void)
{

}

bool Store_manager::Store_Search(IN_Search &in_search, OUT_Search &out_search)
{
	//이미지 구조체를 선언
	Imagelist imagelist;
	//캐시검색성공후 저장을 위한 이미지 구조체 선언
	ImageBufferElement matchcache;
	//이미지벡터를 지역변수로 선언
	vector<Imagelist> Imagevector;
	
	ImageCache* Ic = NULL;
	//이미지 캐시 팩토리에서 캐시 생성
	string str(in_search.ID);
	Icf->createImageCache(str);
	//ID에 대한 이미지 캐시 받아오기
	Ic = Icf->getImageCache(str);
	//NULL일 경우 검사 안해야됨
	//캐시를 우선으로 검색
	if(Ic != NULL && im->matchingImageWithCache(matchcache, in_search.store.image, Ic->imageVector,in_search.filter, out_search.out_list))
	{
		in_search.filter;
		out_search.code = matchcache.store_code;
		memcpy_s(out_search.store_tel,20,matchcache.store_tel,20);
		memcpy_s(out_search.name,256,matchcache.store_name,256);
		//검색된 상점에 대한 의견검색을 위해 선언
		IN_More in_more;
		OUT_More out_more;
		//구조체에 검색한 상점의 인자들을 저장
		in_more.code = matchcache.store_code;
		in_more.comment_count = 0;
		memcpy_s(in_more.cookie,64,in_search.cookie,64);
		in_more.sort = in_search.sort;
		dbm->Query_opi_search(in_more, out_more);
		//아웃 서치에 담아서 반환		//개수가 고려 안되있었음 (수정완)
		memcpy_s(out_search.opi, out_more.opi_cnt*sizeof(out_more.opi[1]), 
			out_more.opi, out_more.opi_cnt*sizeof(out_more.opi[1]));
		out_search.opi_cnt = out_more.opi_cnt;
		out_search.result = out_more.result;
		out_search.score = out_more.score;
		
		//결과값을 분석기에 반환
		return true;
	}else if(!dbm->Query_images(in_search, Imagevector, Ic->imageVector)) // 캐시 안에있는 이미지는 제외하고 검색해야하므로 벡터를 인자로 넣어 SQL의 not in구문을 이용해야 한다.
	{
		cout<<"이미지 벡터 등록에 실패하였습니다."<<endl;
		out_search.result = 2;
		return false;
	}
	//이미지 벡터에 담긴 이미지 경로를 Fetch하여 읽은 매트릭스값을 새로 만든 매트릭스 벡터에 저장
	//이미지 벡터, 이미지 리스트, 검색 구조제를 이미지매니저로 전달
	if(im->matchingImage(imagelist, in_search.store.image, Imagevector,out_search.out_list))
	{
		//이미지 매니저(&이미지구조체, &매트릭스, 상점코드를 담은 벡터, &인서치);
		out_search.code = imagelist.store_code;
		memcpy(out_search.store_tel,imagelist.store_tel,20);
		memcpy_s(out_search.name,256,imagelist.store_name,256);
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
	//매칭된 이미지가 없을 경우 검색시 나타났던 상점들을 클라이언트에게 보내야 한다.
	else
	{
		if(out_search.out_list.size() == 0)
		{
			out_search.result = 2;
			return false;
		}
		else
		{
			if(out_search.out_list.size() > 1)
			{
				vector<OUT_List> temp;
				for(int i=0;out_search.out_list.size() > i ; i++)
				{
					if(out_search.out_list[i].matching > SiftEngine::MIN_PERCENT)
					{
						out_search.out_list[i].matching = (out_search.out_list[i].matching/SiftEngine::SEARCH_PERCENT)*100;//12%를 100%로 환산
						temp.push_back(out_search.out_list[i]);
					}
				}
				bubble_sort(temp);
				for(int i= 0; (int)temp.size() > 5; i++)
					temp.erase(temp.begin()+5);
				out_search.out_list = temp;
				for(int i= 0; (int)out_search.out_list.size() > i; i++){
					OUT_List ele = out_search.out_list[i];
					printf("list[%d].matching : %f\n",i,ele.matching);
				}
			}
			out_search.result = 8;
			return true;
		}
	}
	out_search.result = 2;
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
	in_search.sort = in_report.sort;
	
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
	else //이미지가 이미 등록 되었을 경우 의견을 등록시켜주기(미구현)
	{
		IN_Write_comment in_write_opi;
		OUT_Write_comment out_write_opi;
		in_write_opi.code = out_search.code;
		strcpy_s(in_write_opi.comment,in_report.comment);
		in_write_opi.comment_count = 0;
		in_write_opi.comment_score = in_report.comment_score;
		memcpy_s(in_write_opi.cookie,64,in_report.cookie,64);
		in_write_opi.sort = 2;

		dbm->Query_opi_register(in_write_opi, out_write_opi);
		out_report.code = out_search.code;
		memcpy_s(out_report.opi, out_write_opi.opi_cnt*sizeof(out_write_opi.opi[1]), 
			out_write_opi.opi, out_write_opi.opi_cnt*sizeof(out_write_opi.opi[1]));
		out_report.opi_cnt = out_write_opi.opi_cnt;
		out_report.score = out_write_opi.score;
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

bool Store_manager::Create_cache(IN_Cache in_cache)
{
	ImageCache* Ic = NULL;
	//이미지 캐시 팩토리에서 캐시 생성
	string str(in_cache.ID);
	Icf->createImageCache(str);
	//ID에 대한 이미지 캐시 받아오기
	Ic = Icf->getImageCache(str);
	if(Ic==NULL)
		return false;
	//일단 초기화 시켜주기
	Ic->clearImageVector();
	//경도 위도를 통해 일정범위 안에있는 상점코드, 상점경로를 DB에서 받아오기
	//dbm->Query_Image_cache(경도, 위도, 받아올 캐시 구조체 벡터)
	//캐시에 경도 위도 를 이용해 벡터에 넣는다.
	dbm->Query_Image_cache(in_cache.longitude, in_cache.latitude, Ic->imageVector);
	printf ("imageCacheVector size : %d\n",Ic->imageVector.size());

	for(int i=0; Ic->imageVector.size()>i;i++)
		printf ("%d in CACHE\n",Ic->imageVector[i].store_code);

	//캐시 구조체 벡터 선언
	//받아온 캐시 구조체 벡터 안에 있는 상점 경로를 이용해 xml파일을 불러오기
	for(int i = 0; i<(int)Ic->imageVector.size(); i++)
		se->loadKey(Ic->imageVector[i].store_path, Ic->imageVector[i].key_xml);
	//se->loadKey(상점 경로, 캐시 구조체에 있는 매트릭스)
	//캐시 구조체를 벡터에 저장
	return true;
}