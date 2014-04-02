// IocpSrv.h: interface for the CIocpSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#if !defined(AFX_IOCPSRV_H__20154FF6_2F6C_4B80_BD8F_0EA70F06D32C__INCLUDED_)
#define AFX_IOCPSRV_H__20154FF6_2F6C_4B80_BD8F_0EA70F06D32C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IndexQ.h"


#ifndef __BUF__

#define	BUFSIZE	1000
#endif
typedef enum _IO_OPERATION { 
	IoAccept, 
	IoRead, 
	IoWrite 
} IO_OPERATION, *PIO_OPERATION;

typedef struct io_data	{
	WSAOVERLAPPED	Overlapped;			// IO필수 구조체
	char			Buf[BUFSIZE];		// IO에 사용할 버퍼
	u_char*			completeBuf;		// IO 완료 후 생긴 버퍼를 가르치는 포인터
	WSABUF			wsabuf;				// IO필수 구조체
	DWORD			nTotalBytes;		// 전송된 총 바이트
	DWORD			nCurrentBytes;		// 현 전송된 바이트
	IO_OPERATION	IOOperation;		// 버퍼 타입
	SOCKET			s;					// 소켓(객체)
}IO_DATA, *PIO_DATA;

//I/O완료 통보를 받을때 이 구조체로 식별한다.
typedef struct socket_data	{
	SOCKET			Socket;				// 소켓(객체)
	int				index;				// 자신의 인덱스
	IO_DATA			IOData[2];			// [0]recvIO 구조체 [1] sendIO 구조체
	bool			Status;				// 객체 상태
	char			IpAddress[16];		// 클라이언트 아이피 주소
}SOCKET_DATA,* PSOCKET_DATA;

class CIocpSrv  
{
	int m_Port;				//서버 포트
	int m_MaxConnection;	//서버 연결 수
	int m_Status;

	SOCKET m_ListenSock;				//대기 서버 소켓
	PSOCKET_DATA m_pSockets;
	HANDLE m_hIOCP;						//IOCP핸들	//생산자에서 초기화
	//생성자 (Construction)
	CIocpSrv(int port = 9001,int maxconnection=2000) : m_Port(port),m_MaxConnection(maxconnection)
	{
		
		m_Status = true;
		// 클라이언트가 수시로 들어왔다 나갔다 하기때문에
		// 잦은 파일I/O를 를 방지하고자 미리 생성해 놓는다.
		m_pSockets = new SOCKET_DATA[maxconnection];		//최대 Connection개수 만큼 구조체 생성
		int i;

		for (i=0; i<maxconnection; i++) 
		{
			m_pSockets[i].Status = false;
			m_SocketIndexQ.Put(i);
		}

		StartUp();
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
		
		if (m_hIOCP == NULL)		//객체 생성 실패 생성자 실패
			delete [] m_pSockets;	//소켓 데이터 구조체 반환
	}
public:
	
	CIndexQ		m_SocketIndexQ;    // 인덱스큐

	SOCKET GetLSocket()	{ return m_ListenSock;	}
	HANDLE GetHIocp()	{ return m_hIOCP;		}
	int &GetStatus()	{ return m_Status;		}
	PSOCKET_DATA GetSockets(){	return m_pSockets;	}

	bool CreateLitenSocket();
	void StartThreadRoutine(LPTHREAD_START_ROUTINE lpStartAddress,int num);	//num개수 만큼 스레드 생성
	PSOCKET_DATA UpdataCompletionPort(SOCKET sdAccept,char* pClientAddr);
	bool StartUp();
	void CloseClient(PSOCKET_DATA pSD);
	void ReleaseData(PSOCKET_DATA pSD);	//더이상 사용하지 않는 소켓의 IO_DATA의 동적할당 해제
	//서버 생성
	static CIocpSrv *CreateInstance(unsigned int port=8888,int maxconnection=2000)
	{
		return (new CIocpSrv(port,maxconnection));
	}
	CIocpSrv::~CIocpSrv();

};

#endif // !defined(AFX_IOCPSRV_H__20154FF6_2F6C_4B80_BD8F_0EA70F06D32C__INCLUDED_)
