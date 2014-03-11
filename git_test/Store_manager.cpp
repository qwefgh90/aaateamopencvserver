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
}


Store_manager::~Store_manager(void)
{

}

bool Store_manager::Store_Search(IN_Search &in_search, OUT_Search &out_search)
{
	//�̹��� ����ü�� ����
	Imagelist imagelist;
	//�̹������͸� ���������� ����
	vector<Imagelist> Imagevector;
	dbm->Query_images(in_search, Imagevector);
	//�̹��� ���Ϳ� ��� �̹��� ��θ� Fetch�Ͽ� ���� ��Ʈ�������� ���� ���� ��Ʈ���� ���Ϳ� ����
	//�̹��� ����, �̹��� ����Ʈ, �˻� �������� �̹����Ŵ����� ����
	if(im->matchingImage(imagelist, in_search.store.image, Imagevector))
	{
		//�̹��� �Ŵ���(&�̹�������ü, &��Ʈ����, �����ڵ带 ���� ����, &�μ�ġ);
		out_search.code = imagelist.store_code;
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
	else
	{
		out_report.code = out_search.code;
		memcpy_s(out_report.opi, out_search.opi_cnt*sizeof(out_search.opi[1]), 
			out_search.opi, out_search.opi_cnt*sizeof(out_search.opi[1]));
		out_report.opi_cnt = out_search.opi_cnt;
		out_report.score = out_search.score;
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
