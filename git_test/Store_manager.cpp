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
	//�̹��� �Ŵ���(&�̹�������ü, &��Ʈ����, �����ڵ带 ���� ����, &�μ�ġ);
	//�̹��� ����, �̹��� ����Ʈ, �˻� �������� �̹����Ŵ����� ����

	//�ƿ� ��ġ�� ��Ƽ� ��ȯ

	//������� �м��⿡ ��ȯ
	return false;
}

bool Store_manager::Store_report(IN_Report &in_report, OUT_Report &out_report)
{
	//�̹��� ����ü�� ����
	Imagelist imagelist;
	//����ϱ� �� �˻��ؼ� �̹� �ִ� �ǹ����� ã�ƺ��� ���� ���� 
	IN_Search in_search;
	//�̹������͸� ���������� ����
	vector<Imagelist> Imagevector;

	memcpy_s(in_search.cookie,64,in_report.cookie,64);
	in_search.filter = in_report.filter;
	memcpy_s(in_search.ID,21,in_report.ID,21);
	in_search.store.image.buf = in_report.store.image.buf;
	in_search.store.image.len = in_report.store.image.len;
	in_search.store.latitude = in_report.store.latitude;
	in_search.store.longitude = in_report.store.longitude;

	//�켱 ��ϵ� �̹����� �ִ��� �˻�
	dbm->Query_images(in_search, Imagevector);
	//�̹������͸� �̿��� �̹��� �Ŵ����� �ҷ� �˻�
	//�̹��� �Ŵ���(&�̹�������ü, &��Ʈ����, �����ڵ带 ���� ����, &�μ�ġ);
	//�˻��� ��� �ȵ��ִ� ��� �̹��� ���
	dbm->Query_image_register(in_report, out_report);
	//��� �Ǿ��ִ� ��� ��ϵǾ��ִٰ� �˸�
	return false;
}

bool Store_manager::Store_more(IN_More &in_more, OUT_More &out_more)
{
	dbm->Query_opi_search(in_more, out_more);
	//������ ���� �ǰ� ������
	return false;
}

bool Store_manager::Store_opi_write(IN_Write_comment &in_write, OUT_Write_comment &out_write)
{
	dbm->Query_opi_register(in_write, out_write);
	//������ ���� �ǰ� ����
	return false;
}

bool Store_manager::Store_opi_delete(IN_Delete_comment &in_delete, OUT_Delete_comment &out_delete)
{
	dbm->Query_opi_delete(in_delete, out_delete);
	//������ ���� �ǰ� �����ϱ�
	return false;
}

bool Store_manager::Store_opi_modify(IN_Modify_comment &in_modify, OUT_Modify_comment &out_modify)
{
	dbm->Query_opi_modify(in_modify, out_modify);
	//������ ���� �ǰ� �����ϱ�
	return false;
}

bool Store_manager::Store_like(IN_Like &in_like, OUT_Like &out_like)
{
	dbm->Query_opi_like(in_like, out_like);
	//�ǰ߿� ���� ���ƿ�
	return false;
}
