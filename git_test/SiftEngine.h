#pragma once
#include "stdafx.h"

class SiftEngine
{
public:
	static SiftEngine* getSiftEngine();
	
	//return Matrix
	void loadMatrix();					//Create Image matrix from file
	//return success code
	void matchingImageWithVector();			//Match Image and vector

private:
	static SiftEngine* singleton;

};