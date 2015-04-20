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
//����� �̹��� ĳ�� ��ü ����
bool ImageCacheFactory::createImageCache(string id)
{
	//��ȯ���� ¦���� �޴´�
	pair< hash_map<string,ImageCache*>::iterator, bool > p;
	
	bool result = false;
	
	ImageCache* c= new ImageCache();//�� ĳ�� ����
	//�ؽ� �ʿ� ĳ�� �߰�
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
		//�߰� ����
		ImageCache* temp = iter->second;

		//��ü ��ȯ
		if (temp !=NULL)
			delete temp;

		//�ؽ����� ����
		this->cacheMap.erase(id);

		printf_s("%s","DestroyImageCache() Success\n");
	}else{
		//�߰� ����
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
		//�߰� ����
		result = iter->second;
	}else{
		//�߰� ����
		result = NULL;
	}
	return result;
}

