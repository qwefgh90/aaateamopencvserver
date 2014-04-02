// you can use the win api in your console program.

// the window will minimize into the system tray for 5 seconds and reappear

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include "resource.h"

#define WM_USER_SHELLICON WM_USER + 1	//기존 메세지 +1
NOTIFYICONDATA Tray;//Tray Structure
HMENU hPopMenu;		//Popup Menu
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);	//Aboutbox message handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//Aboutbox CreateWindow handler
HINSTANCE hInst;	// current process instance

HWND MyCreateWindow(HINSTANCE hInst);
int main(int argc, char* argv[])
{
	MSG msg;
	HWND consolHwnd;			//Console Handle
	HWND windowHwnd;			//CreateWindow Handle
	HACCEL hAccelTable;	
	//hide console window
	consolHwnd = FindWindow("ConsoleWindowClass",NULL);
	ShowWindow(consolHwnd,0);
	hInst = GetModuleHandleA(NULL);
	//1)RegisterClass, CreateWindow to handle wndproc for handling message
	windowHwnd = MyCreateWindow(hInst);
	
	//2)Tray , register callback, Shell_NotifyIcon() in <shellapi.h>
	//tray info
	Tray.cbSize=sizeof(Tray);
	Tray.hIcon=LoadIcon(NULL,IDI_WINLOGO);
	Tray.hWnd=windowHwnd;
	strcpy(Tray.szTip,"ImageProcessServer");
	Tray.uID = IDR_MENU1;
	Tray.uCallbackMessage=WM_USER_SHELLICON;
	Tray.uFlags=NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Shell_NotifyIcon(NIM_ADD, &Tray);

	//3)DispatchMessage()
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
		case WM_RBUTTONDOWN:	//오른쪽 클릭시 팝업 메뉴 생성
			UINT uFlag = MF_BYPOSITION|MF_STRING;
			GetCursorPos(&lpClickPoint);
			hPopMenu = CreatePopupMenu();
			InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,ID_TRAY_START,"Start");
			InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,ID_TRAY_STOP,"Stop");
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
			MessageBox(NULL,"This is a test for menu Test 1","Terminate",MB_OK);
			Shell_NotifyIcon(NIM_DELETE,&Tray);
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
