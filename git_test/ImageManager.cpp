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

bool ImageManager::matchingImage(__out Imagelist& image,__in Store& store, __in vector<Imagelist>& imageList)
{
	bool dwResult = false;
	//1)CreateKey , �̹��� Ű����

	//2)����Ʈ�� Ű ��
	
	//�̹����� �������� �ʴ´ٸ� return false;

	//3)code �Ҵ�
	//�̹����� �������� �ʴ´ٸ� false
	
	dwResult = true;
	
END:
	return dwResult;
}