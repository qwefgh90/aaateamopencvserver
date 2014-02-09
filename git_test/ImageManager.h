#pragma once
#include "stdafx.h"

class ImageManager
{
public:
	static ImageManager* getImageManager();
	
private:
	static ImageManager* singleton;
	bool matchingImage(__out Imagelist& image,__in Store& store, __in vector<Imagelist>& imageList);
	
	//DWORD 
	//DWORD createKeyFile();

};