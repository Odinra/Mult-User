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

//enum GameStates {ShipPlacing, Play, GameOver};
//GameStates gamestate = ShipPlacing;

bool bCarrier = false;//5
bool bBattleship = false;//4
bool bSubmarine = false;//3
bool bDestroyer = false;//3
bool bPatrol= false;//1
bool horriz = true;

using namespace std;

#define WINDOW_CLASS_NAME "WINCLASS1"
#define BUTTON_BASE_ID1 100
#define WM_SERVER (WM_USER + 1008)
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define SERVERCOLOR RGB(255, 0, 0)
#define CLIENTCOLOR RGB(0, 0, 255)
#define SHIPCOLOR RGB(0,0,255)
#define HITCOLOR RGB(255,0,0)
#define MISSCOLOR RGB(255,255,255)

char num[5];
int Coordsx[22][22];
int Coordsx2[22][22];
int Coordsy[22][22];
int Coordsy2[22][22];

int p1Score = 0, p2Score = 0;

int recvX, recvY;
bool isTurnClient = false;
bool isTurnServer = true;

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
};

struct lines
{
	int startx, starty, endx, endy;
	bool played;
	lines( int x, int y, int X, int Y)
	{
		startx = x; starty = y; endx = X; endy = Y;
		played = false;
	}

	lines(){}
};

struct box
{
	//lines* top, *bottom, *left, *right;
	bool played;
	bool hit;
	bool ship;
	int playerOwned;
	char nameOwned;
	int top, left, bottom, right;
	box(int t,int b,int l,int r)
	{
		top = t; bottom = b; left = l, right = r;
		played = false;
		ship = false;
		hit = false;
	}
	bool hasShip()
    {
		if (ship)
			return true;

		return false;
    }

	box(){}
};

Player Server, Client;
box *boxes[22][22];

HPEN red = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
HPEN blue = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));

HDC gHDC;
HWND gHWND;
HINSTANCE gHINSTANCE;
int ServerHitsLeft = 17;
int ClientHitsLeft = 17;

int ShipSize();
void ShipPlacement();
void ShipHighlight(int x, int y);
bool MakeGuessPlayer(int , int , COLORREF );
bool BoxSelectPlayer(int , int , COLORREF );
bool BoxSelectOther(int , int , COLORREF );
void DrawGrid1(int, int, int, int, int, int, int, int);
void DrawGrid2(int, int, int, int, int, int, int, int);
void LineHighlight(int, int);
void BoxHighlight(int, int);
void SendSelect(int, int);
bool LineSelect(int, int, COLORREF);
void FillBox(int, int, int, int, COLORREF);
bool Setup();
void vShowText(HWND hChildHandle, char *szText);
void CreateButtons();
bool BoxTest(Player*);
bool ready = false;
void MissOther(int y, int x, COLORREF color);
void HitOther(int y, int x, COLORREF color);

bool placeShips = true;

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
			//if(placeShips)
			//	ShipHighlight(mouse_x, mouse_y);
			//else
			//	BoxHighlight(mouse_x, mouse_y);
			int buttons = (int)wparam;
		}break;
	case WM_LBUTTONDOWN:
		{
			int mouse_x = (int)LOWORD(lparam);
			int mouse_y = (int)HIWORD(lparam);
			if(placeShips)
				BoxSelectPlayer(mouse_x, mouse_y, SERVERCOLOR);
			if(Server.turn)
			{
				if(!placeShips)
				if( MakeGuessPlayer(mouse_x, mouse_y, SERVERCOLOR))
				{
					SendSelect(mouse_x, mouse_y);
				
					Server.turn = false;
					Client.turn = true;
				}
			}
		}break;
	case WM_RBUTTONDOWN:
		{
			int mouse_x = (int)LOWORD(lparam);
			int mouse_y = (int)HIWORD(lparam);
			if(horriz)
				horriz = false;
			else
				horriz = true;
		}
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
				for(int i = 0; i < 80; i++)
				{
					RecvData[i] = '\0';
				}
				if(!ready)
				{
					recv(skClient, Client.Name, sizeof(Client.Name), 0);
					ready = true;
					Sleep(500);			
					Server.Name[0] = UName[0];
					send(skClient, Server.Name, sizeof(Server.Name), 0);
				}
				else
				{
				RecvBytes = recv(skClient, RecvData, sizeof(RecvData), 0);
				//vShowText(listBox, RecvData);
				if(RecvData[0] == 'G')
				{
					Sleep(500);
					RecvBytes = recv(skClient, Buffer2, sizeof(Buffer2), 0);
					recvX = atoi(Buffer2);
					//vShowText(listBox, Buffer2);
					Sleep(500);
					RecvBytes = recv(skClient, Buffer2, sizeof(Buffer2), 0);
					recvY = atoi(Buffer2);
					//vShowText(listBox, Buffer2);

					//LineSelect(recvX, recvY, CLIENTCOLOR);
					BoxSelectOther(recvY, recvX, MISSCOLOR);

					Client.turn = false;
					Server.turn = true;
				}
				else if(RecvData[0] == 'H')
				{
					Sleep(500);
					RecvBytes = recv(skClient, Buffer2, sizeof(Buffer2), 0);
					recvX = atoi(Buffer2);
					//vShowText(listBox, Buffer2);
					Sleep(500);
					RecvBytes = recv(skClient, Buffer2, sizeof(Buffer2), 0);
					recvY = atoi(Buffer2);
					//vShowText(listBox, Buffer2);

					//LineSelect(recvX, recvY, CLIENTCOLOR);
					HitOther(recvX, recvY, HITCOLOR);

					Client.turn = true;
					Server.turn = false;
				}
				else if(RecvData[0] == 'M')
				{
					Sleep(500);
					RecvBytes = recv(skClient, Buffer2, sizeof(Buffer2), 0);
					recvX = atoi(Buffer2);
					//vShowText(listBox, Buffer2);
					Sleep(500);
					RecvBytes = recv(skClient, Buffer2, sizeof(Buffer2), 0);
					recvY = atoi(Buffer2);
					//vShowText(listBox, Buffer2);

					//LineSelect(recvX, recvY, CLIENTCOLOR);
					MissOther(recvX, recvY, MISSCOLOR);

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


	if(!(hwnd = CreateWindowEx(NULL, WINDOW_CLASS_NAME, "BattleShip - SERVER", WS_OVERLAPPED|WS_SYSMENU|WS_VISIBLE,
		0, 0, 1280, 1024, NULL, NULL, hinstance, NULL)))
		return (0);


	hdc = GetDC(hwnd);
	gHDC = hdc;
	gHWND = hwnd;
	gHINSTANCE = hinstance;

	CreateButtons();
	HBRUSH hbrush = CreateSolidBrush(RGB(0, 255, 0));
	HPEN white = CreatePen(PS_SOLID, 1, RGB(125, 125, 125));
	HPEN green = CreatePen(PS_SOLID, 4, RGB(0, 125, 0));
	HPEN old_hpen = (HPEN)SelectObject(gHDC, green);

	int x = 12.5;//1
	int y = 17.5;//2
	int i = 15;//3
	int j = 10;//4
	int i2 = 20;//5
	int j2 = 15;//8
	int stop1 = 440;//6
	int stop2 = 450;//7
	int num1 = 12.5;
	int num2 = 452.5;
	int num3 = 15;
	int num4 = 10;
	int num5 = 455;
	int num8 = 450;
	int num6 = 450;
	int num7 = 880;

	Server.c = SERVERCOLOR;
	Server.score = 0;
	Server.turn = true;
	Client.c = CLIENTCOLOR;
	Client.score = 0;
	Client.turn = false;

	//Draws Grid
	DrawGrid1(x, y, i, j, i2, stop1, stop2, j2);
	DrawGrid2(num1, num2, num3, num4, num5, num6, num7, num8);

	GetWindowText(YourMessage, Message, 100);

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

bool BoxTest(Player* currPlayer)
{
	//
	//for (int x = 0; x< 10; x++)
	//{
	//	for ( int y = 0; y < 10; y++)
	//	{
	//		if(!boxes[x][y]->played)
	//		{
	//			if( boxes[x][y]->hasShip())
	//			{	
	//				//FillBox(boxes[x][y]->top, boxes[x][y]->left, boxes[x][y]->bottom, boxes[x][y]->right, currPlayer->c);
	//				currPlayer->turn= true;
	//				currPlayer->score++;
	//				char things[11] = {'a','b','c','d','e','f','g'};
	//				TextOut(gHDC, boxes[x][y]->top + 10, boxes[x][y]->left + 10, &things[3], sizeof(currPlayer->Name));
	//				
	//				if ( currPlayer->score >= 239)
	//				{
	//					sprintf(Buffer, "%c Has won the game!\nServer Score: %i\nClient Score:%i", currPlayer->Name, Server.score, Client.score);

	//					::MessageBoxA(0, Buffer, "Results", 0);
	//				}
	//				return true;
	//			}
	//		}
	//	}
	//}

	return false;
}

void SendSelect(int x, int y)
{
	for(int i = 0; i < 4; i++)
	{
		Buffer2[i] = '/0';
	}
	sprintf(Buffer2, "G");
	send(skClient, Buffer2, sizeof(Buffer2),0);
	sprintf(Buffer2, "%i", x);
	send(skClient, Buffer2, sizeof(Buffer2),0);
	sprintf(Buffer2, "%i", y);
	send(skClient, Buffer2, sizeof(Buffer2),0);
}
void HitOther(int y, int x, COLORREF color)
{
		bool things;
		static int r = 0, c = 0;
		for(int row = 0; row < 10; row++)
		{
				for (int col = 0; col < 10; col++)
			{
				if(!boxes[row][col]->played)
					if (boxes[row][col]->left < y)
						if (boxes[row][col]->top < x)
							if (boxes[row][col]->right > y)	   
								if (boxes[row][col]->bottom > x)
								{
									
										r = row;
										c = col;
										FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5, boxes[r][c]->right - 5, boxes[r][c]->bottom + 5, HITCOLOR);
										boxes[row][col]->played = true;
										boxes[row][col]->hit = true;
										boxes[row][col]->ship = true;
										ClientHitsLeft--;
										if(ClientHitsLeft == 0)
											MessageBox(0, "Game Over! \nYou are Victorious in Naval Combat this day!", "GameOver", MB_OK);
									
								}
				}
		}
}
void MissOther(int y, int x, COLORREF color)
{
		bool things;
		static int r = 0, c = 0;
		for(int row = 0; row < 10; row++)
		{
				for (int col = 0; col < 10; col++)
			{
				if(!boxes[row][col]->played)
					if (boxes[row][col]->left < y)
						if (boxes[row][col]->top < x)
							if (boxes[row][col]->right > y)	   
								if (boxes[row][col]->bottom > x)
								{
									
										r = row;
										c = col;
										FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5, boxes[r][c]->right - 5, boxes[r][c]->bottom + 5, MISSCOLOR);
										boxes[row][col]->played = true;
									
								}
				}
		}
}
bool MakeGuessPlayer(int y, int x, COLORREF color)
{
	{
		bool things;
		static int r = 0, c = 0;
		for(int row = 0; row < 10; row++)
		{
			for (int col = 0; col < 10; col++)
			{
				if(!boxes[row][col]->played)
					if (boxes[row][col]->left < y)
						if (boxes[row][col]->top < x)
							if (boxes[row][col]->right > y)	   
								if (boxes[row][col]->bottom > x)
								{
									SendSelect(y, x);
									/*if(boxes[row+11][col+11]->ship)
									{
										r = row;
										c = col;
										FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5, boxes[r][c]->right - 5, boxes[r][c]->bottom + 5, HITCOLOR);
										boxes[row][col]->played = true;
										boxes[row][col]->hit = true;
										ClientHitsLeft--;
										if(ClientHitsLeft == 0)
											MessageBox(0, "Game Over! \nYou are Victorious in Naval Combat this day!", "GameOver", MB_OK);
									}
									else
									{
										r = row;
										c = col;
										FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5, boxes[r][c]->right - 5, boxes[r][c]->bottom + 5, MISSCOLOR);
										boxes[row][col]->played = true;
									}*/

								}
			}
		}

		return false;
	}
}
bool Filler(int r, int c)
{
	//FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5, boxes[r][c]->right - 5, boxes[r][c]->bottom + 5, SHIPCOLOR);
	if(horriz)
	{
		int s = ShipSize();
		if(r - 11 + s < 11)
		{
			if(s == 5)
			{
				if(!(!boxes[r][c]->ship && !boxes[r + 1][c]->ship && !boxes[r + 2][c]->ship && !boxes[r + 3][c]->ship && !boxes[r + 4][c]->ship))
				{
					MessageBox(0, "Invalid Move", "Error", MB_OK);
					return false;
				}
			}
			if(s == 4)
			{
				if(!(!boxes[r][c]->ship && !boxes[r + 1][c]->ship && !boxes[r + 2][c]->ship && !boxes[r + 3][c]->ship))
				{
					MessageBox(0, "Invalid Move", "Error", MB_OK);
					return false;
				}
			}
			if(s == 3)
			{
				if(!(!boxes[r][c]->ship && !boxes[r + 1][c]->ship && !boxes[r + 2][c]->ship))
				{
					MessageBox(0, "Invalid Move", "Error", MB_OK);
					return false;
				}
			}
			if(s == 2)
			{
				if(!(!boxes[r][c]->ship && !boxes[r + 1][c]->ship))
				{
					MessageBox(0, "Invalid Move", "Error", MB_OK);
					return false;
				}
			}
			for(int size = ShipSize(); size > 0; size--)
			{
				if(r - 10 < 10 - size + 1)
				{
					FillBox(boxes[r][c]->left - 5 + ((size-1) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-1) * 40), boxes[r][c]->bottom + 5, SHIPCOLOR);
					boxes[r + size-1][c]->ship = true;
				}
				else if(r - 10 < 10 - ShipSize() + 2)
				{
					FillBox(boxes[r][c]->left - 5 + ((size-1) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-1) * 40), boxes[r][c]->bottom + 5, SHIPCOLOR);
					boxes[r + size-2][c]->ship = true;
					for (int size2 = 1; size2 < 2; size2++)
					{
						//FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5,SHIPCOLOR);
						boxes[r - size2][c]->ship = true;
					}
				}
				else if(r - 10 < 10 - ShipSize() + 3)
				{
					FillBox(boxes[r][c]->left - 5 + ((size-3) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-3) * 40), boxes[r][c]->bottom + 5,SHIPCOLOR);
					boxes[r + size-3][c]->ship = true;
					for (int size2 = 1; size2 < 3; size2++)
					{
						FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, SHIPCOLOR);
						boxes[r - size2][c]->ship = true;
					}
				}
				else if(r - 10 < 10 - ShipSize() + 4)
				{
					FillBox(boxes[r][c]->left - 5 + ((size-4) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-4) * 40), boxes[r][c]->bottom + 5, SHIPCOLOR);
					boxes[r + size-4][c]->ship = true;
					for (int size2 = 1; size2 < 4; size2++)
					{
						FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5,SHIPCOLOR);
						boxes[r - size2][c]->ship = true;
					}
				}
				else if(r - 10 < 10 - ShipSize() + 5)
				{
					FillBox(boxes[r][c]->left - 5 + ((size-5) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-5) * 40), boxes[r][c]->bottom + 5, SHIPCOLOR);
					boxes[r + size-5][c]->ship = true;
					for (int size2 = 1; size2 < 5; size2++)
					{
						FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, SHIPCOLOR);
						boxes[r - size2][c]->ship = true;
					}
				}
				boxes[r][c]->ship = true;

			}return true;
		}
		else
		{
			MessageBox(0, "Invalid Move", "Error", MB_OK);
			return false;
		}
	}

	else 
	{
		int s = ShipSize();
		if(c -11 + s < 11)
		{
			if(s == 5)
			{
				if(!(!boxes[r][c]->ship && !boxes[r][c+1]->ship && !boxes[r][c + 2]->ship && !boxes[r][c + 3]->ship && !boxes[r][c + 4]->ship))
				{
					MessageBox(0, "Invalid Move", "Error", MB_OK);
					return false;
				}
			}
			if(s == 4)
			{
				if(!(!boxes[r][c]->ship && !boxes[r][c + 1]->ship && !boxes[r][c + 2]->ship && !boxes[r][c + 3]->ship))
				{
					MessageBox(0, "Invalid Move", "Error", MB_OK);
					return false;
				}
			}
			if(s == 3)
			{
				if(!(!boxes[r][c]->ship && !boxes[r ][c+ 1]->ship && !boxes[r][c + 2]->ship))
				{
					MessageBox(0, "Invalid Move", "Error", MB_OK);
					return false;
				}
			}
			if(s == 2)
			{
				if(!(!boxes[r][c]->ship && !boxes[r][c + 1]->ship))
				{
					MessageBox(0, "Invalid Move", "Error", MB_OK);
					return false;
				}
			}
			for(int size = ShipSize(); size > 0; size--)
			{
				if(c - 10 < 10 - size + 1)
				{
					FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5 + ((size-1) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-1) * 40), SHIPCOLOR);
					boxes[r][c + size-1]->ship = true;
				}
				else if(c-10 < 10 - ShipSize() + 2)
				{
					FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-2) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-2) * 40), SHIPCOLOR);
					boxes[r][c + size-1]->ship = true;
					//for (int size2 = 1; size2 < 2; size2++)
					//{
					////	FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), SHIPCOLOR);
					//	boxes[r ][c- size2-2]->ship = true;
					//}
				}
				else if(c-10 < 10 - ShipSize() + 3)
				{
					FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-3) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-3) * 40), SHIPCOLOR);
					boxes[r][c + size]->ship = true;
					for (int size2 = 1; size2 < 3; size2++)
					{
						FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 - ((size2) * 40), SHIPCOLOR);
						boxes[r][c - size2-3]->ship = true;
					}
				}
				else if(c-10 < 10 - ShipSize() + 4)
				{
					FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-4) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5+ ((size-4) * 40), SHIPCOLOR);
					boxes[r][c + size]->ship = true;
					for (int size2 = 1; size2 < 4; size2++)
					{
						FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), SHIPCOLOR);
						boxes[r ][c- size2-4]->ship = true;
					}
				}
				else if(c-10 < 10 - ShipSize() + 5)
				{
					FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-5) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5+ ((size-5) * 40), SHIPCOLOR);
					boxes[r][c + size]->ship = true;
					for (int size2 = 1; size2 < 5; size2++)
					{
						FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), SHIPCOLOR);
						boxes[r][c - size2-5]->ship = true;
					}
				}
				boxes[r][c]->ship = true;

			}return true;
		}
		else
		{
			MessageBox(0, "Invalid Move", "Error", MB_OK);
			return false;
		}
	}
}
bool BoxSelectPlayer(int y, int x, COLORREF color)
{
	bool things;
	static int r = 0, c = 0;
	int s = ShipSize();
	for(int row = 10; row < 21; row++)
	{
		for (int col = 10; col < 21; col++)
		{
			if(!boxes[row][col]->ship)
				if (boxes[row][col]->left < y)
					if (boxes[row][col]->top < x)
						if (boxes[row][col]->right > y)	   
							if (boxes[row][col]->bottom > x)
							{
								for (int i = 0; i < s; i++)
								{
									if(horriz)
									{
										
										if(!boxes[row][col]->ship)
												{
													r = row;
													c = col;
													things = Filler(r, c);
													if(things)
													{
													if(!bCarrier)
													{
														bCarrier = true;
														return things;
													}
													else if(!bBattleship)
													{
														bBattleship = true;
														return things;
													}
													else if(!bSubmarine)
													{
														bSubmarine = true;
														return things;
													}
													else if(!bDestroyer)
													{
														bDestroyer = true;
														return things;
													}
													else if(!bPatrol)
													{
														bPatrol = true;
														EnableWindow(connectBind, true);
														return things;
													}
													}
												
										}
										else
										{
											MessageBox(0, "Invalid Move", "error", MB_OK);
										}
									}
									else
									{
										if(!boxes[row][col]->ship)
										{
										r = row;
											c = col;
											things = Filler(r, c);
											if(things)
											{
											if(!bCarrier)
											{
												bCarrier = true;
												return things;
											}
											else if(!bBattleship)
											{
												bBattleship = true;
												return things;
											}
											else if(!bSubmarine)
											{
												bSubmarine = true;
												return things;
											}
											else if(!bDestroyer)
											{
												bDestroyer = true;
												return things;
											}
											else if(!bPatrol)
											{
												bPatrol = true;
												EnableWindow(connectBind, true);
												return things;
											}
											}
										}
										else
										{
											MessageBox(0, "Invalid Move", "error", MB_OK);
										}
									}
								}

								//if(boxes[row][col]->ship)
								//{
								//	FillBox(boxes[row][col]->left, boxes[row][col]->top, boxes[row][col]->right, boxes[row][col]->bottom, HITCOLOR);
								//	
								//}
								//else
								//{
								//	
								//	
								//}
								/*FillBox(boxes[row][col]->top, boxes[row][col]->left, boxes[row][col]->bottom, boxes[row][col]->right, SHIPCOLOR);*/


							}
		}
	}

	return false;
}
bool BoxSelectOther(int x, int y, COLORREF color)
{
	{
	{
		bool things;
		static int r = 0, c = 0;
		for(int row = 0; row < 10; row++)
		{
			for (int col = 0; col < 10; col++)
			{
				if(!boxes[row][col]->played)
					if (boxes[row][col]->left < y)
						if (boxes[row][col]->top < x)
							if (boxes[row][col]->right > y)	   
								if (boxes[row][col]->bottom > x)
								{
									//SendSelect(y, x);
									if(boxes[row+11][col+11]->ship)
									{
										r = row;
										c = col;
										FillBox(boxes[r+11][c+11]->left - 5, boxes[r+11][c+11]->top + 5, boxes[r+11][c+11]->right - 5, boxes[r+11][c+11]->bottom + 5, HITCOLOR);
										boxes[row+11][col+11]->played = true;
										boxes[row+11][col+11]->hit = true;
										ServerHitsLeft--;
										if(ServerHitsLeft == 0)
										MessageBox(0, "Game Over! \nYou have lost at Naval Combat this day!", "GameOver", MB_OK);
										for(int i = 0; i < 4; i++)
										{
											Buffer2[i] = '/0';
										}
										sprintf(Buffer2, "H");
										send(skClient, Buffer2, sizeof(Buffer2),0);
										sprintf(Buffer2, "%i", y);
										send(skClient, Buffer2, sizeof(Buffer2),0);
										sprintf(Buffer2, "%i", x);
										send(skClient, Buffer2, sizeof(Buffer2),0);
										return true;
									}
									else
									{
										r = row;
										c = col;
										FillBox(boxes[r+11][c+11]->left - 5, boxes[r+11][c+11]->top + 5, boxes[r+11][c+11]->right - 5, boxes[r+11][c+11]->bottom + 5, MISSCOLOR);
										boxes[row+11][col+11]->played = true;
										for(int i = 0; i < 4; i++)
										{
											Buffer2[i] = '/0';
										}
										sprintf(Buffer2, "M");
										send(skClient, Buffer2, sizeof(Buffer2),0);
										sprintf(Buffer2, "%i", y);
										send(skClient, Buffer2, sizeof(Buffer2),0);
										sprintf(Buffer2, "%i", x);
										send(skClient, Buffer2, sizeof(Buffer2),0);
									}

								}
			}
		}

		return false;
	}
}}

void FillBox(int Sx, int Sy, int Ex, int Ey, COLORREF c)
{
	/*MoveToEx(gHDC, Sx, Sy, 0);*/
	RECT rect;
	//coordsx = left
	//coordsx2 = top
	//coordsy = right
	//coordsy2 = bottom
	HBRUSH hbrush=CreateSolidBrush(c);
	rect.left = Sx;
	rect.top = Sy;
	rect.right = Ex;
	rect.bottom = Ey;
	FillRect(gHDC, &rect, hbrush);
	DeleteObject(hbrush);
}

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
	
	if(WSAAsyncSelect(skServer, gHWND, WM_SERVER, FD_ACCEPT|FD_CLOSE | FD_READ | FD_CONNECT) == 0)
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
	EnableWindow(IPAddress, false);
	EnableWindow(connectBind, false);
	EnableWindow(PortNumber, false);
	SetWindowText(UserName, "Name");
	GetWindowText(YourMessage, Message, 100);
	//	EnableWindow(YourMessage, false);
	//EnableWindow(Send, false);
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

void DrawGrid1(int num1, int num2, int num3, int num4, int num5, int num6, int num7, int num8)
{
	HBRUSH hbrush = CreateSolidBrush(RGB(0, 255, 0));
	HPEN white = CreatePen(PS_SOLID, 1, RGB(125, 125, 125));
	HPEN green = CreatePen(PS_SOLID, 4, RGB(0, 125, 0));
	HPEN old_hpen = (HPEN)SelectObject(gHDC, green);

	int x = 0;
	int y = 0;

	MoveToEx(gHDC, num1, num2, 0);
	for(int i = num3, j = num4; i < num6; i= i + 40, j = j + 40)
	{

		for(int i2 = num5, j2 = num8; i2 < num7; i2= i2 + 40, j2 = j2 + 40)
		{

			//i = left = Coordsx
			//i2 = top = Coordsx2
			//j = right = Coordsy
			//js = bottom = Coordsy2
			old_hpen = (HPEN)SelectObject(gHDC, red);
			/*Rectangle(gHDC, i, i2, j, j2);*/

			Coordsx2[x][y] = i2;
			Coordsx[x][y]  = i;
			Coordsy[x][y]  = j;
			Coordsy2[x][y]  = j2;
			old_hpen = (HPEN)SelectObject(gHDC, white);
			LineTo(gHDC, (i + j) /2, (i2 + j2) /2);
			y++;

		}
		y = 0;
		MoveToEx(gHDC, (Coordsx[x][y] + Coordsy[x][y]) / 2, (Coordsx2[x][y] +  Coordsy2[x][y]) /2, 0);
		x++;
	}

	        int numsX = 430;
        int numsY = 35;
    char numbers[10] = {'0', '1', '2', '3','4','5','6','7','8','9' };
        char letters[10] = {'A', 'B','C','D','E','F','G','H', 'I','J'};
        for(int nums = 0; nums < 10; nums++)
        {
                TextOut(gHDC, numsX, numsY, (LPCSTR)&numbers[nums], 1);
                numsY+=40;
        }
        int letX = 35;
        int letY = 430;
        for(int nums = 0; nums < 10; nums++)
        {
                TextOut(gHDC, letX, letY, (LPCSTR)&letters[nums], 1);
                letX+=40;
        }


	MoveToEx(gHDC, num1, num2, 0);
	for (int y = 0; y < 11; y++ )
	{
		for (x = 0; x < 11; x++)
		{
			old_hpen = (HPEN)SelectObject(gHDC, white);
			LineTo(gHDC, (Coordsx[x][y] + Coordsy[x][y]) / 2, (Coordsx2[x][y] +  Coordsy2[x][y]) /2);
		}
		x = 0;
		MoveToEx(gHDC, (Coordsx[x][y] + Coordsy[x][y]) / 2, (Coordsx2[x][y] +  Coordsy2[x][y]) /2, 0);
	}

	//box(top, bottom, left, right);
	for( int x = 0; x < 10; x++)
	{
		for(int y = 0; y < 10; y++)
		{
			boxes[x][y] = new box(Coordsx2[x][y] - 5, Coordsy2[x][y+1] - 5, Coordsx[x][y]+5 , Coordsy[x+1][y]+	5); 
		}
	}
}

void DrawGrid2(int num1, int num2, int num3, int num4, int num5, int num6, int num7, int num8)
{
	HBRUSH hbrush = CreateSolidBrush(RGB(0, 255, 0));
	HPEN white = CreatePen(PS_SOLID, 1, RGB(125, 125, 125));
	HPEN green = CreatePen(PS_SOLID, 4, RGB(0, 125, 0));
	HPEN old_hpen = (HPEN)SelectObject(gHDC, green);

	int x = 11;
	int y = 11;

	MoveToEx(gHDC, num1, num2, 0);
	for(int i = num3, j = num4; i < num6; i= i + 40, j = j + 40)
	{

		for(int i2 = num5, j2 = num8; i2 < num7; i2= i2 + 40, j2 = j2 + 40)
		{

			//i = left
			//i2 = top
			//j = right
			//js = bottom
			old_hpen = (HPEN)SelectObject(gHDC, red);
			/*Rectangle(gHDC, i, i2, j, j2);*/

			Coordsx2[x][y] = i2;
			Coordsx[x][y]  = i;
			Coordsy[x][y]  = j;
			Coordsy2[x][y]  = j2;
			old_hpen = (HPEN)SelectObject(gHDC, white);
			LineTo(gHDC, (i + j) /2, (i2 + j2) /2);
			y++;

		}
		y = 11;
		MoveToEx(gHDC, (Coordsx[x][y] + Coordsy[x][y]) / 2, (Coordsx2[x][y] +  Coordsy2[x][y]) /2, 0);
		x++;
	}

	MoveToEx(gHDC, num1, num2, 0);
	for (int y = 11; y < 22; y++ )
	{
		for (x = 11; x < 22; x++)
		{
			old_hpen = (HPEN)SelectObject(gHDC, white);
			LineTo(gHDC, (Coordsx[x][y] + Coordsy[x][y]) / 2, (Coordsx2[x][y] +  Coordsy2[x][y]) /2);
		}
		x = 11;
		MoveToEx(gHDC, (Coordsx[x][y] + Coordsy[x][y]) / 2, (Coordsx2[x][y] +  Coordsy2[x][y]) /2, 0);
	}
	       
	char numbers2[10] = {'0', '1', '2', '3','4','5','6','7','8','9' };
    int numsX = 430;
    int numsY = 465;
    for(int nums = 0; nums < 10; nums++)
    {
            TextOut(gHDC, numsX, numsY, (LPCSTR)&numbers2[nums], 1);
            numsY+=40;
    }

	//box(top, bottom, left, right);
	for( int x = 10; x < 21; x++)
	{
		for(int y = 10; y < 21; y++)
		{
			boxes[x][y] = new box(Coordsx2[x][y] - 5, Coordsy2[x][y+1] - 5, Coordsx[x][y]+5 , Coordsy[x+1][y]+	5);  

		}
	}


}

int ShipSize()
{
	if(!bCarrier)
		return 5;
	if(!bBattleship)
		return 4;
	if(!bSubmarine)
		return 3;
	if(!bDestroyer)
		return 3;
	if(!bPatrol)
		return 2;
	
	placeShips = false;
	
	return 0;
}
////void BoxHighlight(int y, int x)
////{
////	static int r = 0, c = 0;
////	for(int row = 1; row < 10; row++)
////	{
////		for (int col = 1; col < 10; col++)
////		{
////			if(!boxes[row][col]->played)
////			{
////				if (boxes[row][col]->left < y)
////				{
////					if (boxes[row][col]->top < x)
////					{
////						if (boxes[row][col]->right > y)	  
////						{
////							if (boxes[row][col]->bottom > x)
////							{
////								r = row;
////								c = col;
////							}
////							else
////							{
////								FillBox(boxes[row][col]->left - 5, boxes[row][col]->top + 5, boxes[row][col]->right - 5, boxes[row][col]->bottom + 5, RGB(0,0,0));
////							}
////						}
////						else
////						{
////							FillBox(boxes[row][col]->left - 5, boxes[row][col]->top + 5, boxes[row][col]->right - 5, boxes[row][col]->bottom + 5, RGB(0,0,0));
////						}
////					}
////					else
////					{
////						FillBox(boxes[row][col]->left - 5, boxes[row][col]->top + 5, boxes[row][col]->right - 5, boxes[row][col]->bottom + 5, RGB(0,0,0));
////					}
////				}
////				else
////				{			
////					FillBox(boxes[row][col]->left - 5, boxes[row][col]->top + 5, boxes[row][col]->right - 5, boxes[row][col]->bottom + 5, RGB(0,0,0));
////
////				}
////			}
////		}
////	}
////	
////	if(horriz)
////	{
////		for(int size = ShipSize(); size > 0; size--)
////		{
////			if(r < 10 - size + 1)
////				FillBox(boxes[r][c]->left - 5 + ((size-1) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-1) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			else if(r < 10 - ShipSize() + 2)
////			{
////				FillBox(boxes[r][c]->left - 5 + ((size-2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////				for (int size2 = 1; size2 < 2; size2++)
////				FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			}
////			else if(r < 10 - ShipSize() + 3)
////			{
////				FillBox(boxes[r][c]->left - 5 + ((size-3) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-3) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////				for (int size2 = 1; size2 < 3; size2++)
////				FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			}
////			else if(r < 10 - ShipSize() + 4)
////			{
////				FillBox(boxes[r][c]->left - 5 + ((size-4) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-4) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////				for (int size2 = 1; size2 < 4; size2++)
////				FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			}
////			else if(r < 10 - ShipSize() + 5)
////			{
////				FillBox(boxes[r][c]->left - 5 + ((size-5) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-5) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////				for (int size2 = 1; size2 < 5; size2++)
////				FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			}
////		}
////	}
////
////	else 
////	{
////		for(int size = ShipSize(); size > 0; size--)
////		{
////			if(c < 10 - size + 1)
////				FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5 + ((size-1) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-1) * 40), RGB(255,242,0));
////			else if(c < 10 - ShipSize() + 2)
////			{
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-2) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-2) * 40), RGB(255,242,0));
////				for (int size2 = 1; size2 < 2; size2++)
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), RGB(255,242,0));
////			}
////			else if(c < 10 - ShipSize() + 3)
////			{
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-3) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-3) * 40), RGB(255,242,0));
////				for (int size2 = 1; size2 < 3; size2++)
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 - ((size2) * 40), RGB(255,242,0));
////			}
////			else if(c < 10 - ShipSize() + 4)
////			{
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-4) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5+ ((size-4) * 40), RGB(255,242,0));
////				for (int size2 = 1; size2 < 4; size2++)
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), RGB(255,242,0));
////			}
////			else if(c < 10 - ShipSize() + 5)
////			{
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-5) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5+ ((size-5) * 40), RGB(255,242,0));
////				for (int size2 = 1; size2 < 5; size2++)
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), RGB(255,242,0));
////			}
////		}
////	}
////	
////}
////
////void ShipHighlight(int y, int x)
////{
////	{
////	static int r = 0, c = 0;
////	for(int row = 0; row < 10; row++)
////	{
////		for (int col = 0; col < 10; col++)
////		{
////			if(!boxes[row][col]->ship)
////			{
////				if (boxes[row][col]->left < y)
////				{
////					if (boxes[row][col]->top < x)
////					{
////						if (boxes[row][col]->right > y)	  
////						{
////							if (boxes[row][col]->bottom > x)
////							{
////								r = row;
////								c = col;
////								//FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5, boxes[r][c]->right - 5, boxes[r][c]->bottom + 5, RGB(255,242,0));
////							}
////							else
////							{
////								FillBox(boxes[row][col]->left - 5, boxes[row][col]->top + 5, boxes[row][col]->right - 5, boxes[row][col]->bottom + 5, RGB(0,0,0));
////							}
////						}
////						else
////						{
////							FillBox(boxes[row][col]->left - 5, boxes[row][col]->top + 5, boxes[row][col]->right - 5, boxes[row][col]->bottom + 5, RGB(0,0,0));
////						}
////					}
////					else
////					{
////						FillBox(boxes[row][col]->left - 5, boxes[row][col]->top + 5, boxes[row][col]->right - 5, boxes[row][col]->bottom + 5, RGB(0,0,0));
////					}
////				}
////				else
////				{			
////					FillBox(boxes[row][col]->left - 5, boxes[row][col]->top + 5, boxes[row][col]->right - 5, boxes[row][col]->bottom + 5, RGB(0,0,0));
////
////				}
////			}
////		}
////	}
////	
////	if(horriz)
////	{
////		for(int size = ShipSize(); size > 0; size--)
////		{
////			if(r < 10 - size + 1)
////				FillBox(boxes[r][c]->left - 5 + ((size-1) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-1) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			else if(r < 10 - ShipSize() + 2)
////			{
////				FillBox(boxes[r][c]->left - 5 + ((size-2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////				for (int size2 = 1; size2 < 2; size2++)
////					FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			}
////			else if(r < 10 - ShipSize() + 3)
////			{
////				FillBox(boxes[r][c]->left - 5 + ((size-3) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-3) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////				for (int size2 = 1; size2 < 3; size2++)
////					FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			}
////			else if(r < 10 - ShipSize() + 4)
////			{
////				FillBox(boxes[r][c]->left - 5 + ((size-4) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-4) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////				for (int size2 = 1; size2 < 4; size2++)
////					FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			}
////			else if(r < 10 - ShipSize() + 5)
////			{
////				FillBox(boxes[r][c]->left - 5 + ((size-5) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 + ((size-5) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////				for (int size2 = 1; size2 < 5; size2++)
////					FillBox(boxes[r][c]->left - 5 - ((size2) * 40), boxes[r][c]->top + 5, boxes[r][c]->right - 5 - ((size2) * 40), boxes[r][c]->bottom + 5, RGB(255,242,0));
////			}
////		}
////	}
////
////	else 
////	{
////		for(int size = ShipSize(); size > 0; size--)
////		{
////			if(c < 10 - size + 1)
////				FillBox(boxes[r][c]->left - 5, boxes[r][c]->top + 5 + ((size-1) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-1) * 40), RGB(255,242,0));
////			else if(c < 10 - ShipSize() + 2)
////			{
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-2) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-2) * 40), RGB(255,242,0));
////				for (int size2 = 1; size2 < 2; size2++)
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), RGB(255,242,0));
////			}
////			else if(c < 10 - ShipSize() + 3)
////			{
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-3) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 + ((size-3) * 40), RGB(255,242,0));
////				for (int size2 = 1; size2 < 3; size2++)
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5, boxes[r][c]->bottom + 5 - ((size2) * 40), RGB(255,242,0));
////			}
////			else if(c < 10 - ShipSize() + 4)
////			{
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-4) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5+ ((size-4) * 40), RGB(255,242,0));
////				for (int size2 = 1; size2 < 4; size2++)
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), RGB(255,242,0));
////			}
////			else if(c < 10 - ShipSize() + 5)
////			{
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5+ ((size-5) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5+ ((size-5) * 40), RGB(255,242,0));
////				for (int size2 = 1; size2 < 5; size2++)
////				FillBox(boxes[r][c]->left - 5 , boxes[r][c]->top + 5- ((size2) * 40), boxes[r][c]->right - 5 , boxes[r][c]->bottom + 5- ((size2) * 40), RGB(255,242,0));
////			}
////		}
////	}
////	
////}
////}
//bool LineSelect(int x, int y, COLORREF color)
//{
//	for(int row = 0; row < 24; row++)
//	{
//		for(int col = 0; col < 24; col++)
//		{
//			//Select right line of box
//		if(!boxes[row][col]->right->played)
//			if(boxes[row][col]->right->startx <= x + 3 || boxes[row][col]->right->startx <= x - 3)
//				if(boxes[row][col]->right->endx >= x + 3 || boxes[row][col]->right->endx >= x - 3 )
//					if(boxes[row][col]->right->starty <= y + 3 || boxes[row][col]->right->starty <= y - 3 )	
//						if(boxes[row][col]->right->endy >= y + 3 || boxes[row][col]->right->endy >= y - 3  )
//						{
//							boxes[row][col]->right->played = true;
//							if(row < 23)
//							{
//								boxes[row + 1][col]->left->played = true;
//							}
//							
//							HPEN red = CreatePen(PS_SOLID, 3, color);
//							HPEN old_hpen = (HPEN)SelectObject(gHDC, red);
//							MoveToEx(gHDC, boxes[row][col]->right->startx, boxes[row][col]->right->starty, NULL);
//							LineTo(gHDC, boxes[row][col]->right->endx, boxes[row][col]->right->endy);
//							/*for(int i = 0; i < 4; i++)
//							{
//								Buffer2[i] = '/0';
//							}
//							sprintf(Buffer2, "M");
//							send(skClient, Buffer2, sizeof(Buffer2),0);
//							sprintf(Buffer2, "%i", x);
//							send(skClient, Buffer2, sizeof(Buffer2),0);
//							sprintf(Buffer2, "%i", y);
//							send(skClient, Buffer2, sizeof(Buffer2),0);*/
//
//							DeleteObject(red);
//							return true;
//						}
//						//Select bottom line of box
//		if(!boxes[row][col]->bottom->played)
//			if(boxes[row][col]->bottom->startx <= x + 3 || boxes[row][col]->bottom->startx <= x - 3)
//				if(boxes[row][col]->bottom->endx >= x + 3 || boxes[row][col]->bottom->endx >= x - 3 )
//					if(boxes[row][col]->bottom->starty <= y + 3 || boxes[row][col]->bottom->starty <= y - 3 )	
//						if(boxes[row][col]->bottom->endy >= y + 3 || boxes[row][col]->bottom->endy >= y - 3  )
//						{
//							boxes[row][col]->bottom->played = true;
//							if(col < 23)
//							{
//							boxes[row][col + 1]->top->played = true;
//							}
//							HPEN red = CreatePen(PS_SOLID, 3, color);
//							HPEN old_hpen = (HPEN)SelectObject(gHDC, red);
//							MoveToEx(gHDC, boxes[row][col]->bottom->startx, boxes[row][col]->bottom->starty, NULL);
//							LineTo(gHDC, boxes[row][col]->bottom->endx, boxes[row][col]->bottom->endy);
//							/*for(int i = 0; i < 4; i++)
//							{
//								Buffer2[i] = '/0';
//							}
//							sprintf(Buffer2, "M");
//							send(skClient, Buffer2, sizeof(Buffer2),0);
//							sprintf(Buffer2, "%i", x);
//							send(skClient, Buffer2, sizeof(Buffer2),0);
//							sprintf(Buffer2, "%i", y);
//							send(skClient, Buffer2, sizeof(Buffer2),0);*/
//							DeleteObject(red);
//							return true;
//						}
//						//Select left line of box
//		if(!boxes[row][col]->left->played)
//			if(boxes[row][col]->left->startx <= x + 3|| boxes[row][col]->left->startx <= x - 3)
//				if(boxes[row][col]->left->endx >= x + 3 || boxes[row][col]->left->endx >= x - 3 )
//					if(boxes[row][col]->left->starty <= y + 3 || boxes[row][col]->left->starty <= y - 3 )	
//						if(boxes[row][col]->left->endy >= y + 3 || boxes[row][col]->left->endy >= y - 3  )
//						{
//							boxes[row][col]->left->played = true;
//							HPEN red = CreatePen(PS_SOLID, 1, color);
//							HPEN old_hpen = (HPEN)SelectObject(gHDC, red);
//							MoveToEx(gHDC, boxes[row][col]->left->startx, boxes[row][col]->left->starty, NULL);
//							LineTo(gHDC, boxes[row][col]->left->endx, boxes[row][col]->left->endy);
//							/*for(int i = 0; i < 4; i++)
//							{
//								Buffer2[i] = '/0';
//							}
//							sprintf(Buffer2, "M");
//							send(skClient, Buffer2, sizeof(Buffer2),0);
//							sprintf(Buffer2, "%i", x);
//							send(skClient, Buffer2, sizeof(Buffer2),0);
//							sprintf(Buffer2, "%i", y);
//							send(skClient, Buffer2, sizeof(Buffer2),0);*/
//
//							DeleteObject(red);
//							return true;
//						}
//						//Select top line of box
//		if(!boxes[row][col]->top->played)
//			if(boxes[row][col]->top->startx <= x + 3 || boxes[row][col]->top->startx <= x - 3)
//				if(boxes[row][col]->top->endx >= x + 3 || boxes[row][col]->top->endx >= x - 3 )
//					if(boxes[row][col]->top->starty <= y + 3 || boxes[row][col]->top->starty <= y - 3 )	
//						if(boxes[row][col]->top->endy >= y + 3 || boxes[row][col]->top->endy >= y - 3  )
//						{
//							boxes[row][col]->top->played = true;
//							HPEN red = CreatePen(PS_SOLID, 1, color);
//							HPEN old_hpen = (HPEN)SelectObject(gHDC, red);
//							MoveToEx(gHDC, boxes[row][col]->top->startx, boxes[row][col]->top->starty, NULL);
//							LineTo(gHDC, boxes[row][col]->top->endx, boxes[row][col]->top->endy);
//							for(int i = 0; i < 4; i++)
//							{
//								Buffer2[i] = '/0';
//							}
//							/*sprintf(Buffer2, "M");
//							send(skClient, Buffer2, sizeof(Buffer2),0);
//							sprintf(Buffer2, "%i", x);
//							send(skClient, Buffer2, sizeof(Buffer2),0);
//							sprintf(Buffer2, "%i", y);
//							send(skClient, Buffer2, sizeof(Buffer2),0);*/
//
//							DeleteObject(red);
//							return true;
//						}
//
//		}
//	}
//	return false;
//}
//void LineHighlight(int x, int y)
//{
//	HDC hdc;
//	HPEN yellow = CreatePen(PS_SOLID, 4, RGB(255, 242, 0));
//	HPEN old_hpen = (HPEN)SelectObject(hdc, yellow);
//	for(int row = 0; row < 24; row++)
//	{
//		for(int col = 0; col < 24; col++)
//		{
//			//Highlight top line of box
//			if(!boxes[row][col]->top->played)
//				if(boxes[row][col]->top->startx <= x + 3 || boxes[row][col]->top->startx <= x - 3)
//				{
//					if(boxes[row][col]->top->endx >= x + 3 || boxes[row][col]->top->endx >= x - 3)
//					{
//						if(boxes[row][col]->top->starty <= y + 3 || boxes[row][col]->top->starty <= y - 3)
//						{		
//							if(boxes[row][col]->top->endy >= y + 3 || boxes[row][col]->top->endy >= y - 3)
//							{
//								HPEN yellow = CreatePen(PS_SOLID, 1, RGB(255, 242, 0));
//								HPEN old_hpen = (HPEN)SelectObject(gHDC, yellow);
//								MoveToEx(gHDC, boxes[row][col]->top->startx, boxes[row][col]->top->starty, NULL);
//								LineTo(gHDC, boxes[row][col]->top->endx, boxes[row][col]->top->endy);
//								DeleteObject(yellow);
//							}
//						}
//					}
//				}
//				else
//				{
//					HPEN white = CreatePen(PS_SOLID, 1, RGB(125, 125, 125));
//					HPEN old_hpen = (HPEN)SelectObject(gHDC, white);
//					MoveToEx(gHDC, boxes[row][col]->top->startx, boxes[row][col]->top->starty, NULL);
//					LineTo(gHDC, boxes[row][col]->top->endx, boxes[row][col]->top->endy);
//					DeleteObject(white);
//				}
//				//Highlight left line of box
//				if(!boxes[row][col]->left->played)
//					if(boxes[row][col]->left->startx <= x + 3 || boxes[row][col]->left->startx <= x - 3)
//					{
//						if(boxes[row][col]->left->endx >= x + 3 || boxes[row][col]->left->endx >= x - 3)
//						{
//							if(boxes[row][col]->left->starty <= y + 3 || boxes[row][col]->left->starty <= y - 3)
//							{		
//								if(boxes[row][col]->left->endy >= y + 3 || boxes[row][col]->left->endy >= y - 3)
//								{
//									HPEN yellow = CreatePen(PS_SOLID, 1, RGB(255, 242, 0));
//									HPEN old_hpen = (HPEN)SelectObject(gHDC, yellow);
//									MoveToEx(gHDC, boxes[row][col]->left->startx, boxes[row][col]->left->starty, NULL);
//									LineTo(gHDC, boxes[row][col]->left->endx, boxes[row][col]->left->endy);
//									DeleteObject(yellow);
//								}
//							}
//						}
//					}
//					else
//					{
//						HPEN white = CreatePen(PS_SOLID, 1, RGB(125, 125, 125));
//						HPEN old_hpen = (HPEN)SelectObject(gHDC, white);
//						MoveToEx(gHDC, boxes[row][col]->left->startx, boxes[row][col]->left->starty, NULL);
//						LineTo(gHDC, boxes[row][col]->left->endx, boxes[row][col]->left->endy);
//						DeleteObject(white);
//					}
//					//Highlight bottom line of box
//					if(!boxes[row][col]->bottom->played)
//						if(boxes[row][col]->bottom->startx <= x + 3 || boxes[row][col]->bottom->startx <= x - 3)
//						{
//							if(boxes[row][col]->bottom->endx >= x + 3 || boxes[row][col]->bottom->endx >= x - 3)
//							{
//								if(boxes[row][col]->bottom->starty <= y + 3 || boxes[row][col]->bottom->starty <= y - 3)
//								{		
//									if(boxes[row][col]->bottom->endy >= y + 3 || boxes[row][col]->bottom->endy >= y - 3)
//									{
//										HPEN yellow = CreatePen(PS_SOLID, 1, RGB(255, 242, 0));
//										HPEN old_hpen = (HPEN)SelectObject(gHDC, yellow);
//										MoveToEx(gHDC, boxes[row][col]->bottom->startx, boxes[row][col]->bottom->starty, NULL);
//										LineTo(gHDC, boxes[row][col]->bottom->endx, boxes[row][col]->bottom->endy);
//										DeleteObject(yellow);
//									}
//								}
//							}
//						}
//						else
//						{
//							HPEN white = CreatePen(PS_SOLID, 1, RGB(125, 125, 125));
//							HPEN old_hpen = (HPEN)SelectObject(gHDC, white);
//							MoveToEx(gHDC, boxes[row][col]->bottom->startx, boxes[row][col]->bottom->starty, NULL);
//							LineTo(gHDC, boxes[row][col]->bottom->endx, boxes[row][col]->bottom->endy);
//							DeleteObject(white);
//						}
//						//Highlight right line of box
//						if(!boxes[row][col]->right->played)
//							if(boxes[row][col]->right->startx <= x + 3 || boxes[row][col]->right->startx <= x - 3)
//							{
//								if(boxes[row][col]->right->endx >= x + 3 || boxes[row][col]->right->endx >= x - 3)
//								{
//									if(boxes[row][col]->right->starty <= y + 3 || boxes[row][col]->right->starty <= y - 3)
//									{		
//										if(boxes[row][col]->right->endy >= y + 3 || boxes[row][col]->right->endy >= y - 3)
//										{
//											HPEN yellow = CreatePen(PS_SOLID, 1, RGB(255, 242, 0));
//											HPEN old_hpen = (HPEN)SelectObject(gHDC, yellow);
//											MoveToEx(gHDC, boxes[row][col]->right->startx, boxes[row][col]->right->starty, NULL);
//											LineTo(gHDC, boxes[row][col]->right->endx, boxes[row][col]->right->endy);
//											DeleteObject(yellow);
//										}
//									}
//								}
//							}
//							else
//							{
//								HPEN white = CreatePen(PS_SOLID, 1, RGB(125, 125, 125));
//								HPEN old_hpen = (HPEN)SelectObject(gHDC, white);
//								MoveToEx(gHDC, boxes[row][col]->right->startx, boxes[row][col]->right->starty, NULL);
//								LineTo(gHDC, boxes[row][col]->right->endx, boxes[row][col]->right->endy);
//								DeleteObject(white);
//							}
//		}
//	}
//
//
//}

