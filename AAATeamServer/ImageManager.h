#pragma once
#include "stdafx.h"
#include "SiftEngine.h"
class ImageManager
{
private:
	static ImageManager* singleton;
	SiftEngine* sift;
	ImageManager();
public:
	static ImageManager* getImageManager();
	bool matchingImage(__out Imagelist& image,__in Memory& memory, __in vector<Imagelist>& imageList,std::vector<OUT_List>& out_list);
	bool matchingImageWithCache(__out ImageBufferElement& image,__in Memory& memory, __in vector<ImageBufferElement>& imageList, u_char filter,std::vector<OUT_List>& out_list);
	bool storeKey(__in Memory& memory,__in char* store_path);
	//DWORD 
	//DWORD createKeyFile();

};