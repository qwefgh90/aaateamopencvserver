// git_test.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "IOCPServer.h"	//서버 동작에 관련된 헤더
#include "MenuAnalyzer.h"
#include "DB_manager.h"
#include "Member_manager.h"
#include "Store_manager.h"
#include "ImageManager.h"

#include "resource.h"
//Tray 
#define WM_USER_SHELLICON WM_USER + 1	//기존 메세지 +1
#define PORT 8888
NOTIFYICONDATA Tray;//Tray Structure
HMENU hPopMenu;		//Popup Menu
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);	//Aboutbox message handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//Aboutbox CreateWindow handler
HWND MyCreateWindow(HINSTANCE hInst);											//RegisterClass, CreateWindow
HINSTANCE hInst;	// current process instance
//Service
DWORD WINAPI WorkerThread ( LPVOID WorkerThreadContext );
DWORD WINAPI AcceptThread( LPVOID DbGatewayThreadContext );
DWORD WINAPI ProcessThread(LPVOID recv_buf);
CIocpSrv *g_pSrv;	//서버 객체

void initManager();
void destroyMnager();

//TEST MODULE CHANG
#include "testcase.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//chang_test();
	//chang_cache();
	//initManager();
	//chang_cookie();
	//return 0;

	MSG msg;
	HWND consolHwnd;			//Console Handle
	HWND windowHwnd;			//CreateWindow Handle
	HACCEL hAccelTable;	
	//hide console window
	consolHwnd = FindWindow("ConsoleWindowClass",NULL);
	//ShowWindow(consolHwnd,0);
	hInst = GetModuleHandleA(NULL);
	//1)RegisterClass, CreateWindow to handle wndproc for handling message
	windowHwnd = MyCreateWindow(hInst);
	
	//2)Tray , register callback, Shell_NotifyIcon() in <shellapi.h>
	//tray info
	Tray.cbSize=sizeof(Tray);
	Tray.hIcon=LoadIcon(NULL,IDI_WINLOGO);
	Tray.hWnd=windowHwnd;
	strcpy_s(Tray.szTip,"ImageProcessServer");
	Tray.uID = IDR_MENU1;
	Tray.uCallbackMessage=WM_USER_SHELLICON;
	Tray.uFlags=NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Shell_NotifyIcon(NIM_ADD, &Tray);

	WSADATA stWSAData = {0};
	int nWSAError = WSAStartup( MAKEWORD( 2, 2 ), &stWSAData );
	if( nWSAError != 0 )
	{
		return nWSAError;
	}
	
	initManager();//Initialize Managers
	g_pSrv = CIocpSrv::CreateInstance(PORT,5);
	if ( !g_pSrv->CreateLitenSocket() )			//Win소켓 -> bind() listen() 초기화
		printf("bind(), listen()실패\n");
	g_pSrv->StartThreadRoutine(WorkerThread,1);	//작업 처리 스레드
	g_pSrv->StartThreadRoutine(AcceptThread,1);	//접속 대기
	//3)서버 생성
	//4)서버 IOCP큐 쓰레드 생성
	//5)서버 동작
	//6)큐에 데이터가 찰 경우 데이터를 영상처리 쓰레드에 넘긴다
	//7)영상처리 쓰레드가 처리를 완료하고 Write를 한다.
	//8)클라이언트 측에서 연결을 끊기를 원할경우나 커넥션이 끊어졌을경우도 고려한다.

	//9)DispatchMessage()
	hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MENU1));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

HWND MyCreateWindow(HINSTANCE hInst)
{
	char* windowname = "Console";	//CreateWindow Name
	HWND windowHwnd;
	WNDCLASS wcex;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.style			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.hIcon			= NULL;
	wcex.hInstance		= hInst;
	wcex.lpfnWndProc	= WndProc;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= windowname;
	RegisterClass(&wcex);

	windowHwnd = CreateWindow(windowname,windowname,WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,CW_USEDEFAULT,
		NULL, (HMENU)NULL,hInst, NULL);	//윈도우 등록
	return windowHwnd;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	POINT lpClickPoint;
	switch (message)
	{

	case WM_USER_SHELLICON:		//윈도우 메세지 발생
		switch(LOWORD(lParam)) 
		{   
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:	//오른쪽 클릭시 팝업 메뉴 생성
			UINT uFlag = MF_BYPOSITION|MF_STRING;
			GetCursorPos(&lpClickPoint);
			hPopMenu = CreatePopupMenu();
			//InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,ID_TRAY_START,"Start");
			//InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,ID_TRAY_STOP,"Stop");
			InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,ID_TRAY_TERMINATE,"Terminate");
			InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,ID_TRAY_ABOUT,"About");
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,lpClickPoint.x, lpClickPoint.y,0,hWnd,NULL);
			return true;
		}
	case WM_COMMAND:			//정의한 메세지
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_TRAY_START:		//Server Start
			MessageBox(NULL,"This is a test for menu Test 1","Start",MB_OK);
			break;
		case ID_TRAY_STOP:		//Server Stop
			MessageBox(NULL,"This is a test for menu Test 1","Stop",MB_OK);
			break;
		case ID_TRAY_TERMINATE:	//Server Terminate
			Shell_NotifyIcon(NIM_DELETE,&Tray);
			MessageBox(NULL,"Terminate","Stop",MB_OK);
			ExitProcess(0);
			DestroyWindow(hWnd);
			break;
		case ID_TRAY_ABOUT:		//About us
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}

//Message Handler Aboutbox
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//  특정 서버 객체에 대한 대기큐 
//  이전의 요청한 IO의 완료 결과를 통보 받고 그 결과에 따라 어떤 IO를 더 요청하거나
//  아니면 다른작업을 합니다. 거의 중심적인 IO처리가 이루워지게 됩니다.
DWORD WINAPI WorkerThread ( LPVOID WorkerThreadContext )
{
	CIocpSrv		*pSrv = (CIocpSrv *)WorkerThreadContext;//스레드 전담 서버 객체
	HANDLE			hIOCP = pSrv->GetHIocp();				//IOCP핸들
	BOOL			bSucces = FALSE;			
	int				nRet=1;			
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
		{
			printf("[WorkerThread] PSOCKET_DAT is NULL\n",nRet);
			return (0);
		}

		if ( !bSucces || (bSucces && (0 == dwIoSize)))	//false거나, 성공했지만 데이터가 없을경우
		{
			printf("[WorkerThread] dwIoSize==0 %d\n",nRet);
			pSD->Status = false;
			pSrv->CloseClient(pSD);
			pSrv->ReleaseData( pSD );
			pSrv->m_SocketIndexQ.Put( pSD->index );
			continue;
		}

		pIOD = (PIO_DATA)pOverlapped; 

		//사용 되는 변수
		//nCurrentBytes	현재 받거나 보낸 버퍼의 크기
		//nTotalBytes	현재 보내야 하는 버퍼의 총 크기
		//completeBuf	보낸 버퍼 / 받은 버퍼
		//wsabuf
		//상대가 하나의 포트로 여러개의 데이터를 보낸다면 completeBuf는 공유되므로 예기치못한 오류가 발생한다.
		//nTotalBytes, nCurrentBytes 역시 마찬가지
		//통보받은 IO를 구분하고 그에해당하는 처리가 이루워 진다.
		switch (pIOD->IOOperation) { 
		case IoWrite:
			pIOD->nCurrentBytes += dwIoSize;
			dwFlags = 0;
			//printf("[Write Data Info] totalsize : %ubytes , csize : %dbytes , dwloSize : %dbytes\n",pIOD->nTotalBytes,pIOD->nCurrentBytes,dwIoSize);

			if (pIOD->nCurrentBytes < pIOD->nTotalBytes ) // 아직 덜보냈다..
			{
				//printf("Write more data : %ubytes\n",pIOD->nTotalBytes-pIOD->nCurrentBytes);
				pIOD->wsabuf.buf = pIOD->Buf + pIOD->nCurrentBytes;			//complete buffer
				pIOD->wsabuf.len = pIOD->nTotalBytes - pIOD->nCurrentBytes;	//complete buffer

				nRet = WSASend( pSD->Socket, &(pIOD->wsabuf), 1, &dwSendNBytes,
					dwFlags, &(pIOD->Overlapped), NULL);

				if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()) )
				{
					printf("[IoWrite] Socket Error _ , code: %d\n",nRet);
					pSD->Status = false;
					pSrv->m_SocketIndexQ.Put( pSD->index );
					pSrv->CloseClient( pSD );
					pSrv->ReleaseData( pSD );
				}
				continue;
			}
			else if(pIOD->nCurrentBytes == pIOD->nTotalBytes) // 다보냈을시에 초기화
			{
				printf("[Write]Send All- total : %ubytes , csize : %dbytes , dwloSize : %dbytes\n",pIOD->nTotalBytes,pIOD->nCurrentBytes,dwIoSize);
				//1)통신 바로 종료
				pSD->Status = false;
				pSrv->m_SocketIndexQ.Put( pSD->index );
				pSrv->CloseClient( pSD );
				pSrv->ReleaseData( pSD );

				//2)다음 통신 대기
				/*nRet = WSARecv( pSD->Socket, &(pSD->IOData[0].wsabuf), 1, &dwRecvNBytes,
					&dwFlags, &(pSD->IOData[0].Overlapped), NULL);
				if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError()) )
				{
					printf("Socket Error _ , code: %d\n",nRet);
					pSD->Status = false;
					pSrv->m_SocketIndexQ.Put( pSD->index );
					pSrv->CloseClient( pSD );
					pSrv->ReleaseData( pSD );
				}*/

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
				u_int totalBytes = *((u_int*)pIOD->Buf);
				pIOD->nTotalBytes = totalBytes;
				pIOD->completeBuf = new u_char[totalBytes];
				memset(pIOD->completeBuf,0,totalBytes);
			}
			pBuffer = pIOD->completeBuf;	//전체 데이터에 대한 버퍼 , completeBuf
			//printf("[Read Data] totalsize : %ubytes , csize : %dbytes , dwloSize : %dbytes\n",pIOD->nTotalBytes,pIOD->nCurrentBytes,dwIoSize);
			if (pIOD->nCurrentBytes < pIOD->nTotalBytes ) // 아직 덜받았따..
			{
				memcpy(pBuffer+(pIOD->nCurrentBytes-dwIoSize),pIOD->wsabuf.buf,dwIoSize);	//받은 일부 데이터를 전체 버퍼에 복사
				//printf("Wait more data : %ubytes\n",pIOD->nTotalBytes-pIOD->nCurrentBytes);
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
				printf("[Read]All Received - total : %ubytes , csize : %dbytes , dwloSize : %dbytes\n",pIOD->nTotalBytes,pIOD->nCurrentBytes,dwIoSize);
			
				memcpy(pBuffer+(pIOD->nCurrentBytes-dwIoSize),pIOD->wsabuf.buf,dwIoSize);	//받은 일부 데이터를 전체 버퍼에 복사
				//printf("bufdata : %s\n",pBuffer+4);
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
// DB처리
// 데이터그램을 이용해 처리를 하는 쓰레드 (작업쓰레드 가장 중요)
// 데이터그램을 이용해 처리를 하는 쓰레드 (자원해제 포함)
DWORD WINAPI ProcessThread(LPVOID recv_buf)
{
	int nRet;
	DWORD dwFlags = 0;
	DWORD dwSendNBytes =0;
	Memory recv_data;
	Memory send_data;
	memset(&recv_data,0,sizeof(Memory));
	memset(&send_data,0,sizeof(Memory));
	MenuAnalyzer* analyzer= MenuAnalyzer::GetMenuAnalyzer();	//Menu analyzer
	PSOCKET_DATA pSD = (PSOCKET_DATA)recv_buf;					//Received data from Mobile	about SOCKET_DATA
						
	//0)configure Memory structure 
	recv_data.buf = pSD->IOData[0].completeBuf;					//complete buffer is available releasing
	recv_data.len = pSD->IOData[0].nCurrentBytes;				//n-bytes
	//1)parse(recv_buf) - MenuAnalyzer - return buffer and buffer_length

	analyzer->MenuSelector(recv_data,send_data);

	//2)allocate completeBuf and copy original buf to complete buffer

	//3)send data to mobile

	//4)release resource (malloc)
	
	int send_len = send_data.len;	//buffer length

	//5)allocate memory of completebuffer
	pSD->IOData[1].completeBuf = new u_char[send_len];
	memcpy_s(pSD->IOData[1].completeBuf,send_len,send_data.buf,send_len);
	delete[] send_data.buf;

	//printf("new addr %x\n",pSD->IOData[1].completeBuf);
	//6)assemble send buffer
	pSD->IOData[1].nTotalBytes = send_len;
	pSD->IOData[1].wsabuf.buf = (char*)pSD->IOData[1].completeBuf;		//To send data to Mobile
	pSD->IOData[1].wsabuf.len = send_len;									//To send data to Mobile

	nRet = WSASend( pSD->Socket, &(pSD->IOData[1].wsabuf), 1, &dwSendNBytes,
		dwFlags, &(pSD->IOData[1].Overlapped), NULL);

	//버퍼 초기화
	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()) )
	{
		pSD->Status = false;
		g_pSrv->m_SocketIndexQ.Put( pSD->index );
		g_pSrv->CloseClient( pSD );
		g_pSrv->ReleaseData( pSD );
	}
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

	HWND *phWnd		=NULL;


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
		if (pSD == NULL)	//사용할 수 있는 소켓이 없을경우
			continue;

		if ( !pSrv->GetStatus() )
			break;

		pSD->Socket = sdAccept;

		//사실 6번째 매개변수는 소켓데이터 구조체의 주소와 마찬가지이다.
		nRet = WSARecv( sdAccept, &(pSD->IOData[0].wsabuf), 1, &dwRecvNBytes, &dwFlags, &(pSD->IOData[0].Overlapped), NULL);
		printf("\n\n\n");
		if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) {
			pSrv->CloseClient(pSD);
			printf("WSARecv ERROR\n");
		}
		Sleep(0);
	}

	return 0;
}
void initManager(){
	Member_manager::GetMember_manager();
	DB_manager::GetDB_manager();
	Store_manager::GetStore_manager();
	ImageManager::getImageManager();
}

void destroyMnager(){
	
}
