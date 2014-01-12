// git_test.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "IOCPServer.h"	//서버 동작에 관련된 헤더
DWORD WINAPI WorkerThread ( LPVOID WorkerThreadContext );
DWORD WINAPI AcceptThread( LPVOID DbGatewayThreadContext );
DWORD WINAPI ProcessThread(LPVOID recv_buf);
CIocpSrv *g_pSrv;	//서버 객체
int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA stWSAData = {0};
	int nWSAError = WSAStartup( MAKEWORD( 2, 2 ), &stWSAData );
	if( nWSAError != 0 )
	{
		return nWSAError;
	}
	g_pSrv = CIocpSrv::CreateInstance(8888,5);
	if ( !g_pSrv->CreateLitenSocket() )			//Win소켓 -> bind() listen() 초기화
		printf("bind(), listen()실패\n");
	g_pSrv->StartThreadRoutine(WorkerThread,1);
	g_pSrv->StartThreadRoutine(AcceptThread,1);
	//1)서버 생성
	//2)서버 IOCP큐 쓰레드 생성
	//3)서버 동작
	//4)큐에 데이터가 찰 경우 데이터를 영상처리 쓰레드에 넘긴다
	//5)영상처리 쓰레드가 처리를 완료하고 Write를 한다.
	//6)클라이언트 측에서 연결을 끊기를 원할경우나 커넥션이 끊어졌을경우도 고려한다.
	while(1);
	return 0;
}

//  특정 서버 객체에 대한 대기큐 
//  이전의 요청한 IO의 완료 결과를 통보 받고 그 결과에 따라 어떤 IO를 더 요청하거나
//  아니면 다른작업을 합니다. 거의 중심적인 IO처리가 이루워지게 됩니다.
DWORD WINAPI WorkerThread ( LPVOID WorkerThreadContext )
{
	CIocpSrv		*pSrv = (CIocpSrv *)WorkerThreadContext;//스레드 전담 서버 객체
	HANDLE			hIOCP = pSrv->GetHIocp();				//IOCP핸들
	BOOL			bSucces = FALSE;			
	int				nRet;			
	LPOVERLAPPED	pOverlapped = NULL;						
	PSOCKET_DATA	pSD		= NULL;							//소켓데이터
	PIO_DATA		pIOD		= NULL;
	DWORD           dwFlags = 0;
	DWORD			dwSendNBytes =0;
	DWORD			dwRecvNBytes =0;
	DWORD			dwIoSize;
	u_char*			pBuffer;


	while (1)
	{	//IO 통보받는 함수. 큐로 이루어 져있으며 이벤트가 발생할떄까지 락상태로 대기한다.
		bSucces = GetQueuedCompletionStatus( hIOCP, &dwIoSize,	(LPDWORD)&pSD, 
			&pOverlapped,  INFINITE );
		if ( !bSucces )
			continue;

		if ( pSD == NULL)
			return (0);

		if ( !bSucces || (bSucces && (0 == dwIoSize)))	//false거나, 성공했지만 데이터가 없을경우
		{
			pSD->Status = false;
			pSrv->CloseClient(pSD);
			pSrv->ReleaseData( pSD );
			pSrv->m_SocketIndexQ.Put( pSD->index );
			continue;
		}

		pIOD = (PIO_DATA)pOverlapped; 

		//통보받은 IO를 구분하고 그에해당하는 처리가 이루워 진다.
		switch (pIOD->IOOperation) { 
		case IoWrite:
			pIOD->nCurrentBytes += dwIoSize;
			dwFlags = 0;
			printf("[Write Data] totalsize : %ubytes , csize : %dbytes , dwloSize : %dbytes\n",pIOD->nTotalBytes,pIOD->nCurrentBytes,dwIoSize);

			if (pIOD->nCurrentBytes < pIOD->nTotalBytes ) // 아직 덜보냈다..
			{ 
				printf("Write more data : %ubytes\n",pIOD->nTotalBytes-pIOD->nCurrentBytes);
				pIOD->wsabuf.buf = pIOD->Buf + pIOD->nCurrentBytes;
				pIOD->wsabuf.len = pIOD->nTotalBytes - pIOD->nCurrentBytes;

				nRet = WSASend( pSD->Socket, &(pIOD->wsabuf), 1, &dwSendNBytes,
					dwFlags, &(pIOD->Overlapped), NULL);

				if (nRet = SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()) )
				{
					pSD->Status = false;
					pSrv->m_SocketIndexQ.Put( pSD->index );
					pSrv->CloseClient( pSD );
					pSrv->ReleaseData( pSD );
				}
				continue;
			}
			else if(pIOD->nCurrentBytes == pIOD->nTotalBytes) // 다보냈을시에 초기화
			{
				printf("All data Send\n");
				pSD->Status = false;
				pSrv->m_SocketIndexQ.Put( pSD->index );
				pSrv->ReleaseData( pSD );
				pSrv->CloseClient( pSD );
			}else
			{		//SEND INVALID DATA
				pSD->Status = false;
				pSrv->m_SocketIndexQ.Put( pSD->index );
				pSrv->CloseClient( pSD );
				pSrv->ReleaseData( pSD );
				printf("Send invalid data!!!\n");
				//에러 발생
			}
			break;
		case IoRead:
			pIOD->nCurrentBytes += dwIoSize;	//현재 받은 데이터
			dwFlags = 0;

			//nTotalBytes값이 0이면 처음 데이터를 받는 경우이다. 이때 버퍼를 하나 생성하여 데이터를 담는다.
			if(pIOD->nTotalBytes==0)
			{
				//binascii.unhexlify('0000000a') in python
				u_int totalBytes = ntohl(*((u_int*)pIOD->Buf));
				pIOD->nTotalBytes = totalBytes;
				pIOD->completeBuf = new u_char[totalBytes];
				memset(pIOD->completeBuf,0,totalBytes);
				printf("New total buffer allocate\n");
			}
			pBuffer = pIOD->completeBuf;	//전체 데이터에 대한 버퍼

			printf("[Read Data] totalsize : %ubytes , csize : %dbytes , dwloSize : %dbytes\n",pIOD->nTotalBytes,pIOD->nCurrentBytes,dwIoSize);

			if (pIOD->nCurrentBytes < pIOD->nTotalBytes ) // 아직 덜받았따..
			{
				memcpy(pBuffer+(pIOD->nCurrentBytes-dwIoSize),pIOD->wsabuf.buf,dwIoSize);	//받은 일부 데이터를 전체 버퍼에 복사
				printf("Wait more data : %ubytes\n",pIOD->nTotalBytes-pIOD->nCurrentBytes);
				nRet = WSARecv( pSD->Socket, &(pIOD->wsabuf), 1, &dwRecvNBytes,
					&dwFlags, &(pIOD->Overlapped), NULL);
				if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError()) )
				{
					pSD->Status = false;
					pSrv->m_SocketIndexQ.Put( pSD->index );
					pSrv->CloseClient( pSD );
					pSrv->ReleaseData( pSD );
				}
			}else if(pIOD->nTotalBytes == pIOD->nCurrentBytes)
			{
				memcpy(pBuffer+(pIOD->nCurrentBytes-dwIoSize),pIOD->wsabuf.buf,dwIoSize);	//받은 일부 데이터를 전체 버퍼에 복사
				printf("All data Received\n");
				printf("bufdata : %s\n",pBuffer+4);
				CloseHandle( CreateThread(NULL, 0, ProcessThread, pSD, 0, 0) );
				//처리 쓰레드 생성
			}else
			{	//RECEIVE INVALID DATA
				//CloseHandle( CreateThread(NULL, 0, ProcessThread, pSD, 0, 0) );

				pSD->Status = false;
				pSrv->m_SocketIndexQ.Put( pSD->index );
				pSrv->CloseClient( pSD );
				pSrv->ReleaseData( pSD );
				printf("Receive invalid data!!!\n");
				//에러 발생
			}

			break;

		} // switch
	} // while
	return 0;
}
// 여러 클라이언트의 요청을 받아서  Completion Port오브젝트에 연결해준다.
DWORD WINAPI AcceptThread( LPVOID DbGatewayThreadContext )
{
	CIocpSrv		*pSrv = (CIocpSrv *)DbGatewayThreadContext;
	HANDLE			hIocp = pSrv->GetHIocp();
	SOCKET			sdAccept = INVALID_SOCKET;
	PSOCKET_DATA	pSD;
	DWORD           dwFlags = 0;
	DWORD			dwSendNBytes =0;
	DWORD			dwRecvNBytes =0;
	int             nRet;
	SOCKADDR_IN		RecvAddr;
	static int		addrlen = sizeof(RecvAddr);  // 송신측 ip주소알아내기
	char*			ClientAddr;

	HWND *phWnd;


	while (1)
	{
		//printf("AcceptStart\n");
		sdAccept = WSAAccept(pSrv->GetLSocket(),(struct sockaddr *)&RecvAddr,
			&addrlen	,NULL,0);

		//printf("NewSocket\n");
		getpeername(sdAccept,(struct sockaddr *)&RecvAddr,&addrlen);

		ClientAddr = inet_ntoa(RecvAddr.sin_addr);	//아이피 주소를 얻어냄

		if (sdAccept == SOCKET_ERROR)
		{
			if ( !pSrv->GetStatus() )				//서버가 종료되었을 때(플래그 값=false)
				break;

			continue;
		}

		//IOCP에 등록된 버퍼구조체 반환
		//SOCKET_DATA	//WSAOVERLAPPED 를 포함한 클래스
		//IO버퍼 포함	//소켓 포함
		pSD = pSrv->UpdataCompletionPort(sdAccept,ClientAddr);	

		if (pSD == NULL)
			break;

		if ( !pSrv->GetStatus() )
			break;

		pSD->Socket = sdAccept;

		//사실 6번째 매개변수는 소켓데이터 구조체의 주소와 마찬가지이다.
		nRet = WSARecv( sdAccept, &(pSD->IOData[0].wsabuf), 1, &dwRecvNBytes, &dwFlags, &(pSD->IOData[0].Overlapped), NULL);

		if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
			pSrv->CloseClient(pSD);
			printf("WSARecv ERROR\n");
		}

		Sleep(0);
	}

	return 0;
}

// DB처리
// 데이터그램을 이용해 처리를 하는 쓰레드 (작업쓰레드 가장 중요)
DWORD WINAPI DbGatewayThread ( LPVOID ScatterThreadContext )
{
	CPacket pack;
	PSOCKET_DATA pSockets;
	DWORD	dwSendNBytes=0;
	DWORD	dwFlags=0;
	int index;

	while (1)
	{
		CIocpSrv::s_PacketQ.Get(pack);//큐에 쌓인패킷을 가져온다.
		pSockets = g_pSrv->GetSockets();
		index = pack.GetIndex();

		WSASend( pSockets[index].Socket, &(pSockets[index].IOData[1].wsabuf), 1, &dwSendNBytes,
			dwFlags, &(pSockets[index].IOData[1].Overlapped), NULL);
		/*
		// type에 따라 처리.
		switch (pack.GetProtoData().nType) { 
		// 클라이언트가 유해싸이트인지 판별요청했을시.
		// 클라이언트에게  PROTODATA구조체형으로 결과값을 send해준다.
		// type 이 0은 유해싸이트가 아니고  1은 유해싸이트이다.
		// contents배열에 내용을 넣어준다.
		case SM_CONNECT: 

		// db에서 ip를 select한다.
		// false를 리턴했다는것은 오류가 있다.
		if( !g_pQuery->HarmfulSearch(pack.GetProtoData().szContent,pack.GetProtoData().grade) )
		{
		pack.GetProtoData().nType = 0;
		if (g_pQuery->GetRet() == SQL_NO_DATA)
		{
		// m_Ret이 SQL_NO_DATA라는것은 유해싸이트가 아니다 
		strcpy(pack.GetProtoData().szContent,"NO_DATA");
		}
		else
		{
		strcpy(pack.GetProtoData().szContent,"ERROR");
		}
		}
		else 
		{
		pack.GetProtoData().nType = 1;
		strcpy(pack.GetProtoData().szContent,"HARMFUL_IP");
		}
		pSockets[index].IOData[1].wsabuf.buf = (char *)&pack.GetProtoData();
		WSASend( pSockets[index].Socket, &(pSockets[index].IOData[1].wsabuf), 1, &dwSendNBytes,
		dwFlags, &(pSockets[index].IOData[1].Overlapped), NULL);

		break;

		case SM_USERID: // 회원이 컴퓨터를 켰다.
		g_pQuery->UserUpdate(pack.GetProtoData().szContent,pSockets[index].IpAddress);
		break;
		case SM_USEEND: // 회원이 컴퓨터를 껏다.
		g_pQuery->UserEnd(pack.GetProtoData().szContent);
		break;

		case SM_ADDURL: // 회원이 유해싸이트를 로컬에 추가하였다.
		g_pQuery->AddHarmfulURL(pack.GetProtoData().szContent);
		break;

		case SM_GETPWD: // 프로그램 무단 상태변경 방지를 위한 회원 확인절차
		if( !g_pQuery->GetPassWord(pack.GetProtoData().szContent) )
		{
		pack.GetProtoData().nType = 0;
		if (g_pQuery->GetRet() == SQL_NO_DATA)
		{
		// 아이디가 존재 하지않습니다.
		strcpy(pack.GetProtoData().szContent,"NO_ID");
		}
		else
		{	// 일반적인 에러.
		strcpy(pack.GetProtoData().szContent,"ERROR");
		}
		}
		else 
		{	// 회원으로 확인됨
		pack.GetProtoData().nType = 1;
		}
		pSockets[index].IOData[1].wsabuf.buf = (char *)&pack.GetProtoData();
		WSASend( pSockets[index].Socket, &(pSockets[index].IOData[1].wsabuf), 1, &dwSendNBytes,
		dwFlags, &(pSockets[index].IOData[1].Overlapped), NULL);
		break;

		case SM_IDCHECK:
		if( !g_pQuery->GetIDCheck(pack.GetProtoData().szContent) )
		{
		pack.GetProtoData().nType = 0;
		if (g_pQuery->GetRet() == SQL_NO_DATA)
		{	// 아이디가 존재 하지않습니다.
		strcpy(pack.GetProtoData().szContent,"NO_ID");
		}
		else
		{	// 일반적인 에러.
		strcpy(pack.GetProtoData().szContent,"ERROR");
		}
		}
		else 
		{	// 아이디 존재
		pack.GetProtoData().nType = 1;
		}
		pSockets[index].IOData[1].wsabuf.buf = (char *)&pack.GetProtoData();
		WSASend( pSockets[index].Socket, &(pSockets[index].IOData[1].wsabuf), 1, &dwSendNBytes,
		dwFlags, &(pSockets[index].IOData[1].Overlapped), NULL);
		break;

		default:
		break;
		}*/
	}
	return 0;
}
// 데이터그램을 이용해 처리를 하는 쓰레드 (자원해제 포함)
DWORD WINAPI ProcessThread(LPVOID recv_buf)
{
	int nRet;
	DWORD dwFlags = 0;
	DWORD dwSendNBytes =0;
	PSOCKET_DATA pSD = (PSOCKET_DATA)recv_buf;
	char temp[256]={0,};
	sprintf(temp,"You send data : %dbytes\n",pSD->IOData[0].nCurrentBytes);
	int length = strlen(temp)+1;
	pSD->IOData[1].completeBuf = new byte[length];

	strcpy((char*)(pSD->IOData[1].completeBuf),temp);

	pSD->IOData[1].nTotalBytes = length;
	pSD->IOData[1].wsabuf.buf = (char*)pSD->IOData[1].completeBuf;
	pSD->IOData[1].wsabuf.len=length;

	nRet = WSASend( pSD->Socket, &(pSD->IOData[1].wsabuf), 1, &dwSendNBytes,
		dwFlags, &(pSD->IOData[1].Overlapped), NULL);
	//버퍼 초기화
	if (nRet = SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()) )
	{
		pSD->Status = false;
		pSrv->m_SocketIndexQ.Put( pSD->index );
		pSrv->CloseClient( pSD );
		pSrv->ReleaseData( pSD );
	}
	return 0;
}