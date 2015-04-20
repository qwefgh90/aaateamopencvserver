
#include "stdafx.h"

#if !defined(AFX_PACKET_H__643EAE08_3798_43A6_B1C9_9BC6AAF6D0B4__INCLUDED_)
#define AFX_PACKET_H__643EAE08_3798_43A6_B1C9_9BC6AAF6D0B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define	MAX_CONTENT 60
#define SM_CONNECT		1000
#define SM_USERID		1001
#define SM_ADDURL		1002
#define SM_GETPWD		1003
#define SM_USEEND		1004
#define SM_IDCHECK		1005
#pragma pack(1)

//데이터 그램의 구조체
typedef struct _protodata
{
	char szContent[MAX_CONTENT]; // 실메시지
	int nType; // 메시지의 종류
	int grade; // 유해싸이트 등급
} PROTODATA, *P_PROTODATA;

#pragma pack()


class CPacket  
{
	PROTODATA m_Data;
	int m_Index;
public:
	static CPacket MakePacket(P_PROTODATA data,int index)
	{
		CPacket tmp(data,index);
		return tmp;
	}
	PROTODATA &GetProtoData(){	return m_Data;	}
	int	GetIndex(){	return m_Index;	}
	

	CPacket(P_PROTODATA data,int index);
	CPacket();
	virtual ~CPacket();

};

#endif // !defined(AFX_PACKET_H__643EAE08_3798_43A6_B1C9_9BC6AAF6D0B4__INCLUDED_)
