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
	WSAOVERLAPPED	Overlapped;			// IO�ʼ� ����ü
	char			Buf[BUFSIZE];		// IO�� ����� ����
	u_char*			completeBuf;		// IO �Ϸ� �� ���� ���۸� ����ġ�� ������
	WSABUF			wsabuf;				// IO�ʼ� ����ü
	DWORD			nTotalBytes;		// ���۵� �� ����Ʈ
	DWORD			nCurrentBytes;		// �� ���۵� ����Ʈ
	IO_OPERATION	IOOperation;		// ���� Ÿ��
	SOCKET			s;					// ����(��ü)
}IO_DATA, *PIO_DATA;

//I/O�Ϸ� �뺸�� ������ �� ����ü�� �ĺ��Ѵ�.
typedef struct socket_data	{
	SOCKET			Socket;				// ����(��ü)
	int				index;				// �ڽ��� �ε���
	IO_DATA			IOData[2];			// [0]recvIO ����ü [1] sendIO ����ü
	bool			Status;				// ��ü ����
	char			IpAddress[16];		// Ŭ���̾�Ʈ ������ �ּ�
}SOCKET_DATA,* PSOCKET_DATA;

class CIocpSrv  
{
	int m_Port;				//���� ��Ʈ
	int m_MaxConnection;	//���� ���� ��
	int m_Status;

	SOCKET m_ListenSock;				//��� ���� ����
	PSOCKET_DATA m_pSockets;
	HANDLE m_hIOCP;						//IOCP�ڵ�	//�����ڿ��� �ʱ�ȭ
	//������ (Construction)
	CIocpSrv(int port = 9001,int maxconnection=2000) : m_Port(port),m_MaxConnection(maxconnection)
	{
		
		m_Status = true;
		// Ŭ���̾�Ʈ�� ���÷� ���Դ� ������ �ϱ⶧����
		// ���� ����I/O�� �� �����ϰ��� �̸� ������ ���´�.
		m_pSockets = new SOCKET_DATA[maxconnection];		//�ִ� Connection���� ��ŭ ����ü ����
		int i;

		for (i=0; i<maxconnection; i++) 
		{
			m_pSockets[i].Status = false;
			m_SocketIndexQ.Put(i);
		}

		StartUp();
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
		
		if (m_hIOCP == NULL)		//��ü ���� ���� ������ ����
			delete [] m_pSockets;	//���� ������ ����ü ��ȯ
	}
public:
	
	CIndexQ		m_SocketIndexQ;    // �ε���ť

	SOCKET GetLSocket()	{ return m_ListenSock;	}
	HANDLE GetHIocp()	{ return m_hIOCP;		}
	int &GetStatus()	{ return m_Status;		}
	PSOCKET_DATA GetSockets(){	return m_pSockets;	}

	bool CreateLitenSocket();
	void StartThreadRoutine(LPTHREAD_START_ROUTINE lpStartAddress,int num);	//num���� ��ŭ ������ ����
	PSOCKET_DATA UpdataCompletionPort(SOCKET sdAccept,char* pClientAddr);
	bool StartUp();
	void CloseClient(PSOCKET_DATA pSD);
	void ReleaseData(PSOCKET_DATA pSD);	//���̻� ������� �ʴ� ������ IO_DATA�� �����Ҵ� ����
	//���� ����
	static CIocpSrv *CreateInstance(unsigned int port=8888,int maxconnection=2000)
	{
		return (new CIocpSrv(port,maxconnection));
	}
	CIocpSrv::~CIocpSrv();

};

#endif // !defined(AFX_IOCPSRV_H__20154FF6_2F6C_4B80_BD8F_0EA70F06D32C__INCLUDED_)
