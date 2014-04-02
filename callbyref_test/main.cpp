#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <vector>
using namespace std;

void dumpbyte(u_char* buf,u_int len);
typedef struct _Memory{
	u_char* buf;
	u_int len;
}Memory;
string spliter("\r\n"); 
string spliter_end("\r\n\r\n"); 
typedef struct _IN_Search{
	unsigned char cookie[64];
	u_char filter;
	float latitude;	//����
	float longitude;//�浵
	Memory image;	//image
}IN_Search;
typedef struct _IN_Login{
	char ID[20];
	char pass[20];
}IN_Login;

vector<string> split(string str, string sep){
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

bool packetToLogin(__out IN_Login& out, __in Memory& memory)
{
	bool result = false;

	string str((char*)(memory.buf));
	vector<string> v= split(str,spliter);
	printf("%d\n",v.size());
	strcpy(out.ID,v[0].c_str());
	strcpy(out.pass,v[1].c_str());
	printf("0:%ssplit\n",v[0].c_str());
	printf("1:%ssplit\n",v[1].c_str());
	//printf("2:%ssplit\n",v[2].c_str());
	result = true;

	return result;
}

const u_int SIZE_BUTIMAGE = 64+4+1+1+4+4;

bool packetToSearch(__out IN_Search& out, __in Memory& memory )
{
	bool result = false;
	u_char* cookie = memory.buf+5;		//��Ű
	u_char* filter = cookie+64;			//����
	u_char* latitude = filter+1;		//����
	u_char* longitude = latitude+4;		//�浵
	u_char* image_buf = longitude+4;	//image
	u_int	image_size = *((u_int*)memory.buf)-SIZE_BUTIMAGE;	//image size
	memcpy(out.cookie,cookie,64);
	out.filter = *filter;
	memcpy(&(out.latitude),latitude,4);
	memcpy(&(out.longitude),longitude,4);


	out.image.buf = new byte[image_size];
	out.image.len = image_size;
	memcpy(out.image.buf,image_buf,image_size);

	result = true;
	return result;
}

typedef struct _IN_Report{
	unsigned char cookie[64];
	u_char filter;
	float latitude;	//����
	float longitude;//�浵
	char comment[400+1];
	u_char comment_score;
	Memory image;	//image
	u_char id[31];
}IN_Report;
const u_int SEARCH_SIZE_BUTIMAGE_OPINION = (4+2)+ (1+2) + (64+2)+(1+2)+(4+2)+(4+2) +4;
//�����ͱ��� + Ÿ�� + ��Ű + ���� + ���� + �浵 + ���� ������ + ������ ������ + (�ǰ�+2/����+2/�̹��� ����) 
bool packetToReport(__out IN_Report& out, __in Memory& memory )
{
	bool result = false;
	string str;
	vector<string> v;

	u_char* cookie = memory.buf+5 +2+2;
	u_char* filter = cookie+64 +2;
	float*	latitude =((float*)(filter+1  +2));
	float*	longitude= (float*)(((u_char*)latitude)+4 +2);
	u_char* opinion = ((u_char*)longitude)+4 +2;

	u_int	opinion_size = 0;
	u_char* opinion_socre = NULL;
	u_char* image = NULL;
	u_int	image_size = 0;

	memcpy(out.cookie,cookie,64);
	memcpy(&(out.filter),filter,1);				//����
	memcpy(&(out.latitude),latitude,4);			//����
	memcpy(&(out.longitude),longitude,4);		//�浵

	char* end_ptr =NULL;
	end_ptr = strstr((char*)opinion,"\r\n");	//\r\n������
	if(end_ptr!=NULL)
	{
		end_ptr[0]=NULL;
		end_ptr[1]=NULL;
		opinion_size = ((u_char*)end_ptr-opinion);	//�ǰ� ����

		opinion_socre = ((u_char*)(end_ptr+2));		//����
		image = opinion_socre+1 +2;					//�̹��� ������
		//�������� ���̴� little endian ���� ����Ǿ� �־����.
		image_size = *((u_int*)memory.buf) - (SEARCH_SIZE_BUTIMAGE_OPINION + (opinion_size +2   +1 +2));//�ǰ�+���� ����
	}else
	{
		goto END;
	}

	str = string((char*)opinion);
	v = split(str,spliter);	//spliter�� �������� �ڸ��� �������� NULL�� �־�� ������ ���� ���� ����, ���Ƿ� NULL�� ä����
	strcpy(out.comment,v[0].c_str());		//�� ���� (���� �������ݿ��� �� ���븸 0�� �ε����� ������ �ȴ�)
	out.comment_score = *opinion_socre;		//�� ����

	//�̹��� ����
	out.image.buf=new byte[image_size];		//���� ó�� �� �Ҵ����� �ʿ�
	memcpy(out.image.buf,image,image_size);
	out.image.len = image_size;
	
	result = true;
	END:
	return result;

}

typedef struct _stu{
	int num;
	int math;
	int lang;
}stu;

void setnum(stu &s)
{
	s.num = 1;
}
void setmath(stu &s)
{
	s.math = 1;	
	printf("%d %d %d\n",s.num,s.math,s.lang);
	setnum(s);
}
void main4();
void main3();
void main2();
void main()
{
	main2();
	
}

void dumpbyte(u_char* buf,u_int len)
{
	int i=0;
	for (i=0 ; i<len;i++)
		printf("0x%x ",buf[i]);
	printf("\n");
}
//Report ����ü ���� �Լ� ����
void main2()
{
	IN_Report report;
	Memory memory;

	u_char type = 1;
	
	u_char cookie[64] = {1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,
		1,2,3,4,5,6,7,8,9,0,1,2,3,4};
	
	u_char filter = 0x4;
	
	float la = 365.4;
	
	float lo = 200.1;
	
	char opi[400] = "this is opinion !";

	u_int opi_size = strlen(opi);

	char opi_score = 10;
	
	u_char image[140] = {1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,
		1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,
		1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};
	
	u_int image_size =140;

	u_int data_len = SEARCH_SIZE_BUTIMAGE_OPINION + (opi_size+2) + (1+2) + image_size ;//�ǰ�/����/�̹��� 
	
	printf("data_len : %d\n",data_len);

	u_char* bytes = new u_char[data_len];

	memory.buf = bytes;				//��Ŷ
	memory.len = data_len;			//��Ŷ�� ����

	memcpy(bytes,&(data_len),4);//����
	memcpy(bytes+4,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2,&type ,1);//Ÿ��
	memcpy(bytes+4+2+1,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2+1+2,cookie ,64);//��Ű
	memcpy(bytes+4+2+1+2+64,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2+1+2+64+2,&filter ,1);//����
	memcpy(bytes+4+2+1+2+64+2+1,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2+1+2+64+2+1+2,&la ,4);//����
	memcpy(bytes+4+2+1+2+64+2+1+2+4,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2,&lo ,4);//�浵
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2+4,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2+4+2,&opi ,opi_size);//�ǰ�
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2+4+2+opi_size,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2+4+2+opi_size+2,&opi_score ,1);//�ǰ�����
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2+4+2+opi_size+2+1,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2+4+2+opi_size+2+1+2,&image ,image_size);//�̹���
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2+4+2+opi_size+2+1+2+image_size,spliter.c_str(),2);//\r\n
	memcpy(bytes+4+2+1+2+64+2+1+2+4+2+4+2+opi_size+2+1+2+image_size+2,spliter.c_str(),2);//\r\n

	printf("data_len2 %d\n",4+2+1+2+64+2+1+2+4+2+4+2+opi_size+2+1+2+image_size+2+2);
	packetToReport(report,memory);

	printf("-----------parse data list------------------------------------\n");
	dumpbyte(report.cookie,64);
	printf("filter %d \n",report.filter);
	printf("la : %f, lo : %f \n",report.latitude,report.longitude);
	printf("comment : %s \n",report.comment);
	dumpbyte(report.image.buf,report.image.len);
}

void main4()
{
}
//���� �� �α��� ��� ����?
void main3()
{
}