#pragma once
#include "stdafx.h"
#include <time.h>
class SiftEngine
{
public:
	static SiftEngine* getSiftEngine();

	static const u_int MIN_MATCH = 20;

	//return Matrix
	bool createKey(__in Memory& memory,cv::Mat& m);	//Create a key for a Image
	bool storeKey(__in Memory& memory,__in char* fname);	//Store a key in a file
	bool loadKey(char* store_path,cv::Mat& mat);	//Create matrix from a file
	//return success code
	bool matchingImageWithVector(__out Imagelist& image ,cv::Mat mat, __in vector<Imagelist>& imageList);			//Match Image and vector

private:
	static SiftEngine* singleton;

};
//Create matrix from a file
typedef struct _goodMatch
{
	int index;
	u_int match_cnt;
}goodMatch;