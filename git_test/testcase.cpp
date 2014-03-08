#include "stdafx.h"
#include "testcase.h"
bool getMemoryFromImage(char* fname, Memory& m)
{
	FILE* f=0;
	f = fopen(fname,"rb");
	if(f==NULL){
		printf("�̹����� ã�� �� �����ϴ�.\n ���μ��� ����\n");
		TerminateProcess(GetCurrentProcess(),0);
	}
	fseek(f,0,SEEK_END);
	int	size = ftell(f);
	printf("���� ������ : %d\n",size);
	fseek(f,0,SEEK_SET);
	m.buf = new u_char[size];
	m.len = size;
	fread(m.buf,size,1,f);
	fclose(f);
	return true;
}

void chang_test(){
	Memory m;
	Memory mem1;
	Memory mem2;
	char fname[25]={0,};
	strcpy(fname,"gs25.jpg");
	printf ("�����̸� %s\n",fname);
	getMemoryFromImage(fname,m);
	strcpy(fname,"gs25_2.jpg");
	printf ("�����̸� %s\n",fname);
	getMemoryFromImage(fname,mem1);
	strcpy(fname,"gs25_3.jpg");
	printf ("�����̸� %s\n",fname);
	getMemoryFromImage(fname,mem2);
	/////////////////////////////////////////////////////////////////////////
	Imagelist l1;
	l1.store_code=11;
	strcpy(l1.store_path,"E:/11work/workspaceOpenCV/aaateamopencvserver/Debug/debug_matrixxxxxxxxxxxxx");
	Imagelist l2;
	l1.store_code=12;
	strcpy(l2.store_path,"E:/11work/workspaceOpenCV/aaateamopencvserver/Debug/debug_matrixxxxxxxxxxxxx2");
	
	vector<Imagelist> v;
	v.push_back(l1);
	v.push_back(l2);
	SiftEngine* sift = SiftEngine::getSiftEngine();
	//sift->storeKey(mem1,"debug_matrixxxxxxxxxxxxx");
	//sift->storeKey(mem2,"debug_matrixxxxxxxxxxxxx2");

	ImageManager* imanager = ImageManager::getImageManager();
	imanager->storeKey(mem1,"debug_matrixxxxxxxxxxxxx");
	imanager->storeKey(mem2,"debug_matrixxxxxxxxxxxxx2");
	/////////////////////////////////////////////////////////////////////////
	cv::Mat mat ;
	Imagelist list;
	
	printf("Ű(��Ʈ����) ������ ...\n");
	if(sift->createKey(m,mat))
	{
		printf("��Ʈ���� ���� ����\n");
	}else
		printf("��Ʈ���� ���� ����\n");
//	if(sift->matchingImageWithVector(list,mat,v))
	if(imanager->matchingImage(list,m,v))
	{
		printf("��Ī ����\n");
	}else
		printf("��Ī ����\n");
	

	printf("���μ��� ����\n");
	TerminateProcess(GetCurrentProcess(),0);
}

