#include "stdafx.h"
#include "IOCPServer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


bool CIocpSrv::StartUp()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2), &wsa)  != 0)
		return false;

	return true;
}
CIocpSrv::~CIocpSrv()
{
	int		index;
	
	while( m_SocketIndexQ.Get(&index) )
	{
		if(m_pSockets[index].Status)
			CloseClient(&m_pSockets[index]);
	}
	delete [] m_pSockets;
	m_Status = false;
	closesocket(m_ListenSock);
	WSACleanup();
}
void CIocpSrv::StartThreadRoutine(LPTHREAD_START_ROUTINE lpStartAddress,int num)
{
	int i;

	for(i=0; i< num; i++)
		CloseHandle( CreateThread(NULL, 0, lpStartAddress, this, 0, 0) );
}
//���������� ���� �� ���ε�,����
bool CIocpSrv::CreateLitenSocket()
{
	SOCKADDR_IN serveraddr;
	int retval;
	
	m_ListenSock = WSASocket(AF_INET, SOCK_STREAM, NULL, NULL, 0,WSA_FLAG_OVERLAPPED);
	if( m_ListenSock == INVALID_SOCKET ) 
		return false;

	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(m_Port);
	serveraddr.sin_family = AF_INET;
	
	retval = bind( m_ListenSock,(SOCKADDR *)&serveraddr, sizeof(serveraddr) );
	if (retval ==  SOCKET_ERROR)
		return false;
	
	retval = listen( m_ListenSock, 15);
	if (retval == SOCKET_ERROR)
		return false;

	m_Status = true;
	
	return true;
}


//�ʿ��� �����̳� ���۵��� �ʱ�ȭ�ϰ� IOCP�� ������ ����Ѵ�.
PSOCKET_DATA CIocpSrv::UpdataCompletionPort(SOCKET sdAccept,char* pClientAddr)
{
	int index;	//�� ������ �ε����� �ο���

	if ( !m_SocketIndexQ.Get(&index) )	//ť���ִ� �������� �ε����� �ϳ��� ���´�.(��Ŀ�ؼǼ� < ���� Ŀ�ؼǼ� False)
	{	//ť�� ����� ���
		closesocket(sdAccept);
		return 0;
	}
	
	//���� �����ͱ���ü�� ����� (m_pSockets[index])
	memset(&m_pSockets[index],0,sizeof( m_pSockets[index] ) );	
	
	m_pSockets[index].Socket = sdAccept;				//�����ڵ�
	m_pSockets[index].index = index;					//��ü �ڽ��� ��ġ
	m_pSockets[index].Status = true;					//��ü ��� ��
	strcpy(m_pSockets[index].IpAddress,pClientAddr);	//�������ּ� ����

	m_pSockets[index].IOData[0].IOOperation = IoRead;				//Overlaped
	m_pSockets[index].IOData[0].nTotalBytes = 0;					//����ȭ�� ��Ŷ�� ũ��� TotalBytes�� ũ�⸦ ������
	m_pSockets[index].IOData[0].nCurrentBytes	=0;					//���� ũ��
	m_pSockets[index].IOData[0].completeBuf = NULL;					//��ü �����ͱ׷��� �ӽ÷� ��� ����
	m_pSockets[index].IOData[0].wsabuf.buf = m_pSockets[index].IOData[0].Buf;	//�����Ͱ� ���� ������ �ּ� WSARecv, WSASend�� ���� ����
	m_pSockets[index].IOData[0].wsabuf.len = BUFSIZE;							//�����Ͱ� ���� ������ ũ�� WSARecv, WSASend�� ���� ����
	m_pSockets[index].IOData[0].s =sdAccept;

	m_pSockets[index].IOData[1].IOOperation = IoWrite;				
	m_pSockets[index].IOData[1].nTotalBytes = 0;
	m_pSockets[index].IOData[1].nCurrentBytes	=0;
	m_pSockets[index].IOData[1].completeBuf = NULL;	//������ ������ �����Ͱ� ��� ����
	m_pSockets[index].IOData[1].wsabuf.buf = NULL;	//������ ������ ������ �Ϸ� �� �� ����
	m_pSockets[index].IOData[1].wsabuf.len = 0;		//������ ������ ������ �Ϸ� �� �� ����
	m_pSockets[index].IOData[1].s =sdAccept;
	//�׹�° �Ű������� �����ڵ��� �����ϴ� ��ü�� �ѱ��. I/O�� ���� �Ϸ� �뺸�� ������ �ĺ��Ѵ�. 
	m_hIOCP = CreateIoCompletionPort((HANDLE)sdAccept, m_hIOCP, (DWORD)&m_pSockets[index], 0);
	
	if (NULL == m_hIOCP)	//IOCP ���� ����
	{
		//	�ش� �ε����� �����ϴ� ���ϵ���Ÿ�� �ʱ�ȭ����..
		closesocket(sdAccept);
		m_pSockets[index].Status = false;
		m_SocketIndexQ.Put(index);
		return 0;
	}
	return &m_pSockets[index];
}

void CIocpSrv::CloseClient(PSOCKET_DATA pSD)
{ 
	printf("CloseClient!\n");
	if ( pSD )
	{
		//���� ����� ���� �ۼ����� �Ϸ���� ���� ��Ŷ���� ó�� ��� ����
		LINGER  lingerStruct;  
			
		lingerStruct.l_onoff = 1; // 1�� On 2�� off
		lingerStruct.l_linger = 0;  // �ð��� �����Ѵ�. 0�Ͻÿ��� �Ϸ�ȵ� ��Ŷ���� ������ �ٷ� ����.
		setsockopt(pSD->Socket, SOL_SOCKET, SO_LINGER,	(char *)&lingerStruct, sizeof(lingerStruct) );
		closesocket(pSD->Socket);
		pSD->Socket=NULL;
	}
}
void CIocpSrv::ReleaseData(PSOCKET_DATA pSD)
{ 
	printf("ReleaseData()\n");
	pSD->IOData[0].nCurrentBytes = 0;//���� ���� ũ�� �ʱ�ȭ
	pSD->IOData[1].nCurrentBytes = 0;
	pSD->IOData[0].nTotalBytes = 0;
	pSD->IOData[1].nTotalBytes = 0;
	memset(pSD->IOData[0].Buf,0,sizeof(pSD->IOData[0].Buf));						//���� �ʱ�ȭ
	memset(pSD->IOData[1].Buf,0,sizeof(pSD->IOData[1].Buf));
	if(pSD->IOData[0].completeBuf!=NULL)	//���� �ڿ�����
	{
		u_char* temp=pSD->IOData[0].completeBuf;
		delete[] temp;
		pSD->IOData[0].completeBuf=NULL;
	}
	if(pSD->IOData[1].completeBuf!=NULL)
	{
		u_char* temp=pSD->IOData[1].completeBuf;
		delete[] temp;
		pSD->IOData[1].completeBuf=NULL;

		pSD->IOData[1].wsabuf.buf=NULL;
		pSD->IOData[1].wsabuf.len=0;
	}
	
}