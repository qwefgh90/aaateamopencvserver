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

bool ImageManager::matchingImage(__out u_int& code,__in Store& store, __in vector<Imagelist>& imageList)
{
	bool dwResult = false;
	//1)CreateKey , 이미지 키생성

	//2)리스트와 키 비교
	
	//이미지가 존재하지 않는다면 return false;

	//3)code 할당
	//이미지가 존재하지 않는다면 false
	
	dwResult = true;
	
END:
	return dwResult;
}