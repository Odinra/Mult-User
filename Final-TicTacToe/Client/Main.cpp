#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WindowsX.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <WinSock2.h>
#include <iostream>
#include <math.h>
#include <cmath>

using namespace std;

#define WINDOW_CLASS_NAME "WINCLASS1"
#define BUTTON_BASE_ID1 100
#define WM_SERVER (WM_USER + 1008)
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define SERVERCOLOR RGB(255, 0, 0)
#define CLIENTCOLOR RGB(0, 0, 255)
#define HIGHLIGHTCOLOR RGB(255,242,0)
#define BLACKCOLOR RGB(0,0,0)
char num[5];

int p1Score = 0, p2Score = 0;

int recvX, recvY;


//MULTIUSERSTUFF
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
	char Buffer2[4];
	//char InitialServer[2];
	//char InitialClient[2];

	SOCKET skServer;
	SOCKET skClient;
	sockaddr_in saServer;
	sockaddr_in saClient;
	WSADATA wsaData;

	HWND listBox;
	HWND IPAddress;
	HWND PortNumber;
	HWND UserName;
	HWND YourMessage;
	HWND connectBind;
	HWND QuitExit;
	HWND Send;
//END MULTIUSERSTUFF

struct Player
{
	char Name[2];
	int score;
	bool turn;
	COLORREF c;
	bool isX;
};

struct box
{
	bool played;
	Player* p;
	int top, left, bottom, right;
	box(int t, int b, int l, int r)
	{
		top = t; bottom = b; left = l, right = r;
		played = false;
	}
	box(){}
};
Player Server, Client, Highlighter;

box *boxes[9];

HPEN red = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
HPEN blue = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));

HDC gHDC;
HWND gHWND;
HINSTANCE gHINSTANCE;
bool ready = false;
void BoxHighlight(int, int, Player*);
bool BoxSelect(int, int, Player*);
void SendSelect(int, int);
void FillX(box*, Player*);
void FillO(box*, Player*);
bool Setup();
void vShowText(HWND hChildHandle, char *szText);
void CreateButtons();
bool winTest(Player* p);
void FillBox(box*, COLORREF);
void restart();


LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	int mouse_x = (int)LOWORD(lparam);
	int mouse_y = (int)HIWORD(lparam);
	switch(msg)
	{
	case WM_CREATE:
		{
			

			return(0);
		}break;
	case WM_MOUSEMOVE:
		{
			
			int mouse_x = (int)LOWORD(lparam);
			int mouse_y = (int)HIWORD(lparam);
			BoxHighlight(mouse_x, mouse_y, &Highlighter);
			int buttons = (int)wparam;
		}break;
	case WM_LBUTTONDOWN:
		{
			if(ready)
			{
			int mouse_x = (int)LOWORD(lparam);
			int mouse_y = (int)HIWORD(lparam);
			if(Client.turn)
			{
				if( BoxSelect(mouse_x, mouse_y, &Client))
				{
					SendSelect(mouse_x, mouse_y);
				
					Server.turn = true;
					Client.turn = false;
				}
			}
			}
		}break;
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
				GetWindowText(IPAddress,IP, 15);
				GetWindowText(PortNumber,Port, 5);
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
				GetWindowText(YourMessage,Message, 70);
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
			//case FD_ACCEPT:
			//	skServer= accept(skServer, (struct sockaddr*)&saClient, &clientSize);
			//	ClientIP.seg1 = saClient.sin_addr.S_un.S_un_b.s_b1;
			//	ClientIP.seg2 = saClient.sin_addr.S_un.S_un_b.s_b2;
			//	ClientIP.seg3 = saClient.sin_addr.S_un.S_un_b.s_b3;
			//	ClientIP.seg4 = saClient.sin_addr.S_un.S_un_b.s_b4;

			//	vShowText(listBox, "CONNECTION:\testablished with client...\n\n");

			//	sprintf(Buffer, "%i.%i.%i.%i.", ClientIP.seg1, ClientIP.seg2, ClientIP.seg3, ClientIP.seg4);
			//	EnableWindow(IPAddress, true);
			//	SetWindowText(IPAddress, Buffer);
			//	EnableWindow(IPAddress, false);

			//	break;
			case FD_CONNECT:
				EnableWindow(YourMessage, true);
				EnableWindow(Send, true);
				Sleep(500);
				Client.Name[0] = UName[0];
				send(skServer, Client.Name, sizeof(Client.Name), 0);
				break;
			case FD_CLOSE:
				SendMessage(hwnd,WM_CLOSE,0,0);
//				delete [] boxes;
				break;
			case FD_READ:
				for(int i = 0; i < 80; i++)
				{
					RecvData[i] = '\0';
				}
				if(!ready)
				{
					recv(skServer, Server.Name, sizeof(Server.Name), 0);
					ready = true;
					Sleep(500);			
					
					char ch[2];
					recv(skServer, ch, sizeof(ch), 0);
					if(ch[0] == 'X')
						Client.isX = false;
					else
						Client.isX = true;
					Server.isX = !Client.isX;
					Server.turn = Server.isX;
					Client.turn = Client.isX;
					Highlighter.isX = Client.isX;
					
				}
				else
				{
				RecvBytes = recv(skServer, RecvData, sizeof(RecvData), 0);
				//vShowText(listBox, RecvData);
				if(RecvData[0] == 'M')
				{
					//Sleep(500);
					RecvBytes = recv(skServer, Buffer2, sizeof(Buffer2), 0);
					recvX = atoi(Buffer2);

					//Sleep(500);
					RecvBytes = recv(skServer, Buffer2, sizeof(Buffer2), 0);
					recvY = atoi(Buffer2);

					BoxSelect(recvX, recvY, &Server);

					Client.turn = true;
					Server.turn = false;
				}
				else
					vShowText(listBox, RecvData);
				}
				break;
			}
		}
	case WM_PAINT:
		{
			gHDC = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		}break;
	case WM_DESTROY:
		{
//			delete [] boxes;
			PostQuitMessage(0);
			return(0);
		}break;
	default:break;
	}
	return(DefWindowProc(hwnd, msg, wparam, lparam));
}
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASSEX winclass; //structure that holds the class
	HWND hwnd; //windows handles
	MSG msg; //message id
	HDC hdc;

	winclass.cbSize = sizeof(WNDCLASSEX); //size of class structure
	winclass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC|CS_DBLCLKS;
	winclass.lpfnWndProc = WinProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_HAND);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//Register class
	if(!RegisterClassEx(&winclass))
		return (0);


	if(!(hwnd = CreateWindowEx(NULL, WINDOW_CLASS_NAME, "Tic Tac Toe - Client", WS_OVERLAPPED|WS_SYSMENU|WS_VISIBLE,
		0, 0, 1280, 1024, NULL, NULL, hinstance, NULL)))
		return (0);


	hdc = GetDC(hwnd);
	gHDC = hdc;
	gHWND = hwnd;
	gHINSTANCE = hinstance;

	srand(GetTickCount());

	CreateButtons();
	HBRUSH hbrush = CreateSolidBrush(RGB(0, 255, 0));
	HPEN white = CreatePen(PS_SOLID, 1, RGB(125, 125, 125));
	HPEN green = CreatePen(PS_SOLID, 4, RGB(125, 125, 125));
	HPEN old_hpen = (HPEN)SelectObject(gHDC, green);


	Server.c = SERVERCOLOR;
	Server.score = 0;
	//Server.isX = rand()%2;
	//Server.turn = Server.isX;
	Client.c = CLIENTCOLOR;
	Client.score = 0;
	//Client.isX = !Server.isX;
	//Client.turn = Client.isX;
	Highlighter.c = HIGHLIGHTCOLOR;
	//Highlighter.isX = Client.isX;
	

	//box(top, bottom, left, right)
	boxes[0] = new box(100, 300, 100, 300);//topLeft
	boxes[1] = new box(100, 300, 300, 500);//topMid
	boxes[2] = new box(100, 300, 500, 700);//topRight
	boxes[3] = new box(300, 500, 100, 300);//MidLeft
	boxes[4] = new box(300, 500, 300, 500);//MidMid
	boxes[5] = new box(300, 500, 500, 700);//MidRight
	boxes[6] = new box(500, 700, 100, 300);//BottomLeft
	boxes[7] = new box(500, 700, 300, 500);//BottomMid
	boxes[8] = new box(500, 700, 500, 700);//BottomRight


	MoveToEx(gHDC, 300, 100, 0);//FirstVertLineTop
	LineTo(gHDC, 300, 700);//FirstVertLineBottom
	MoveToEx(gHDC, 500, 100, 0);//SecondVertLineTop
	LineTo(gHDC, 500, 700);//SecondVertLineBottom
	MoveToEx(gHDC, 100, 300, 0);//FirstHorizLineLeft
	LineTo(gHDC, 700, 300);//FirstHorizLineRight
	MoveToEx(gHDC, 100, 500, 0);//SecondHorizLineLeft
	LineTo(gHDC, 700, 500);//SecondHorizLineRight
	
	GetWindowText(YourMessage, Message, 100);

	FillX(boxes[4], &Server);
	FillO(boxes[5], &Client);
	
	SetWindowText(IPAddress, "IP Address");
	SetWindowText(PortNumber, "Port Number");
	//EnableWindow(IPAddress, false);
	EnableWindow(PortNumber, false);
	SetWindowText(UserName, "Name");
	GetWindowText(YourMessage, Message, 100);
	EnableWindow(YourMessage, false);
	EnableWindow(Send, false);
		
	
	//Ends drawing of Dots and Lines
	int n = 0;
	while(GetMessage(&msg, NULL, 0, 0))
	{
				
		if(KEYDOWN(VK_RETURN))
			SendMessage(hwnd,WM_COMMAND,2200,0);
		if(KEYDOWN('P'))
			n = 1;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(msg.wParam);
}

void BoxHighlight(int x, int y, Player* p)
{
	static int r = 0, c = 0;
	for(int row = 0; row < 9; row++)
	{
			if(!boxes[row]->played)
			{
				if (boxes[row]->left < x)
				{
					if (boxes[row]->top < y)
					{
						if (boxes[row]->right > x)	  
						{
							if (boxes[row]->bottom > y)
							{
								if(p->isX)
									FillX(boxes[row], p);
								else
									FillO(boxes[row], p);
							}
							else
							{
								FillBox(boxes[row], BLACKCOLOR);
							}
						}
						else
						{
							FillBox(boxes[row], BLACKCOLOR);
						}
					}
					else
					{
						FillBox(boxes[row], BLACKCOLOR);
					}
				}
				else
				{			
					FillBox(boxes[row], BLACKCOLOR);

				}
			}
		
	}
}
bool BoxSelect(int x, int y, Player* p)
{
	for (int i = 0; i < 9; i++)
	{
		if(!boxes[i]->played)
		{
			if(x > boxes[i]->left)
				if(y < boxes[i]->bottom)
					if(x < boxes[i]->right)
						if( y > boxes[i]->top)
						{
							if(p->isX)
								FillX(boxes[i], p);
							else
								FillO(boxes[i], p);
							boxes[i]->played=true;
							boxes[i]->p = p;
							if(winTest(p))
							{
								p->score++;
								sprintf(Buffer, "Congratulations %s is the winner", p->Name);
								MessageBox(gHWND, Buffer, "Winner", 0);
								sprintf(Buffer, "Current Score:\n%s:\t%d\n%s:\t%d", Server.Name, Server.score, Client.Name, Client.score);
								MessageBox(gHWND, Buffer, "Winner", 0);
								restart();
							}
							return true;
						}
					
		}
	}
	return false;
}
void SendSelect(int x, int y)
{
	for(int i = 0; i < 4; i++)
	{
		Buffer2[i] = '/0';
	}
	sprintf(Buffer2, "M");
	send(skServer, Buffer2, sizeof(Buffer2),0);
	sprintf(Buffer2, "%i", x);
	send(skServer, Buffer2, sizeof(Buffer2),0);
	sprintf(Buffer2, "%i", y);
	send(skServer, Buffer2, sizeof(Buffer2),0);
}
void FillX(box* b, Player* p)
{
	HPEN hpen=CreatePen(PS_SOLID, 10,  p->c );
	HBRUSH hbrush=CreateSolidBrush(BLACKCOLOR);
	HPEN old_hpen = (HPEN)SelectObject(gHDC, hpen);
	HBRUSH old_hbrush = (HBRUSH)SelectObject(gHDC, hbrush);
	MoveToEx(gHDC, b->left + 20, b->top + 20, 0);
	LineTo(gHDC, b->right - 20, b->bottom - 20);
	MoveToEx(gHDC, b->right - 20, b->top + 20, 0);
	LineTo(gHDC, b->left + 20, b->bottom - 20);
	DeleteObject(hpen);
	(HPEN)SelectObject(gHDC, old_hpen);
	(HBRUSH)SelectObject(gHDC, old_hbrush);
	DeleteObject(hbrush);
}
void FillO(box* b, Player* p)
{
	HPEN hpen=CreatePen(PS_SOLID, 10,  p->c );
	HBRUSH hbrush=CreateSolidBrush(BLACKCOLOR);
	HPEN old_hpen = (HPEN)SelectObject(gHDC, hpen);
	HBRUSH old_hbrush = (HBRUSH)SelectObject(gHDC, hbrush);
	Ellipse(gHDC, b->left + 20, b->top + 20, b->right - 20, b->bottom - 20);
	DeleteObject(hpen);
	(HPEN)SelectObject(gHDC, old_hpen);
	(HBRUSH)SelectObject(gHDC, old_hbrush);
	DeleteObject(hbrush);
}
void FillBox(box* b, COLORREF c)
{
	RECT rect;
	HPEN hpen=CreatePen(PS_SOLID, 5,BLACKCOLOR);
	HBRUSH hbrush=CreateSolidBrush(BLACKCOLOR);
	HPEN old_hpen = (HPEN)SelectObject(gHDC, hpen);
	HBRUSH old_hbrush = (HBRUSH)SelectObject(gHDC, hbrush);
	rect.left = b->left + 10;
	rect.top = b->top + 10;
	rect.right = b->right - 10;
	rect.bottom = b->bottom - 10;
	FillRect(gHDC, &rect, hbrush);
	DeleteObject(hpen);
	(HPEN)SelectObject(gHDC, old_hpen);
	(HBRUSH)SelectObject(gHDC, old_hbrush);
	DeleteObject(hbrush);
}
bool Setup()
{
	
	SetWindowText(PortNumber, "2006");
	GetWindowText(PortNumber, Port, 5);
	EnableWindow(PortNumber, false);
	GetWindowText(UserName, UName, 10);
	SetWindowText(UserName, UName);
	GetWindowText(IPAddress, IP, 15);
	SetWindowText(IPAddress, IP);
	EnableWindow(IPAddress, true);
	EnableWindow(YourMessage, false);
	EnableWindow(Send, false);
	/**************************************************************************
	SETUP SOCKET VARIABLES AN OBJECTS
	**************************************************************************/

	iResult = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (iResult != NO_ERROR)
	{
		sprintf(Buffer, "Error with WSAStartup()\n");
		MessageBox(0,Buffer, 0, 0);
		return false;
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
		MessageBox(0,Buffer, 0, 0);
		WSACleanup();
		getch();
		return false;
	}
	else
	{
		vShowText(listBox, "SOCKET:\topen...\n");
		vShowText(listBox, "SERVER TYPE:\tTCP...\n\n");
	}

	if(WSAAsyncSelect(skServer, gHWND, WM_SERVER, FD_CLOSE | FD_READ | FD_CONNECT) == 0)
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

	/**************************************************************************
	CONNECT TO SERVER FROM CLIENT
	**************************************************************************/
	iResult = connect(skServer, (struct sockaddr*)&saServer, sizeof(sockaddr));
	//if(iResult == SOCKET_ERROR)
	//{
	//	sprintf(Buffer, "Connections failed: %i\n", WSAGetLastError());
	//	MessageBox(0,Buffer, 0, 0);
	//	closesocket(skServer);
	//	WSACleanup();
	//	getch();
	//	return false;
	//}

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
void CreateButtons()
	
{
	listBox = CreateWindowEx(WS_EX_CLIENTEDGE, 
		"LISTBOX",
		NULL, 
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY,
		800, 200,
		400, 400,
		gHWND,
		NULL,
		gHINSTANCE,
		NULL);


	IPAddress = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL, WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL,
		800, 25,
		100, 21,
		gHWND,
		NULL,
		gHINSTANCE,
		NULL);

	PortNumber = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL, WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL,
		800, 50,
		100, 21,
		gHWND,
		NULL,
		gHINSTANCE,
		NULL);

	UserName = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL, WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL,
		800, 75,
		100, 21,
		gHWND,
		NULL,
		gHINSTANCE,
		NULL);

	YourMessage = CreateWindowEx(WS_EX_CLIENTEDGE,
		"EDIT",
		NULL, WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE,
		800, 625,
		400, 21,
		gHWND,
		NULL,
		gHINSTANCE,
		NULL);

	connectBind = CreateWindowEx(WS_EX_APPWINDOW, 
		"BUTTON", 
		"Connect/Bind",
		WS_CHILD | WS_VISIBLE,
		800, 125,
		100, 21,
		gHWND,
		(HMENU)(2000),
		gHINSTANCE, 
		NULL); 

	QuitExit = CreateWindowEx(WS_EX_APPWINDOW, 
		"BUTTON", 
		"Quit/Exit",
		WS_CHILD | WS_VISIBLE,
		800, 150,
		100, 21,
		gHWND,
		(HMENU)(2100),
		gHINSTANCE, 
		NULL); 

	Send = CreateWindowEx(WS_EX_APPWINDOW, 
		"BUTTON", 
		"Send",
		WS_CHILD | WS_VISIBLE,
		800, 650,
		400, 21,
		gHWND,
		(HMENU)(2200),
		gHINSTANCE, 
		NULL);

	SetWindowText(IPAddress, "IP Address");
	SetWindowText(PortNumber, "Port Number");
	//EnableWindow(IPAddress, false);
	//EnableWindow(connectBind, false);
	EnableWindow(PortNumber, false);
	SetWindowText(UserName, "Name");
	GetWindowText(YourMessage, Message, 100);
	//	EnableWindow(YourMessage, false);
	//EnableWindow(Send, false);
}
bool winTest(Player* p)
{
	//0 1 2
	//3 4 5
	//6 7 8
	//top row, left col, TL->BR
	if(boxes[0]->played && boxes[0]->p == p)
	{
		//0 1 3
		//
		//
		if(boxes[1]->played && boxes[1]->p == p)
		{
			if(boxes[2]->played && boxes[2]->p == p)
			{
				//Game Over
				return true;
			}
		}
		//0
		//3
		//5
		if(boxes[3]->played && boxes[3]->p == p)
		{
			if(boxes[5]->played && boxes[5]->p == p)
			{
				//Game Over
				return true;
			}
		}
		//0
		//  4
		//    8
		if(boxes[4]->played && boxes[4]->p == p)
		{
			if(boxes[8]->played && boxes[8]->p == p)
			{
				//Game Over
				return true;
			}
		}
	}
	//mid col
	if(boxes[1]->played && boxes[1]->p == p)
	{
		//  1
		//  4
		//  7
		if(boxes[4]->played && boxes[4]->p == p)
		{
			if(boxes[7]->played && boxes[7]->p == p)
			{
				//Game Over
				return true;
			}
		}
	}
	//right col TR-BL diagonal
	if(boxes[2]->played && boxes[2]->p == p)
	{
		//    2
		//    5
		//    8
		if(boxes[5]->played && boxes[5]->p == p)
		{
			if(boxes[8]->played && boxes[8]->p == p)
			{
				//Game Over
				return true;
			}
		}
		//    2
		//  4
		//6
		if(boxes[4]->played && boxes[4]->p == p)
		{
			if(boxes[6]->played && boxes[4]->p == p)
			{
				//Game Over
				return true;
			}
		}
	}
	//second row
	if(boxes[3]->played && boxes[3]->p == p)
	{
		//
		//3  4  5
		//
		if(boxes[4]->played && boxes[4]->p == p)
		{
			if(boxes[5]->played && boxes[5]->p == p)
			{
				//Game Over
				return true;
			}
		}
	}
	//Third Row
	if(boxes[6]->played && boxes[6]->p == p)
	{
		//
		//
		//6  7  8
		if(boxes[7]->played && boxes[7]->p == p)
		{
			if(boxes[8]->played && boxes[8]->p == p)
			{
				//GameOver
				return true;
			}
		}
	}
	return false;
}
void restart()
{
	for (int i = 0; i < 9; i++)
	{
		boxes[i]->played = false;
		boxes[i]->p = &Highlighter;
	}
	FillBox(new box(50, 750, 50, 750), BLACKCOLOR);
	MoveToEx(gHDC, 300, 100, 0);//FirstVertLineTop
	LineTo(gHDC, 300, 700);//FirstVertLineBottom
	MoveToEx(gHDC, 500, 100, 0);//SecondVertLineTop
	LineTo(gHDC, 500, 700);//SecondVertLineBottom
	MoveToEx(gHDC, 100, 300, 0);//FirstHorizLineLeft
	LineTo(gHDC, 700, 300);//FirstHorizLineRight
	MoveToEx(gHDC, 100, 500, 0);//SecondHorizLineLeft
	LineTo(gHDC, 700, 500);//SecondHorizLineRight
}