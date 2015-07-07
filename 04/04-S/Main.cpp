/**********************************************************
Server
**********************************************************/
// INCLUDES ///////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN 
#include <windows.h> 
#include <windowsx.h> 
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <conio.h>
#include <WinSock2.h>
// DEFINES ////////////////////////////////////////////////
// defines for windows
#define WINDOW_CLASS_NAME "WINCLASS1"
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)



#define WM_SERVER (WM_USER + 1008)

// GLOBALS ////////////////////////////////////////////////
HWND listBox;
HWND IPAddress;
HWND PortNumber;
HWND UserName;
HWND YourMessage;
HWND connectBind;
HWND QuitExit;
HWND Send;
HWND mainHWND;
HINSTANCE mainHINSTANCE;

char IP[16] = "127.0.0.1", Port[6], UName[11], Message[125];

char ServerIP[15] = "127.0.0.1";

struct IPADDRESS
{
	int seg1, seg2, seg3, seg4;
};

IPADDRESS ClientIP;
char SendData[80];
int SendBytes = 0;
char RecvData[80];
int RecvBytes= 0;
int clientSize = sizeof(sockaddr_in);
int iResult = 0;
char Buffer[250];


SOCKET skServer;
SOCKET skClient;
sockaddr_in saServer;
sockaddr_in saClient;
WSADATA wsaData;
// FUNCTIONS //////////////////////////////////////////////
void vShowText(HWND hChildHandle, char *szText);
bool Setup()
{

	SetWindowText(PortNumber, "2006");
	GetWindowText(PortNumber, Port, 5);
	EnableWindow(PortNumber, false);
	GetWindowText(UserName, UName, 10);
	SetWindowText(UserName, UName);
	SetWindowText(IPAddress, IP);
	EnableWindow(IPAddress, false);
	//EnableWindow(YourMessage, false);
	//EnableWindow(Send, false);


	/**************************************************************************
	SETUP SOCKET VARIABLES AN OBJECTS
	**************************************************************************/
	iResult = WSAStartup(MAKEWORD(2, 0), &wsaData);

	if (iResult != NO_ERROR)
	{
		MessageBox(0,"Error with WSAStartup()\n",0,0);
		return(false);
	}

/**************************************************************************
	CLEAR MEMORY AREA - SET ALL TO ZERO (0)
	**************************************************************************/
	memset(RecvData, 0, sizeof(RecvData));
	memset(&saServer, 0, sizeof(sockaddr_in));

	/**************************************************************************
	SETUP SOCKET ADDRESS INFORMATION
	**************************************************************************/
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = inet_addr(IP);
	saServer.sin_port = htons(2006);

	/**************************************************************************
	OPEN SOCKET FOR CONNECTION
	**************************************************************************/
	skServer = socket(AF_INET, SOCK_STREAM, 0);
	if(skServer == INVALID_SOCKET)
	{
		sprintf(Buffer, "ERROR with socket():%i\n", WSAGetLastError());
		MessageBox(0,Buffer,0,0);
		WSACleanup();
		getch();
		return(false);
	}
	else
	{
		sprintf(Buffer, "SERVER SOCKET:\topen...\n");
		vShowText(listBox, Buffer);
		sprintf(Buffer, "SERVER TYPE:\tTCP...\n\n");
		vShowText(listBox, Buffer);
	}
	
	if(WSAAsyncSelect(skServer, mainHWND, WM_SERVER, FD_ACCEPT|FD_CLOSE | FD_READ | FD_CONNECT) == 0)
	{
		sprintf(Buffer, "WSAAsyncSelect() is OK\n");
		vShowText(listBox, Buffer);
	}
	else
	{
		sprintf(Buffer, "ERROR with WSAAsyncSelect():%i\n", WSAGetLastError());
		MessageBox(0,Buffer, 0, 0);
		return false;
	}

	iResult = bind(skServer, (struct sockaddr*)&saServer, sizeof(sockaddr));
	if(iResult == SOCKET_ERROR)
	{
		sprintf(Buffer, "Error binding socket(): %i\n", WSAGetLastError());
		MessageBox(0,Buffer, 0, 0);
		closesocket(skServer);
		WSACleanup();
		getch();
		return(false);
	}
	else
	{
		vShowText(listBox, "BIND SERVER: Socket Bound\n");
		if(listen(skServer, SOMAXCONN) == SOCKET_ERROR)
		{
			sprintf(Buffer, "Error listening on socket(): %i\n", WSAGetLastError());
			MessageBox(0,Buffer, 0, 0);
			closesocket(skServer);
			WSACleanup();
			getch();
			return(false);
		}
		else
		{
			vShowText(listBox, "Listening on port 2006\n\n");
		}
	}

	return true;
}

void vShowText(HWND hChildHandle, char *szText)
{
	int line;
	//Add string to the listbox
	SendMessage(hChildHandle, LB_ADDSTRING, NULL, (LPARAM)szText);
	//determine number of lines in listbox
	line = SendMessage(hChildHandle, LB_GETCOUNT, NULL, NULL);
	//flag last entry as the selected item to scroll down listbox
	SendMessage(hChildHandle, LB_SETCURSEL, line-1, NULL);
	//unflag all items to avoid negative highlite
	SendMessage(hChildHandle, LB_SETCURSEL, -1, NULL);
}



LRESULT CALLBACK WindowProc(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)
{

	PAINTSTRUCT ps; 
	HDC hdc; 
	switch(msg)
	{
	case WM_COMMAND:
		{
			// test for id
			//connect/Bind
			if (LOWORD(wparam) == 2000)
			{
				if(!Setup())
				{
					MessageBox(0, "Setup() - FAILED", 0, 0);
					return 0;
				}

				// do whatever
				GetWindowText(IPAddress, IP, 15);
				GetWindowText(PortNumber, Port, 5);
				GetWindowText(UserName, UName, 10);

				vShowText(listBox, IP);
				vShowText(listBox, Port);
				vShowText(listBox, UName);


EnableWindow(connectBind, false);
			} // end if
			// process all other child controls, menus, etc.
			// we handled it

			// test for id
			//Quit/Exit
			if (LOWORD(wparam) == 2100)
			{
				SendMessage(hwnd,WM_CLOSE,0,0);
			} // end if
			// process all other child controls, menus, etc.
			// we handled it

			// test for id
			//Send
			if (LOWORD(wparam) == 2200)
			{			
				GetWindowText(YourMessage, Message, 70);
				if(Message[0] != '\0')
				{
					vShowText(listBox, Message);
					SendBytes = send(skClient, Message, sizeof(SendData), 0);
				}
				SetWindowText(YourMessage, '\0');
				// do whatever
			} // end if
			// process all other child controls, menus, etc.
			// we handled it
			return(0);
		} break;
	case WM_SERVER:
		{
			if(WSAGETSELECTERROR(lparam))
			{
				sprintf(Buffer, "Select Error:%i\n", WSAGetLastError());
				SendMessage(hwnd,WM_CLOSE,0,0);
			}
			switch(WSAGETSELECTEVENT(lparam))
			{
			case FD_ACCEPT:
				skClient= accept(skServer, (struct sockaddr*)&saClient, &clientSize);
				ClientIP.seg1 = saClient.sin_addr.S_un.S_un_b.s_b1;
				ClientIP.seg2 = saClient.sin_addr.S_un.S_un_b.s_b2;
				ClientIP.seg3 = saClient.sin_addr.S_un.S_un_b.s_b3;
				ClientIP.seg4 = saClient.sin_addr.S_un.S_un_b.s_b4;

				vShowText(listBox, "CONNECTION:\testablished with client...\n\n");

				sprintf(Buffer, "%i.%i.%i.%i.", ClientIP.seg1, ClientIP.seg2, ClientIP.seg3, ClientIP.seg4);
				EnableWindow(IPAddress, true);
				SetWindowText(IPAddress, Buffer);
				EnableWindow(IPAddress, false);

				break;
			case FD_CONNECT:
				//EnableWindow(YourMessage, true);
				//EnableWindow(Send, true);
				break;
			case FD_CLOSE:
				SendMessage(hwnd,WM_CLOSE,0,0);
				break;
			case FD_READ:
				RecvBytes = recv(skClient, RecvData, sizeof(RecvData), 0);
				vShowText(listBox, RecvData);
				break;
			}
		}
	case WM_CREATE:
		{

			return(0);
		} break;
	case WM_PAINT:
		{

			hdc = BeginPaint(hwnd,&ps);

			EndPaint(hwnd,&ps);

			return(0);
		} break;
	case WM_CLOSE:
		{
			closesocket(skServer);
			closesocket(skClient);
			WSACleanup();
		}break;
	case WM_DESTROY:
		{

			PostQuitMessage(0);

			return(0);
		} break;
	default:break;
	} 
	return (DefWindowProc(hwnd, msg, wparam, lparam));
} 
// WINMAIN ////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	WNDCLASSEX winclass; 
	HWND hwnd;

	MSG msg; 
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC |CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winclass))
		return(0);

	if (!(hwnd = CreateWindowEx(NULL,
		WINDOW_CLASS_NAME, 
		"SERVER", 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,0, 
		1200,900, 
		NULL, 
		NULL, 
		hinstance,
		NULL))) 
		return(0);

	mainHWND = hwnd;
	mainHINSTANCE = hinstance;

	
	listBox = CreateWindowEx(WS_EX_CLIENTEDGE, 
		"LISTBOX",
		NULL, 
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY,
		75, 300,
		1025, 400,
		mainHWND,
		NULL,
		mainHINSTANCE,
		NULL);


	IPAddress = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL, WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL,
		50, 50,
		300, 21,
		mainHWND,
		NULL,
		mainHINSTANCE,
		NULL);

	PortNumber = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL, WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL,
		450, 50,
		300, 21,
		mainHWND,
		NULL,
		mainHINSTANCE,
		NULL);

	UserName = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL, WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL,
		850, 50,
		300, 21,
		mainHWND,
		NULL,
		mainHINSTANCE,
		NULL);

	YourMessage = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL, WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE,
		75, 750,
		800, 21,
		mainHWND,
		NULL,
		mainHINSTANCE,
		NULL);

	connectBind = CreateWindowEx(WS_EX_APPWINDOW, 
		"BUTTON", 
		"connect/Bind",
		WS_CHILD | WS_VISIBLE,
		75, 150,
		400, 21,
		mainHWND,
		(HMENU)(2000),
		mainHINSTANCE, 
		NULL); 

	QuitExit = CreateWindowEx(WS_EX_APPWINDOW, 
		"BUTTON", 
		"Quit/Exit",
		WS_CHILD | WS_VISIBLE,
		625, 150,
		400, 21,
		mainHWND,
		(HMENU)(2100),
		mainHINSTANCE, 
		NULL); 

	Send = CreateWindowEx(WS_EX_APPWINDOW, 
		"BUTTON", 
		"Send",
		WS_CHILD | WS_VISIBLE,
		950, 750,
		150, 21,
		mainHWND,
		(HMENU)(2200),
		mainHINSTANCE, 
		NULL);



	SetWindowText(IPAddress, "IP Address");
	SetWindowText(PortNumber, "Port Number");
	EnableWindow(IPAddress, false);
	EnableWindow(PortNumber, false);
	SetWindowText(UserName, "User Name (Less than 10 Characters)");
	GetWindowText(YourMessage, Message, 100);
	//	EnableWindow(YourMessage, false);
	//EnableWindow(Send, false);



	while(GetMessage(&msg,NULL,0,0))
	{
		if(KEYDOWN(VK_RETURN))
			SendMessage(hwnd,WM_COMMAND,2200,0);

		TranslateMessage(&msg);

		DispatchMessage(&msg);
	} 
	return(msg.wParam);
} // end WinMain
///////////////////////////////////////////////////////////