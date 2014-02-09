#include "StdAfx.h"
#include "MenuAnalyzer.h"

MenuAnalyzer* MenuAnalyzer::singleton=NULL;

MenuAnalyzer::MenuAnalyzer(void){
}
MenuAnalyzer::~MenuAnalyzer(void){
}
//parse menu to structures and call assemble packet function
//param
//@data received from mobile 
//return
//@buffer and size to send to mobile
DWORD MenuAnalyzer::MenuSelector(Memory& in_memory,Memory& out_memory)
{
	DWORD dwResult=FALSE;
	u_char* out_buf=NULL;		//buffer to mobile
	u_int out_len=0;			//buffer length to mobile

	u_char* in_buf=in_memory.buf;			//buffer from mobile(complete buffer available releasing)
	u_int in_len=in_memory.len;				//buffer length from mobile

	u_char proto_type;		//proto_type

	proto_type = *((u_char*)(in_buf+4));
	printf ("proto: %d\n",proto_type);
	switch(proto_type)
	{
	case LOGIN:
		{
		IN_Login in;	//�α��� ��û ����ü
		OUT_Login out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		//1)��Ŷ���� ����ü ����
		this->packetToLogin(in,in_memory);
		printf("id: %s@@@@",in.ID);
		printf("passwd: %s@@@@",in.pass);
		//2)������ ����ü�� �� ��⿡ ����
		
		//3)������ ����ü���� ��Ŷ�� ����
		
		//4)����Ͽ��� ��Ŷ ����
		
		break;
		}
	case SIGNUP:
		{
		IN_Signup in;	//�α��� ��û ����ü
		OUT_Signup out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToSignup(in,in_memory);
		printf("id: %s@@@@",in.ID);
		printf("passwd: %s@@@@",in.pass);
		printf("nick: %s@@@@",in.nick);
		//2)������ ����ü�� �� ��⿡ ����
		
		//3)������ ����ü���� ��Ŷ�� ����

		//4)����Ͽ��� ��Ŷ ����
		break;
		}
	case LOGOUT:
		{
		IN_Logout in;	//�α��� ��û ����ü
		OUT_Logout out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToLogout(in,in_memory);
		break;
		}
	case LEAVE:
		{
		IN_Leave in;	//�α��� ��û ����ü
		OUT_Leave out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToLeave(in,in_memory);

		break;
		}
	case SEARCH:
		{
		IN_Search in;	//�α��� ��û ����ü
		OUT_Search out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToSearch(in,in_memory);

		break;
		}
	case MORE:
		{
		IN_More in;	//�α��� ��û ����ü
		OUT_More out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToMore(in,in_memory);

		break;
		}
	case WRITE_COMMENT:
		{
		IN_Write_comment in;	//�α��� ��û ����ü
		OUT_Write_comment out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToWriteComment(in,in_memory);

		break;
		}
	case MODIFY_COMMENT:
		{
		IN_Modify_comment in;	//�α��� ��û ����ü
		OUT_Modify_comment out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToModifyComment(in,in_memory);

		break;
		}
	case DELETE_COMMENT:
		{
		IN_Delete_comment in;	//�α��� ��û ����ü
		OUT_Delete_comment out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToDeleteComment(in,in_memory);


		break;
		}
	case LIKE:
		{
		IN_Like in;	//�α��� ��û ����ü
		OUT_Like out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToLike(in,in_memory);

		break;
		}
	case REPORT:
		{
		IN_Report in;	//�α��� ��û ����ü
		OUT_Report out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToReport(in,in_memory);

		break;
		}

	}
	out_memory.len=out_len;
	out_memory.buf=out_buf;
	dwResult= TRUE;
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

vector<string> MenuAnalyzer::split(string str, string sep){
    char* cstr = const_cast<char*>(str.c_str());
    char* current;
    vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current != NULL){
        arr.push_back(current);
        current=strtok(NULL, sep.c_str());
    }
    return arr;
}