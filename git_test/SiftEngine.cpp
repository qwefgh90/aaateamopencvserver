#include "stdafx.h"
#include "SiftEngine.h"
SiftEngine* SiftEngine::singleton= NULL;
SiftEngine* SiftEngine::getSiftEngine()
{
	if(SiftEngine::singleton==NULL)
	{
		SiftEngine::singleton = new SiftEngine();
	}
	return SiftEngine::singleton;
}
/*
	Memory : 이미지
	이미지에 대한 키값 생성
*/
bool SiftEngine::createKey(__in Memory& memory,__out cv::Mat& m)
{
	bool result=false;
	char title[255] = {0,};
	u_char* img = memory.buf;
	u_int len = memory.len;
	time_t   current_time;
	time( &current_time);
	cv::Mat db_original;
	// SIFT feature detector and feature extractor
	cv::SiftFeatureDetector detector( 0.05, 5.0 );
	cv::SiftDescriptorExtractor extractor( 3.0 );

	std::vector<cv::KeyPoint> kps_db;
	sprintf(title,"%ldn",current_time);

	//1)이미지 파일생성
	FILE* f = fopen(title,"wb");
	fwrite(img,len,1,f);
	fclose(f);
	//2)이미지 로드 및 특징점 생성 (키 생성)
	try{
	db_original = cv::imread(title,CV_LOAD_IMAGE_GRAYSCALE);
	}catch(cv::Exception& e) {
		printf("Exception occurred. createKey... ");
		printf(e.err.c_str());
		goto END;
	}

	detector.detect( db_original, kps_db );
	// Feature description 디스크립터 생성
	extractor.compute( db_original, kps_db, m);
	
	if(remove(title)==0)
	{
		//임시파일 삭제
	}else
	{
		printf("%s","Fail to delete temporary file\n");
	}

	result = true;

	END:
	return result;
}
/*
	메모리에 대한 키값(특징점 디스크립터)을 "fname"에 저장
*/
bool SiftEngine::storeKey(__in Memory& memory,__in char* fname)
{
	bool result = false;
	cv::Mat m;
	if(!createKey(memory,m))
	{
		printf("createKey() error\n");
		goto END;
	}
	try{
		cv::FileStorage storage(fname, cv::FileStorage::WRITE);
		storage << "dscr_db" <<  m;
		storage.release();
	}catch(cv::Exception& e) {
		printf("Exception occurred. storeKey()... ");
		printf(e.err.c_str());
		goto END;
	}
	//이미지 경로 복사
	strcpy((char*)memory.buf,fname);
	result = true;
	END:
	return result;
}
/*
	지정된 경로에 키를 호출
*/
bool SiftEngine::loadKey(__in char* store_path,__out cv::Mat& mat)
{
	bool result = false;
	// Feature description 디스크립터 생성
	if (store_path != NULL)
	{
		//예외 발생 가능함 by chang
		try{
			cv::FileStorage storage_(store_path, cv::FileStorage::READ);
			storage_["dscr_db"] >> mat;
			storage_.release();
		}catch(cv::Exception& e) {
			printf("Exception occurred. loadKey()... ");
			printf(e.err.c_str());
			goto END;
		}
	}else
		goto END;
	result = true;
	END:
	return result;
}
//return success code
bool SiftEngine::matchingImageWithVector(__out Imagelist& image ,__in cv::Mat mat, __in vector<Imagelist>& imageList)
{
	std::vector<goodMatch> cntSet;	//store matching results into vector
	bool result = false;
	for(int i = 0; i<(int)imageList.size(); i++)
	{
		Imagelist img = imageList[i];
		cv::Mat compareM;
		if(!loadKey(img.store_path,compareM))
		{
			goto END;
			printf("loadKey() Fail\n");
		}

		try{
		// 매칭 수행
		cv::FlannBasedMatcher matcher;               
		std::vector<cv::DMatch> matches;

		matcher.match(mat, compareM, matches);		//매칭함수 호출
		
		double max_dist = 0.0, min_dist = 100.0;

//		printf("img1 feature size : %d\n",kps_db.size());
//		printf("img2 feature size : %d\n",kps_db2.size());
		printf("path : %s\n",img.store_path);
		printf("matches size : %d\n",matches.size());
		for(int i=0; i<matches.size(); i++) {
			double dist = matches[i].distance;
			if ( dist < min_dist ) min_dist = dist;
			if ( dist > max_dist ) max_dist = dist;
		}

		// drawing only good matches (dist less than 2*min_dist)
		std::vector<cv::DMatch> good_matches;

		for (int i=0; i<matches.size(); i++) {
			if (matches[i].distance <= 2*min_dist) {
				good_matches.push_back( matches[i] );
			}
		}
		u_int size= good_matches.size();
		printf("good matches size : %ld\n",size);

		//Save count of a matching
		goodMatch m;
		m.index=i;				//image index in vector
		m.match_cnt=size;		//match size
		cntSet.push_back(m);	//save matching results to vector

		} catch(cv::Exception& e) {
			printf("Exception occurred. Match images... ");
			printf(e.err.c_str());
		}
	}
	//Best match image 
	goodMatch max;
	max.index=-1;
	max.match_cnt=0;
	
	//Find the best match image
	for (int i=0; i<cntSet.size() ; i++)
	{
		if(max.match_cnt < cntSet[i].match_cnt)
		{
			max=cntSet[i];
		}
	}
	printf("The best picture index %d, matching_count %ld\n",max.index,max.match_cnt);
	//if bigger than minimum size
	if ((max.match_cnt > SiftEngine::MIN_MATCH) && (max.index>=0))
	{
		result = true;
		image = imageList[max.index];
	}else
	{
		result = false;
		image.store_code=-1;
	}
	
	//return result
	END:
	return result;

}//Match Image and vector
