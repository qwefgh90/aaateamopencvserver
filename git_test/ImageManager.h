#pragma once
#include "stdafx.h"
#include "SiftEngine.h"
class ImageManager
{
private:
	static ImageManager* singleton;
	
public:
	static ImageManager* getImageManager();
	bool matchingImage(__out Imagelist& image,__in Memory& memory, __in vector<Imagelist>& imageList);
	bool matchingImageWithCache(__out ImageBufferElement& image,__in Memory& memory, __in vector<ImageBufferElement>& imageList);
	bool storeKey(__in Memory& memory,__in char* store_path);
	//DWORD 
	//DWORD createKeyFile();

};