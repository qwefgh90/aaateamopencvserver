#include "StdAfx.h"
#include "MenuAnalyzer.h"

MenuAnalyzer* MenuAnalyzer::singleton=NULL;

MenuAnalyzer::MenuAnalyzer(void){
	//initialize member manager
	member_manager = Member_manager::GetMember_manager();
	store_manager = Store_manager::GetStore_manager();
}
MenuAnalyzer::~MenuAnalyzer(void){
}
//Parse menu to structures and call a function of assembling packet
//param
//@data received from mobile 
//return
//@buffer and size to send to mobile
bool MenuAnalyzer::MenuSelector(Memory& in_memory,Memory& out_memory)
{
	bool dwResult=FALSE;
	u_char* in_buf=in_memory.buf;			//buffer from mobile(complete buffer available releasing)
	u_int in_len=in_memory.len;				//buffer length from mobile

	u_char proto_type;		//proto_type
	u_char err_code=1;		//erorr_code
	proto_type = *((u_char*)(in_buf+4));
	printf ("proto: %d\n",proto_type);
	switch(proto_type)
	{
	case LOGIN:
		{
		IN_Login in;	//로그인 요청 구조체
		OUT_Login out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		//1)패킷에서 구조체 생성

		if(this->packetToLogin(in,in_memory))
		{
			printf("id: %s@@@@\n",in.ID);
			printf("passwd: %s@@@@\n",in.pass);
			//2)생성된 구조체를 각 모듈에 전달
			if(member_manager->Login(in,out))
			{
				dumpbyte(out.cookie,64);
				printf("nick: %s@@@\n",out.nick);
				printf("result: %d@@@\n",out.result);

				//3)생성된 구조체에서 패킷을 생성
				if(this->packetFromLogin(out_memory,out))
				{
					
				}
				else{
					err_code = out.result=3;
					goto ERRORCODE;
				}
				//4)모바일에서 패킷 전송
			}else
			{
				err_code = out.result;
				goto ERRORCODE;
			}
		}else
		{
			err_code = out.result=3;
			goto ERRORCODE;
		}

		break;
		}
	case SIGNUP:
		{
		IN_Signup in;	//로그인 요청 구조체
		OUT_Signup out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		if(packetToSignup(in,in_memory))
		{
			printf("id: %s@@@@\n",in.ID);
			printf("passwd: %s@@@@\n",in.pass);
			printf("nick: %s@@@@\n",in.nick);

			//2)생성된 구조체를 각 모듈에 전달
			if(member_manager->Signup(in,out))
			{
				printf("result: %d@@@@\n",out.result);
				//3)생성된 구조체에서 패킷을 생성
				if(this->packetFromSignup(out_memory,out))
				{

				}else
				{
					err_code = out.result=3;
					goto ERRORCODE;
				}
				//4)모바일에서 패킷 전송
			}else{
				err_code = out.result;
				goto ERRORCODE;
			}
		}else
		{
			err_code = out.result=3;
			goto ERRORCODE;
		}
		break;
		}
	case LOGOUT://로그아웃
		{
		IN_Logout in;	//로그인 요청 구조체
		OUT_Logout out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		if(packetToLogout(in,in_memory))
		{
			if(member_manager->Logout(in,out))
			{
				printf("result: %d@@@\n",out.result);

				if(this->packetFromLogout(out_memory,out))
				{

				}else
				{
					err_code = out.result=3;
					goto ERRORCODE;
				}
			}else{
				err_code = out.result;
				goto ERRORCODE;
			}
		}else
		{
			err_code = out.result=3;
			goto ERRORCODE;
		}
		break;
		}
	case LEAVE://회원탈퇴
		{
		IN_Leave in;	//로그인 요청 구조체
		OUT_Leave out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToLeave(in,in_memory);
		
		member_manager->Leave(in,out);
		
		printf("result: %d@@@\n",out.result);
		
		this->packetFromLeave(out_memory,out);
		break;
		}
	case SEARCH://상점 검색
		{
		IN_Search in;	//로그인 요청 구조체
		OUT_Search out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToSearch(in,in_memory);
		//1)쿠키검사
		if(member_manager->cookiechk(in.ID,in.cookie))
		{
			printf("쿠키성공\n");
			if(store_manager->Store_Search(in,out))
			{
				printf("result: %d @@@\ncode : %u\nscore%f\nopi_cnt : %u\n",out.result,out.code,out.score,out.opi_cnt);
				for (int i = 0 ; i < out.opi_cnt ; i++)
				{
					printf("%u : %s,%s,%u,%u,%u\n",i,out.opi[i].comment,out.opi[i].nick,out.opi[i].dislike_cnt,out.opi[i].like_cnt,out.opi[i].sns_id);
				}
				printf("검색성공\n");
				if(packetFromSearch(out_memory,out))
				{
					printf("패킷생성성공\n");
				}else{
					err_code = out.result;
					dwResult=false;
					goto ERRORCODE;
				}
			}else{
				printf("Error Store Search \n");
				err_code = out.result;
				dwResult=false;
				goto ERRORCODE;
			}

		}else{
			err_code = out.result;
			dwResult=false;
			goto ERRORCODE;
		}
		//2)상점검색

		//3)패킷조립
		
		break;
		}
	case MORE://더보기
		{
		IN_More in;	//로그인 요청 구조체
		OUT_More out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToMore(in,in_memory);
		
		break;
		}
	case WRITE_COMMENT://의견남기기
		{
		IN_Write_comment in;	//로그인 요청 구조체
		OUT_Write_comment out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToWriteComment(in,in_memory);
		// false== f멤버 매니저 쿠키 인증
		if(member_manager->cookiechk(in.ID,in.cookie))
		{
			if(store_manager->Store_opi_write(in,out))
			{
				if(packetFromWriteComment(out_memory,out))
				{
					printf("result: %d @@@\nscore%f\nopi_cnt : %u\n",out.result,out.score,out.opi_cnt);
					for (int i = 0 ; i < out.opi_cnt ; i++)
					{
						printf("%u : %s,%s,%u,%u,%u\n",i,out.opi[i].comment,out.opi[i].nick,out.opi[i].dislike_cnt,out.opi[i].like_cnt,out.opi[i].sns_id);
					}	
				}
				else{
					err_code = out.result = 3;
					goto ERRORCODE;
				}
			}else
			{
				err_code = out.result;
				goto ERRORCODE;
			}
		}else
		{
				err_code = out.result;
				goto ERRORCODE;
		}
		
		// true==
		// store_manager()
		break;
		}
	case MODIFY_COMMENT://의견수정
		{
		IN_Modify_comment in;	//로그인 요청 구조체
		OUT_Modify_comment out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToModifyComment(in,in_memory);
		
		break;
		}
	case DELETE_COMMENT://의견삭제
		{
		IN_Delete_comment in;	//로그인 요청 구조체
		OUT_Delete_comment out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToDeleteComment(in,in_memory);

		break;
		}
	case LIKE://좋아요
		{
		IN_Like in;	//로그인 요청 구조체
		OUT_Like out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToLike(in,in_memory);
		
		break;
		}
	case REPORT://상점 제보
		{
		IN_Report in;	//로그인 요청 구조체
		OUT_Report out;	//로그인 응답 구조체
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		if(!packetToReport(in,in_memory))
		{
			//파싱 실패
			err_code=out.code=3;	//에러코드 세팅
			goto ERRORCODE;
		}
		//1)쿠키체크/인증과정
		if(member_manager->cookiechk(in.ID,in.cookie))
		{
			//성공
		}else
		{
			//인증 실패
			err_code=out.code=5;	//에러코드 세팅
			goto ERRORCODE;
		}
		
		//2)상점등록
		if(store_manager->Store_report(in,out))
		{
			//성공
			printf("상점 등록 성공\n");
		}else
		{
			//실패
			printf("상점 등록 실패\n");
			err_code=out.code;	//에러코드 세팅
			//goto ERRORCODE;	//에러코드가 아닌 정보를 관련 전송해줌
			
		}

		//3)패킷조립
		packetFromReport(out_memory,out);


		printf("result: %d @@@\ncode : %u\nscore%f\nopi_cnt : %u\n",out.result,out.code,out.score,out.opi_cnt);
		for (int i = 0 ; i < out.opi_cnt ; i++)
		{
			printf("%u : %s,%s,%u,%u,%u\n",i,out.opi[i].comment,out.opi[i].nick,out.opi[i].dislike_cnt,out.opi[i].like_cnt,out.opi[i].sns_id);
		}
	
		break;
		}

	}
	//정상 패킷
	dwResult= true;
	return dwResult;

ERRORCODE:
	//에러 처리 패킷 생성
	printf("------  SEND ERROR PACKET  -----\n");
	packetFromError(out_memory,err_code);
	dwResult= true;

	return dwResult;
}

MenuAnalyzer* MenuAnalyzer::GetMenuAnalyzer()
{
	if(MenuAnalyzer::singleton==NULL)
	{
		singleton = new MenuAnalyzer();
	}
	return singleton;
}
/*
vector<string> MenuAnalyzer::split_(string str, string sep){
    char* cstr = const_cast<char*>(str.c_str());
    char* current;
    vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current != NULL){
        arr.push_back(current);
        current=strtok(NULL, sep.c_str());
    }
    return arr;
}*/

vector<string> MenuAnalyzer::split(string str, string sep){
    char* cstr = const_cast<char*>(str.c_str());
    char* current = cstr;
	char* next=  NULL;
    vector<std::string> arr;

    next=strstr(cstr,sep.c_str());
	if(next!=NULL)
	{
		int i = 0;
		for (i=0 ; i<sep.length();i++)
		{
			next[i]=NULL;
		}
		next= next+sep.length();

	}
    arr.push_back(current);

    while(next != NULL){
		current = next; 
		next=strstr(next,sep.c_str());
		if(next!=NULL)
		{
			int i = 0;
			for (i=0 ; i<sep.length();i++)
			{
				next[i]=NULL;
			}
			next= next+sep.length();
		}
        arr.push_back(current);   
    }
    return arr;
}