#include "stdafx.h"
#include "ImageManager.h"

ImageManager* ImageManager::singleton= NULL;
ImageManager* ImageManager::getImageManager()
{
	if(ImageManager::singleton==NULL)
	{
		ImageManager::singleton = new ImageManager();
	}
	return ImageManager::singleton;
}
//1)비교후 선택된 이미지 2)받은 이미지 3)이미지 리스트
bool ImageManager::matchingImage(__out Imagelist& image,__in Memory& memory, __in vector<Imagelist>& imageList)
{
	bool result = false;
	SiftEngine* sift = SiftEngine::getSiftEngine();
	
	cv::Mat target;
	//Create key
	if(!sift->createKey(memory,target))
	{
		goto END;
	}
	//compare target with a compared list
	if(!sift->matchingImageWithVector(image,target,imageList))
	{
		goto END;
	}
	result = true;
	END:
	return result;
}
//1)비교후 선택된 이미지 2)받은 이미지 3)이미지 리스트
bool ImageManager::matchingImageWithCache(__out ImageBufferElement& image,__in Memory& memory, __in vector<ImageBufferElement>& imageList)
{
	bool result = false;
	SiftEngine* sift = SiftEngine::getSiftEngine();
	
	cv::Mat target;
	//Create key
	if(!sift->createKey(memory,target))
	{
		goto END;
	}
	//compare target with a compared list
	if(!sift->matchingImageWithCache(image,target,imageList))
	{
		goto END;
	}
	result = true;
	END:
	return result;
}
bool ImageManager::storeKey(__in Memory& memory,__in char* store_path)
{
	bool result = false;
	SiftEngine* sift = SiftEngine::getSiftEngine();
	if(!sift->storeKey(memory,store_path))
	{
		goto END;
	}
	result = true;
END:
	return result;
}