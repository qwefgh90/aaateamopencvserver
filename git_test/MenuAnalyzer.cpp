#include "StdAfx.h"
#include "MenuAnalyzer.h"

MenuAnalyzer* MenuAnalyzer::singleton=NULL;

MenuAnalyzer::MenuAnalyzer(void){
	//initialize member manager
	member_manager = Member_manager::GetMember_manager();
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
		printf("id: %s@@@@\n",in.ID);
		printf("passwd: %s@@@@\n",in.pass);
		//2)������ ����ü�� �� ��⿡ ����
		member_manager->Login(in,out);
		printf("here is ! \n");
		dumpbyte(out.cookie,64);
		printf("nick: %s@@@\n",out.nick);
		printf("result: %d@@@\n",out.result);
		
		//3)������ ����ü���� ��Ŷ�� ����
		this->packetFromLogin(out_memory,out);
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
		printf("id: %s@@@@\n",in.ID);
		printf("passwd: %s@@@@\n",in.pass);
		printf("nick: %s@@@@\n",in.nick);

		//2)������ ����ü�� �� ��⿡ ����
		member_manager->Signup(in,out);
		printf("result: %d@@@@\n",out.result);
		//3)������ ����ü���� ��Ŷ�� ����
		this->packetFromSignup(out_memory,out);
		//4)����Ͽ��� ��Ŷ ����
		break;
		}
	case LOGOUT://�α׾ƿ�
		{
		IN_Logout in;	//�α��� ��û ����ü
		OUT_Logout out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToLogout(in,in_memory);

		member_manager->Logout(in,out);
		
		printf("result: %d@@@\n",out.result);
		
		this->packetFromLogout(out_memory,out);
		break;
		}
	case LEAVE://ȸ��Ż��
		{
		IN_Leave in;	//�α��� ��û ����ü
		OUT_Leave out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToLeave(in,in_memory);
		
		member_manager->Leave(in,out);
		
		printf("result: %d@@@\n",out.result);
		
		this->packetFromLeave(out_memory,out);
		break;
		}
	case SEARCH://���� �˻�
		{
		IN_Search in;	//�α��� ��û ����ü
		OUT_Search out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToSearch(in,in_memory);
		
		break;
		}
	case MORE://������
		{
		IN_More in;	//�α��� ��û ����ü
		OUT_More out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToMore(in,in_memory);
		
		break;
		}
	case WRITE_COMMENT://�ǰ߳����
		{
		IN_Write_comment in;	//�α��� ��û ����ü
		OUT_Write_comment out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToWriteComment(in,in_memory);
		
		break;
		}
	case MODIFY_COMMENT://�ǰ߼���
		{
		IN_Modify_comment in;	//�α��� ��û ����ü
		OUT_Modify_comment out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToModifyComment(in,in_memory);
		
		break;
		}
	case DELETE_COMMENT://�ǰ߻���
		{
		IN_Delete_comment in;	//�α��� ��û ����ü
		OUT_Delete_comment out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToDeleteComment(in,in_memory);

		break;
		}
	case LIKE://���ƿ�
		{
		IN_Like in;	//�α��� ��û ����ü
		OUT_Like out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToLike(in,in_memory);
		
		break;
		}
	case REPORT://���� ����
		{
		IN_Report in;	//�α��� ��û ����ü
		OUT_Report out;	//�α��� ���� ����ü
		memset(&in,0,sizeof(in));
		memset(&out,0,sizeof(out));
		packetToReport(in,in_memory);
		
		break;
		}

	}
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