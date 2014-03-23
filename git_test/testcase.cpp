#include "stdafx.h"
#include "testcase.h"
#include "Store_manager.h"
#include "ImageManager.h"
#include "ImageCacheFactory.h"
bool getMemoryFromImage(char* fname, Memory& m)
{
	FILE* f=0;
	f = fopen(fname,"rb");
	if(f==NULL){
		printf("이미지를 찾을 수 없습니다.\n 프로세스 종료\n");
		TerminateProcess(GetCurrentProcess(),0);
	}
	fseek(f,0,SEEK_END);
	int	size = ftell(f);
	printf("파일 사이즈 : %d\n",size);
	fseek(f,0,SEEK_SET);
	m.buf = new u_char[size];
	m.len = size;
	fread(m.buf,size,1,f);
	fclose(f);
	return true;
}
void chang_cache(){
	/*char* a =NULL;
	__try{
		//*a = 'a';
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		printf( "ERROR Found. \n");
	}
	return;
*/

	Store_manager* m = Store_manager::GetStore_manager();
	ImageManager* im = ImageManager::getImageManager();
	ImageCacheFactory* icf = ImageCacheFactory::GetImageCacheFactory();

	IN_Cache c;
	c.longitude = 126.633911;
	c.latitude = 37.375160;
	strcpy(c.ID,"Master");

	m->Create_cache(c);
	
	string str(c.ID);
	ImageCache* ic = icf->getImageCache(str);
	

	ImageBufferElement ib;
	Memory mm;
	FILE * f= fopen("1395371428imgfile.jpg","rb");
	
	if(f==NULL)
	{
		printf("fopen fail\n");
		return;
	}
	fseek(f,0,SEEK_END);
	int	size = ftell(f);
	printf("파일 사이즈 : %d\n",size);
	fseek(f,0,SEEK_SET);
	mm.buf = new u_char[size];
	mm.len = size;
	fread(mm.buf,size,1,f);
	fclose(f);

	printf("hh\n");
	im->matchingImageWithCache(ib,mm,ic->imageVector);

}
hash_map<string,int> h;
void chang_test(){
	string s("안녕");
	string s1("퍼킹");
	h.insert(hash_map<string,int>::value_type(s,1234));
	h.insert(hash_map<string,int>::value_type(s1,4321));

	string s2("안녕");
	hash_map<string,int>::iterator iter = h.find(s2); 
	printf ("%d\n",iter->second);
	

	Memory m;
	Memory mem1;
	Memory mem2;
	char fname[25]={0,};
	strcpy(fname,"gs25.jpg");
	printf ("파일이름 %s\n",fname);
	getMemoryFromImage(fname,m);
	strcpy(fname,"hello_error.jpg");
	printf ("파일이름 %s\n",fname);
	getMemoryFromImage(fname,mem1);
	strcpy(fname,"immm3.jpg");
	printf ("파일이름 %s\n",fname);
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
	
	printf("키(매트릭스) 생성중 ...\n");
	if(sift->createKey(m,mat))
	{
		printf("매트릭스 생성 성공\n");
	}else
		printf("매트릭스 생성 실패\n");
//	if(sift->matchingImageWithVector(list,mat,v))
	if(imanager->matchingImage(list,m,v))
	{
		printf("매칭 성공\n");
	}else
		printf("매칭 실패\n");
	

	printf("프로세스 종료\n");
	TerminateProcess(GetCurrentProcess(),0);
}