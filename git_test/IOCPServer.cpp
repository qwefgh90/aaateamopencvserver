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
//서버소켓을 생성 및 바인드,리슨
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


//필요한 소켓이나 버퍼등을 초기화하고 IOCP에 소켓을 등록한다.
PSOCKET_DATA CIocpSrv::UpdataCompletionPort(SOCKET sdAccept,char* pClientAddr)
{
	int index;	//이 변수에 인덱스를 부여함

	if ( !m_SocketIndexQ.Get(&index) )	//큐에있는 여러개의 인덱스중 하나를 얻어온다.(총커넥션수 < 현재 커넥션수 False)
	{	//큐가 비었을 경우
		closesocket(sdAccept);
		return 0;
	}
	
	//소켓 데이터구조체를 사용함 (m_pSockets[index])
	memset(&m_pSockets[index],0,sizeof( m_pSockets[index] ) );	
	
	m_pSockets[index].Socket = sdAccept;				//소켓핸들
	m_pSockets[index].index = index;					//객체 자신의 위치
	m_pSockets[index].Status = true;					//객체 사용 중
	strcpy(m_pSockets[index].IpAddress,pClientAddr);	//아이피주소 복사

	m_pSockets[index].IOData[0].IOOperation = IoRead;				//Overlaped
	m_pSockets[index].IOData[0].nTotalBytes = 0;					//구조화된 패킷의 크기로 TotalBytes의 크기를 결정함
	m_pSockets[index].IOData[0].nCurrentBytes	=0;					//현재 크기
	m_pSockets[index].IOData[0].completeBuf = NULL;					//전체 데이터그램을 임시로 담는 버퍼
	m_pSockets[index].IOData[0].wsabuf.buf = m_pSockets[index].IOData[0].Buf;	//데이터가 담기는 버퍼의 주소 WSARecv, WSASend와 같이 사용됨
	m_pSockets[index].IOData[0].wsabuf.len = BUFSIZE;							//데이터가 담기는 버퍼의 크기 WSARecv, WSASend와 같이 사용됨
	m_pSockets[index].IOData[0].s =sdAccept;

	m_pSockets[index].IOData[1].IOOperation = IoWrite;				
	m_pSockets[index].IOData[1].nTotalBytes = 0;
	m_pSockets[index].IOData[1].nCurrentBytes	=0;
	m_pSockets[index].IOData[1].completeBuf = NULL;	//보낼때 구성된 데이터가 담길 버퍼
	m_pSockets[index].IOData[1].wsabuf.buf = NULL;	//보낼땐 데이터 구성이 완료 된 후 결정
	m_pSockets[index].IOData[1].wsabuf.len = 0;		//보낼땐 데이터 구성이 완료 된 후 결정
	m_pSockets[index].IOData[1].s =sdAccept;
	//네번째 매개변수로 소켓핸들을 포함하는 객체를 넘긴다. I/O에 대한 완료 통보를 받을때 식별한다. 
	m_hIOCP = CreateIoCompletionPort((HANDLE)sdAccept, m_hIOCP, (DWORD)&m_pSockets[index], 0);
	
	if (NULL == m_hIOCP)	//IOCP 생성 실패
	{
		//	해당 인덱스에 존재하는 소켓데이타를 초기화하자..
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
		//소켓 종료시 아직 송수신이 완료되지 않은 패킷들의 처리 방안 지정
		LINGER  lingerStruct;  
			
		lingerStruct.l_onoff = 1; // 1은 On 2는 off
		lingerStruct.l_linger = 0;  // 시간을 지정한다. 0일시에는 완료안된 패킷들을 버리고 바로 종료.
		setsockopt(pSD->Socket, SOL_SOCKET, SO_LINGER,	(char *)&lingerStruct, sizeof(lingerStruct) );
		closesocket(pSD->Socket);
		pSD->Socket=NULL;
	}
}
void CIocpSrv::ReleaseData(PSOCKET_DATA pSD)
{ 
	printf("ReleaseData()\n");
	pSD->IOData[0].nCurrentBytes = 0;//현재 받은 크기 초기화
	pSD->IOData[1].nCurrentBytes = 0;
	pSD->IOData[0].nTotalBytes = 0;
	pSD->IOData[1].nTotalBytes = 0;
	memset(pSD->IOData[0].Buf,0,sizeof(pSD->IOData[0].Buf));						//버퍼 초기화
	memset(pSD->IOData[1].Buf,0,sizeof(pSD->IOData[1].Buf));
	if(pSD->IOData[0].completeBuf!=NULL)	//버퍼 자원해제
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