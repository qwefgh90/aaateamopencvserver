#include "stdafx.h"
#include "ImageCache.h"
#pragma once
class ImageCacheFactory
{
	
private:
	static ImageCacheFactory* singleton;
	//hash_map<아이디,캐쉬포인터>
	hash_map<string,ImageCache*> cacheMap;
	ImageCacheFactory(void);
	~ImageCacheFactory(void);
public:
	static ImageCacheFactory* GetImageCacheFactory();
public:
	bool createImageCache(string id);
	bool destroyImageCache(string id);
	u_int getCachesCount();
	u_int getCachesSize();
	ImageCache* getImageCache(string id);
};