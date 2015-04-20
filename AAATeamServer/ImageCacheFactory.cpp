#include "StdAfx.h"
#include "ImageCacheFactory.h"

ImageCacheFactory* ImageCacheFactory::singleton = NULL;
ImageCacheFactory::ImageCacheFactory(void)
{
}
ImageCacheFactory::~ImageCacheFactory(void)
{
}
//single ton
ImageCacheFactory* ImageCacheFactory::GetImageCacheFactory()
{
	if(ImageCacheFactory::singleton==NULL)
	{
		ImageCacheFactory::singleton = new ImageCacheFactory();
	}
	return ImageCacheFactory::singleton;
}
//사용자 이미지 캐쉬 객체 생성
bool ImageCacheFactory::createImageCache(string id)
{
	//반환값을 짝으로 받는다
	pair< hash_map<string,ImageCache*>::iterator, bool > p;
	
	bool result = false;
	
	ImageCache* c= new ImageCache();//빈 캐쉬 생성
	//해쉬 맵에 캐쉬 추가
	p =this->cacheMap.insert(hash_map<string,ImageCache*>::value_type(id,c));
	if(p.second==false)
	{
		printf_s("%s","ImageCacheFactory::createImageCache\nalready cache exists\n");
		delete c;
		goto END;
	}
	result = true;
	END:
	return result;
}
bool ImageCacheFactory::destroyImageCache(string id)
{
	bool result = false;
	pair< hash_map<string,ImageCache*>::iterator, bool > p;
	hash_map<string,ImageCache*>::iterator iter = this->cacheMap.find(id);
	if(iter	!= this->cacheMap.end())
	{
		//발견 성공
		ImageCache* temp = iter->second;

		//객체 반환
		if (temp !=NULL)
			delete temp;

		//해쉬에서 제거
		this->cacheMap.erase(id);

		printf_s("%s","DestroyImageCache() Success\n");
	}else{
		//발견 실패
		goto END;
	}
	result = true;
	END:
	return result;
	
}
u_int ImageCacheFactory::getCachesCount()
{
	return this->cacheMap.size();
}
u_int ImageCacheFactory::getCachesSize()
{
	u_int cnt = 0;
	return cnt;
}
ImageCache* ImageCacheFactory::getImageCache(string id)
{
	ImageCache *result= NULL;
	hash_map<string,ImageCache*>::iterator iter = this->cacheMap.find(id);
	if(iter	!= this->cacheMap.end())
	{
		//발견 성공
		result = iter->second;
	}else{
		//발견 실패
		result = NULL;
	}
	return result;
}

