#include"stdafx.h"
#include "Store_manager.h"

Store_manager* Store_manager::singleton=NULL;

//�̱���
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
	//�̹��� ����ü�� ����
	Imagelist imagelist;
	//ĳ�ð˻������� ������ ���� �̹��� ����ü ����
	ImageBufferElement matchcache;
	//�̹������͸� ���������� ����
	vector<Imagelist> Imagevector;
	
	ImageCache* Ic = NULL;
	//�̹��� ĳ�� ���丮���� ĳ�� ����
	string str(in_search.ID);
	Icf->createImageCache(str);
	//ID�� ���� �̹��� ĳ�� �޾ƿ���
	Ic = Icf->getImageCache(str);
	//NULL�� ��� �˻� ���ؾߵ�
	//ĳ�ø� �켱���� �˻�
	if(Ic != NULL && im->matchingImageWithCache(matchcache, in_search.store.image, Ic->imageVector,in_search.filter, out_search.out_list))
	{
		in_search.filter;
		out_search.code = matchcache.store_code;
		memcpy_s(out_search.store_tel,20,matchcache.store_tel,20);
		memcpy_s(out_search.name,256,matchcache.store_name,256);
		//�˻��� ������ ���� �ǰ߰˻��� ���� ����
		IN_More in_more;
		OUT_More out_more;
		//����ü�� �˻��� ������ ���ڵ��� ����
		in_more.code = matchcache.store_code;
		in_more.comment_count = 0;
		memcpy_s(in_more.cookie,64,in_search.cookie,64);
		in_more.sort = in_search.sort;
		dbm->Query_opi_search(in_more, out_more);
		//�ƿ� ��ġ�� ��Ƽ� ��ȯ		//������ ��� �ȵ��־��� (������)
		memcpy_s(out_search.opi, out_more.opi_cnt*sizeof(out_more.opi[1]), 
			out_more.opi, out_more.opi_cnt*sizeof(out_more.opi[1]));
		out_search.opi_cnt = out_more.opi_cnt;
		out_search.result = out_more.result;
		out_search.score = out_more.score;
		
		//������� �м��⿡ ��ȯ
		return true;
	}else if(!dbm->Query_images(in_search, Imagevector, Ic->imageVector)) // ĳ�� �ȿ��ִ� �̹����� �����ϰ� �˻��ؾ��ϹǷ� ���͸� ���ڷ� �־� SQL�� not in������ �̿��ؾ� �Ѵ�.
	{
		cout<<"�̹��� ���� ��Ͽ� �����Ͽ����ϴ�."<<endl;
		out_search.result = 2;
		return false;
	}
	//�̹��� ���Ϳ� ��� �̹��� ��θ� Fetch�Ͽ� ���� ��Ʈ�������� ���� ���� ��Ʈ���� ���Ϳ� ����
	//�̹��� ����, �̹��� ����Ʈ, �˻� �������� �̹����Ŵ����� ����
	if(im->matchingImage(imagelist, in_search.store.image, Imagevector,out_search.out_list))
	{
		//�̹��� �Ŵ���(&�̹�������ü, &��Ʈ����, �����ڵ带 ���� ����, &�μ�ġ);
		out_search.code = imagelist.store_code;
		memcpy(out_search.store_tel,imagelist.store_tel,20);
		memcpy_s(out_search.name,256,imagelist.store_name,256);
		//�˻��� ������ ���� �ǰ߰˻��� ���� ����
		IN_More in_more;
		OUT_More out_more;
		//����ü�� �˻��� ������ ���ڵ��� ����
		in_more.code = imagelist.store_code;
		in_more.comment_count = 0;
		memcpy_s(in_more.cookie,64,in_search.cookie,64);
		in_more.sort = 1;
		dbm->Query_opi_search(in_more, out_more);
		//�ƿ� ��ġ�� ��Ƽ� ��ȯ		//������ ��� �ȵ��־��� (������)
		memcpy_s(out_search.opi, out_more.opi_cnt*sizeof(out_more.opi[1]), 
			out_more.opi, out_more.opi_cnt*sizeof(out_more.opi[1]));
		out_search.opi_cnt = out_more.opi_cnt;
		out_search.result = out_more.result;
		out_search.score = out_more.score;
		
		//������� �м��⿡ ��ȯ
		return true;
	}
	//��Ī�� �̹����� ���� ��� �˻��� ��Ÿ���� �������� Ŭ���̾�Ʈ���� ������ �Ѵ�.
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
						out_search.out_list[i].matching = (out_search.out_list[i].matching/SiftEngine::SEARCH_PERCENT)*100;//12%�� 100%�� ȯ��
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
	//�̹��� ����ü�� ����
	//Imagelist imagelist;
	//����ϱ� �� �˻��ؼ� �̹� �ִ� �ǹ����� ã�ƺ��� ���� ���� 
	IN_Search in_search;
	OUT_Search out_search;
	//�̹������͸� ���������� ����
	vector<Imagelist> Imagevector;
	char save_path[255]={0,};
	time_t   current_time;
	time( &current_time);
	sprintf_s(save_path,"%ld",current_time);
	

	memcpy_s(in_search.cookie,64,in_report.cookie,64);
	in_search.filter = in_report.filter;
	
	//memcpy_s(in_search.ID,21,in_report.ID,21);
	//ID�� ������ �ʿ䰡 ����. 1)UNION �̶� 2)ȸ������ ��⿡�� ID -> COOKIE ��ȯ �۾����Ͼ
	
	in_search.store.image.buf = in_report.store.image.buf;
	in_search.store.image.len = in_report.store.image.len;
	in_search.store.latitude = in_report.store.latitude;
	in_search.store.longitude = in_report.store.longitude;
	in_search.sort = in_report.sort;
	
	//�켱 ��ϵ� �̹����� �ִ��� �˻�
	if(!Store_Search(in_search, out_search))
	{
		//Ű����
		if(im->storeKey(in_search.store.image,save_path))
		{
			//�˻��� ��� �ȵ��ִ� ��� �̹���,sns ���
			if(dbm->Query_image_register(in_report, out_report))
			{
				out_report.result = 1;
				return true;
			}
		}
	}
	else //�̹����� �̹� ��� �Ǿ��� ��� �ǰ��� ��Ͻ����ֱ�(�̱���)
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
	//��� �Ǿ��ִ� ��� ��ϵǾ��ִٰ� �˸�
	return false;
}

bool Store_manager::Store_more(IN_More &in_more, OUT_More &out_more)
{
	if(dbm->Query_opi_search(in_more, out_more))
		return true;
	//������ ���� �ǰ� ������
	return false;
}

bool Store_manager::Store_opi_write(IN_Write_comment &in_write, OUT_Write_comment &out_write)
{
	if(dbm->Query_opi_register(in_write, out_write))
		return true;
	//������ ���� �ǰ� ����
	return false;
}

bool Store_manager::Store_opi_delete(IN_Delete_comment &in_delete, OUT_Delete_comment &out_delete)
{
	if(dbm->Query_opi_delete(in_delete, out_delete))
		return true;
	//������ ���� �ǰ� �����ϱ�
	return false;
}

bool Store_manager::Store_opi_modify(IN_Modify_comment &in_modify, OUT_Modify_comment &out_modify)
{
	if(dbm->Query_opi_modify(in_modify, out_modify))
		return true;
	//������ ���� �ǰ� �����ϱ�
	return false;
}

bool Store_manager::Store_like(IN_Like &in_like, OUT_Like &out_like)
{
	if(dbm->Query_opi_like(in_like, out_like))
		return true;
	//�ǰ߿� ���� ���ƿ�
	return false;
}

bool Store_manager::Create_cache(IN_Cache in_cache)
{
	ImageCache* Ic = NULL;
	//�̹��� ĳ�� ���丮���� ĳ�� ����
	string str(in_cache.ID);
	Icf->createImageCache(str);
	//ID�� ���� �̹��� ĳ�� �޾ƿ���
	Ic = Icf->getImageCache(str);
	if(Ic==NULL)
		return false;
	//�ϴ� �ʱ�ȭ �����ֱ�
	Ic->clearImageVector();
	//�浵 ������ ���� �������� �ȿ��ִ� �����ڵ�, ������θ� DB���� �޾ƿ���
	//dbm->Query_Image_cache(�浵, ����, �޾ƿ� ĳ�� ����ü ����)
	//ĳ�ÿ� �浵 ���� �� �̿��� ���Ϳ� �ִ´�.
	dbm->Query_Image_cache(in_cache.longitude, in_cache.latitude, Ic->imageVector);
	printf ("imageCacheVector size : %d\n",Ic->imageVector.size());

	for(int i=0; Ic->imageVector.size()>i;i++)
		printf ("%d in CACHE\n",Ic->imageVector[i].store_code);

	//ĳ�� ����ü ���� ����
	//�޾ƿ� ĳ�� ����ü ���� �ȿ� �ִ� ���� ��θ� �̿��� xml������ �ҷ�����
	for(int i = 0; i<(int)Ic->imageVector.size(); i++)
		se->loadKey(Ic->imageVector[i].store_path, Ic->imageVector[i].key_xml);
	//se->loadKey(���� ���, ĳ�� ����ü�� �ִ� ��Ʈ����)
	//ĳ�� ����ü�� ���Ϳ� ����
	return true;
}