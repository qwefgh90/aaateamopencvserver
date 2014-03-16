#include "stdafx.h"
#pragma once
class ImageCache
{
public:
	vector<ImageBufferElement> imageVector;
	ImageCache(void);
	~ImageCache(void);
	bool addImageBufferElement(ImageBufferElement e);
	bool deleteImageBufferElement(u_int store_code);
	bool clearImageVector();
	u_int getImagesCount();
	u_int getImagesSize();
	bool getImageBufferElement(__in u_int store_code, __out ImageBufferElement e);
};

