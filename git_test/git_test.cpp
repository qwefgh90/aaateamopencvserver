// git_test.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "IOCPServer.h"	//���� ���ۿ� ���õ� ���
DWORD WINAPI WorkerThread ( LPVOID WorkerThreadContext );
DWORD WINAPI AcceptThread( LPVOID DbGatewayThreadContext );
DWORD WINAPI ProcessThread(LPVOID recv_buf);
CIocpSrv *g_pSrv;	//���� ��ü
int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA stWSAData = {0};
	int nWSAError = WSAStartup( MAKEWORD( 2, 2 ), &stWSAData );
	if( nWSAError != 0 )
	{
		return nWSAError;
	}
	g_pSrv = CIocpSrv::CreateInstance(8888,5);
	if ( !g_pSrv->CreateLitenSocket() )			//Win���� -> bind() listen() �ʱ�ȭ
		printf("bind(), listen()����\n");
	g_pSrv->StartThreadRoutine(WorkerThread,1);
	g_pSrv->StartThreadRoutine(AcceptThread,1);
	//1)���� ����
	//2)���� IOCPť ������ ����
	//3)���� ����
	//4)ť�� �����Ͱ� �� ��� �����͸� ����ó�� �����忡 �ѱ��
	//5)����ó�� �����尡 ó���� �Ϸ��ϰ� Write�� �Ѵ�.
	//6)Ŭ���̾�Ʈ ������ ������ ���⸦ ���Ұ�쳪 Ŀ�ؼ��� ����������쵵 ����Ѵ�.
	while(1);
	return 0;
}

//  Ư�� ���� ��ü�� ���� ���ť 
//  ������ ��û�� IO�� �Ϸ� ����� �뺸 �ް� �� ����� ���� � IO�� �� ��û�ϰų�
//  �ƴϸ� �ٸ��۾��� �մϴ�. ���� �߽����� IOó���� �̷������ �˴ϴ�.
DWORD WINAPI WorkerThread ( LPVOID WorkerThreadContext )
{
	CIocpSrv		*pSrv = (CIocpSrv *)WorkerThreadContext;//������ ���� ���� ��ü
	HANDLE			hIOCP = pSrv->GetHIocp();				//IOCP�ڵ�
	BOOL			bSucces = FALSE;			
	int				nRet;			
	LPOVERLAPPED	pOverlapped = NULL;						
	PSOCKET_DATA	pSD		= NULL;							//���ϵ�����
	PIO_DATA		pIOD		= NULL;
	DWORD           dwFlags = 0;
	DWORD			dwSendNBytes =0;
	DWORD			dwRecvNBytes =0;
	DWORD			dwIoSize;
	u_char*			pBuffer;


	while (1)
	{	//IO �뺸�޴� �Լ�. ť�� �̷�� �������� �̺�Ʈ�� �߻��ҋ����� �����·� ����Ѵ�.
		bSucces = GetQueuedCompletionStatus( hIOCP, &dwIoSize,	(LPDWORD)&pSD, 
			&pOverlapped,  INFINITE );
		if ( !bSucces )
			continue;

		if ( pSD == NULL)
			return (0);

		if ( !bSucces || (bSucces && (0 == dwIoSize)))	//false�ų�, ���������� �����Ͱ� �������
		{
			pSD->Status = false;
			pSrv->CloseClient(pSD);
			pSrv->ReleaseData( pSD );
			pSrv->m_SocketIndexQ.Put( pSD->index );
			continue;
		}

		pIOD = (PIO_DATA)pOverlapped; 

		//�뺸���� IO�� �����ϰ� �׿��ش��ϴ� ó���� �̷�� ����.
		switch (pIOD->IOOperation) { 
		case IoWrite:
			pIOD->nCurrentBytes += dwIoSize;
			dwFlags = 0;
			printf("[Write Data] totalsize : %ubytes , csize : %dbytes , dwloSize : %dbytes\n",pIOD->nTotalBytes,pIOD->nCurrentBytes,dwIoSize);

			if (pIOD->nCurrentBytes < pIOD->nTotalBytes ) // ���� �����´�..
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
			else if(pIOD->nCurrentBytes == pIOD->nTotalBytes) // �ٺ������ÿ� �ʱ�ȭ
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
				//���� �߻�
			}
			break;
		case IoRead:
			pIOD->nCurrentBytes += dwIoSize;	//���� ���� ������
			dwFlags = 0;

			//nTotalBytes���� 0�̸� ó�� �����͸� �޴� ����̴�. �̶� ���۸� �ϳ� �����Ͽ� �����͸� ��´�.
			if(pIOD->nTotalBytes==0)
			{
				//binascii.unhexlify('0000000a') in python
				u_int totalBytes = ntohl(*((u_int*)pIOD->Buf));
				pIOD->nTotalBytes = totalBytes;
				pIOD->completeBuf = new u_char[totalBytes];
				memset(pIOD->completeBuf,0,totalBytes);
				printf("New total buffer allocate\n");
			}
			pBuffer = pIOD->completeBuf;	//��ü �����Ϳ� ���� ����

			printf("[Read Data] totalsize : %ubytes , csize : %dbytes , dwloSize : %dbytes\n",pIOD->nTotalBytes,pIOD->nCurrentBytes,dwIoSize);

			if (pIOD->nCurrentBytes < pIOD->nTotalBytes ) // ���� ���޾ҵ�..
			{
				memcpy(pBuffer+(pIOD->nCurrentBytes-dwIoSize),pIOD->wsabuf.buf,dwIoSize);	//���� �Ϻ� �����͸� ��ü ���ۿ� ����
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
				memcpy(pBuffer+(pIOD->nCurrentBytes-dwIoSize),pIOD->wsabuf.buf,dwIoSize);	//���� �Ϻ� �����͸� ��ü ���ۿ� ����
				printf("All data Received\n");
				printf("bufdata : %s\n",pBuffer+4);
				CloseHandle( CreateThread(NULL, 0, ProcessThread, pSD, 0, 0) );
				//ó�� ������ ����
			}else
			{	//RECEIVE INVALID DATA
				//CloseHandle( CreateThread(NULL, 0, ProcessThread, pSD, 0, 0) );

				pSD->Status = false;
				pSrv->m_SocketIndexQ.Put( pSD->index );
				pSrv->CloseClient( pSD );
				pSrv->ReleaseData( pSD );
				printf("Receive invalid data!!!\n");
				//���� �߻�
			}

			break;

		} // switch
	} // while
	return 0;
}
// ���� Ŭ���̾�Ʈ�� ��û�� �޾Ƽ�  Completion Port������Ʈ�� �������ش�.
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
	static int		addrlen = sizeof(RecvAddr);  // �۽��� ip�ּҾ˾Ƴ���
	char*			ClientAddr;

	HWND *phWnd;


	while (1)
	{
		//printf("AcceptStart\n");
		sdAccept = WSAAccept(pSrv->GetLSocket(),(struct sockaddr *)&RecvAddr,
			&addrlen	,NULL,0);

		//printf("NewSocket\n");
		getpeername(sdAccept,(struct sockaddr *)&RecvAddr,&addrlen);

		ClientAddr = inet_ntoa(RecvAddr.sin_addr);	//������ �ּҸ� ��

		if (sdAccept == SOCKET_ERROR)
		{
			if ( !pSrv->GetStatus() )				//������ ����Ǿ��� ��(�÷��� ��=false)
				break;

			continue;
		}

		//IOCP�� ��ϵ� ���۱���ü ��ȯ
		//SOCKET_DATA	//WSAOVERLAPPED �� ������ Ŭ����
		//IO���� ����	//���� ����
		pSD = pSrv->UpdataCompletionPort(sdAccept,ClientAddr);	

		if (pSD == NULL)
			break;

		if ( !pSrv->GetStatus() )
			break;

		pSD->Socket = sdAccept;

		//��� 6��° �Ű������� ���ϵ����� ����ü�� �ּҿ� ���������̴�.
		nRet = WSARecv( sdAccept, &(pSD->IOData[0].wsabuf), 1, &dwRecvNBytes, &dwFlags, &(pSD->IOData[0].Overlapped), NULL);

		if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
			pSrv->CloseClient(pSD);
			printf("WSARecv ERROR\n");
		}

		Sleep(0);
	}

	return 0;
}

// DBó��
// �����ͱ׷��� �̿��� ó���� �ϴ� ������ (�۾������� ���� �߿�)
DWORD WINAPI DbGatewayThread ( LPVOID ScatterThreadContext )
{
	CPacket pack;
	PSOCKET_DATA pSockets;
	DWORD	dwSendNBytes=0;
	DWORD	dwFlags=0;
	int index;

	while (1)
	{
		CIocpSrv::s_PacketQ.Get(pack);//ť�� ������Ŷ�� �����´�.
		pSockets = g_pSrv->GetSockets();
		index = pack.GetIndex();

		WSASend( pSockets[index].Socket, &(pSockets[index].IOData[1].wsabuf), 1, &dwSendNBytes,
			dwFlags, &(pSockets[index].IOData[1].Overlapped), NULL);
		/*
		// type�� ���� ó��.
		switch (pack.GetProtoData().nType) { 
		// Ŭ���̾�Ʈ�� ���ؽ���Ʈ���� �Ǻ���û������.
		// Ŭ���̾�Ʈ����  PROTODATA����ü������ ������� send���ش�.
		// type �� 0�� ���ؽ���Ʈ�� �ƴϰ�  1�� ���ؽ���Ʈ�̴�.
		// contents�迭�� ������ �־��ش�.
		case SM_CONNECT: 

		// db���� ip�� select�Ѵ�.
		// false�� �����ߴٴ°��� ������ �ִ�.
		if( !g_pQuery->HarmfulSearch(pack.GetProtoData().szContent,pack.GetProtoData().grade) )
		{
		pack.GetProtoData().nType = 0;
		if (g_pQuery->GetRet() == SQL_NO_DATA)
		{
		// m_Ret�� SQL_NO_DATA��°��� ���ؽ���Ʈ�� �ƴϴ� 
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

		case SM_USERID: // ȸ���� ��ǻ�͸� �״�.
		g_pQuery->UserUpdate(pack.GetProtoData().szContent,pSockets[index].IpAddress);
		break;
		case SM_USEEND: // ȸ���� ��ǻ�͸� ����.
		g_pQuery->UserEnd(pack.GetProtoData().szContent);
		break;

		case SM_ADDURL: // ȸ���� ���ؽ���Ʈ�� ���ÿ� �߰��Ͽ���.
		g_pQuery->AddHarmfulURL(pack.GetProtoData().szContent);
		break;

		case SM_GETPWD: // ���α׷� ���� ���º��� ������ ���� ȸ�� Ȯ������
		if( !g_pQuery->GetPassWord(pack.GetProtoData().szContent) )
		{
		pack.GetProtoData().nType = 0;
		if (g_pQuery->GetRet() == SQL_NO_DATA)
		{
		// ���̵� ���� �����ʽ��ϴ�.
		strcpy(pack.GetProtoData().szContent,"NO_ID");
		}
		else
		{	// �Ϲ����� ����.
		strcpy(pack.GetProtoData().szContent,"ERROR");
		}
		}
		else 
		{	// ȸ������ Ȯ�ε�
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
		{	// ���̵� ���� �����ʽ��ϴ�.
		strcpy(pack.GetProtoData().szContent,"NO_ID");
		}
		else
		{	// �Ϲ����� ����.
		strcpy(pack.GetProtoData().szContent,"ERROR");
		}
		}
		else 
		{	// ���̵� ����
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
// �����ͱ׷��� �̿��� ó���� �ϴ� ������ (�ڿ����� ����)
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
	//���� �ʱ�ȭ
	if (nRet = SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()) )
	{
		pSD->Status = false;
		pSrv->m_SocketIndexQ.Put( pSD->index );
		pSrv->CloseClient( pSD );
		pSrv->ReleaseData( pSD );
	}
	return 0;
}