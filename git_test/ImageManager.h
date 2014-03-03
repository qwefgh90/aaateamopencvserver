#pragma once
#include "stdafx.h"
#include "SiftEngine.h"
class ImageManager
{
public:
	static ImageManager* getImageManager();
	
private:
	static ImageManager* singleton;
	bool matchingImage(__out Imagelist& image,__in Memory& memory, __in vector<Imagelist>& imageList);
	bool storeKey(__in Memory& memory,__in char* store_path);
	//DWORD 
	//DWORD createKeyFile();

};