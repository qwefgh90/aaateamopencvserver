#include "StdAfx.h"
#include "ImageCache.h"


ImageCache::ImageCache(void)
{
}


ImageCache::~ImageCache(void)
{
}


bool ImageCache::addImageBufferElement(ImageBufferElement e)
{
	this->imageVector.push_back(e);
	return true;
}
bool ImageCache::deleteImageBufferElement(u_int store_code)
{
	bool result=false;
	int i = 0;
	for (i=0;i<imageVector.size();i++)
	{
		if(imageVector[i].store_code == store_code)
		{
			imageVector.erase(imageVector.begin()+i);
		}
	}
	result = true;
	return result;
}
bool ImageCache::clearImageVector()
{
	imageVector.clear();
	return true;
}
u_int ImageCache::getImagesCount()
{
	return imageVector.size();
}
u_int ImageCache::getImagesSize()
{
	u_int size = 0;
	return size;
}
bool ImageCache::getImageBufferElement(__in u_int store_code,__out ImageBufferElement e)
{
	bool result=false;
	int i = 0;
	for (i=0;i<imageVector.size();i++)
	{
		if(imageVector[i].store_code == store_code)
		{
			e= imageVector[i];
			result = true;
			goto END;
		}
	}

	END:
	return result;
}