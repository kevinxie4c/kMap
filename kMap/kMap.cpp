// kMap.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "kMap.h"
#include "Graph.h"
#include "tstring.h"
#include "FileIO.h"
#include <string>
#include "vector.h"
#include <cmath>

#define MAX_LOADSTRING 100
#define RADIUS 5
#define RADIUSSQU 64
#define S_ADD_VERTEX 0
#define S_DELETE_VERTEX 1
#define S_EDIT_VERTEX 2
#define S_ADD_EDGE 4
#define S_DELETE_EDGE 5
#define S_EDIT_EDGE 6
#define S_QUERY 7
#define MASK_TYPE 4
#define ID_EDIT_NAME 1
#define ID_EDIT_DESCRIPTION 2
#define ID_EDIT_POSITION 3
#define ID_EDIT_DISTANCE 4
#define ID_EDIT_TIME 5
#define ID_EDIT_MESSAGE 6
#define ID_COMBOBOX_SRC 7
#define ID_COMBOBOX_DST 8
#define ID_COMBOBOX_VERTEXS 9
#define ID_BUTTON_ADD 10
#define ID_BUTTON_RESET 11

#define ReportError(str) MessageBox(NULL, TEXT(str), TEXT("Error"), MB_OK)

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
int g_iState;
bool g_bShowAll;
HPEN g_hRedPen;
HBRUSH g_hRedBrush;
HPEN g_hBluePen;

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_KMAP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KMAP));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KMAP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_KMAP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

inline int Distance(int x1, int y1, int x2, int y2)
{
	return (int)sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

inline int DistSqu(int x1, int  y1, int x2, int y2)
{
	return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
}

inline int PixelToMeter(int x)
{
	return x * 20 / 7;
}

inline int Time(int distance)
{
	return distance * 5 / 4;
}

void DrawPoint(HDC hdc, int x, int y)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = (HPEN)SelectObject(hdc, g_hRedPen);
	hBrush = (HBRUSH)SelectObject(hdc, g_hRedBrush);
	Ellipse(hdc, x - RADIUS, y - RADIUS, x + RADIUS, y + RADIUS);
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);
}

void DrawSrc(HDC hdc, int x, int y)
{
	int mode;
	DrawPoint(hdc, x, y);
	mode = SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, x, y, TEXT("起点"), lstrlen(TEXT("起点")));
	SetBkMode(hdc, mode);
}

void DrawDst(HDC hdc, int x, int y)
{
	int mode;
	DrawPoint(hdc, x, y);
	mode = SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, x, y, TEXT("终点"), lstrlen(TEXT("终点")));
	SetBkMode(hdc, mode);
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	HPEN hPen;
	hPen = (HPEN)SelectObject(hdc, g_hBluePen);
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
	SelectObject(hdc, hPen);
}

void DrawPolyline(HDC hdc, POINT *apt, size_t num)
{
	HPEN hPen;
	hPen = (HPEN)SelectObject(hdc, g_hBluePen);
	Polyline(hdc, apt, num);
	SelectObject(hdc, hPen);
}

void DrawAll(HDC hdc, CGraph &g)
{
	vector<SVertex> v;
	vector<SEdge> e;
	SVertex src, dst;
	g.GetVertexsVector(v);
	for (auto v_t : v)
	{
		g.GetEdgesVector(v_t.name, e);
		for (auto e_t : e)
		{
			src = g.GetVertex(e_t.src);
			dst = g.GetVertex(e_t.dst);
			DrawLine(hdc, src.x, src.y, dst.x, dst.y);
		}
	}
	for (auto v_t : v)
		DrawPoint(hdc, v_t.x, v_t.y);
}

void HideWindows(HWND *phwnd[], int num)
{
	int i;
	for (i = 0; i < num; ++i)
		ShowWindow(*phwnd[i], SW_HIDE);
}

void ClearText(HWND *phwnd[], int num)
{
	int i;
	for (i = 0; i < num; ++i)
		SetWindowText(*phwnd[i], TEXT(""));
}

bool GetInput(HWND hwnd, std::tstring &s)
{
	TCHAR *t_buffer;
	int t_length;
	t_length = GetWindowTextLength(hwnd);
	if (t_length)
	{
		t_buffer = new TCHAR[t_length + 1];
		GetWindowText(hwnd, (LPWSTR)t_buffer, t_length + 1);
		s = t_buffer;
		return true;
	}
	else
	{
		ReportError("Input can not be empty.");
		return false;
	}
}

bool GetNum(HWND hwnd, int &num)
{
	TCHAR *t_buffer;
	int t_length;
	t_length = GetWindowTextLength(hwnd);
	if (t_length)
	{
		t_buffer = new TCHAR[t_length + 1];
		GetWindowText(hwnd, (LPWSTR)t_buffer, t_length + 1);
		for (size_t i = 0; i < _tcslen(t_buffer); ++i)
		if (t_buffer[i] < TEXT('0') || t_buffer[i] > TEXT('9'))
		{
			ReportError("Distance/Time should be an integer.");
			return false;
		}
		swscanf_s(t_buffer, TEXT("%d"), &num);
		return true;
	}
	else
	{
		ReportError("Input can not be empty.");
		return false;
	}
}

void FillComboBox(HWND hwnd, vector<std::tstring> &v)
{
	SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	for (size_t i = 0; i < v.size(); ++i)
		SendMessage(hwnd, CB_INSERTSTRING, i, (LPARAM)v[i].c_str());
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc, hdcMem;
	HINSTANCE hInstance;
	BITMAP bitmap;
	static HWND hwndEditName, hwndEditDescription, hwndEditPosition, hwndEditDistance, hwndEditTime,hwndEditMessage;
#define EDIT_NUM 6
	static HWND hwndComboBoxSrc, hwndComboBoxDst, hwndComboBoxVertexs;
	static HWND hwndButtonAdd, hwndButtonReset;
	static HWND *phwndAll[] = { &hwndEditName, &hwndEditDescription, &hwndEditPosition, &hwndEditDistance, &hwndEditTime, &hwndEditMessage,
		&hwndComboBoxSrc, &hwndComboBoxDst, &hwndComboBoxVertexs,
		&hwndButtonAdd, &hwndButtonReset };
	const int num = (sizeof phwndAll) / (sizeof phwndAll[0]);
	static HBITMAP hBitmap;
	static int cxSource, cySource, cxChar, cyChar, xSelect, ySelect;
	TCHAR buffer[256];
	int length, iIndex;
	using std::tstring;
	static vector<tstring> src, dst, vertexs, path;
	static POINT *apt;
	static size_t aptnum;
	static CGraph graph;
	static SVertex CurVertex, srcVertex, dstVertex;
	static SEdge CurEdge;
	static bool bDrawLine;
	tstring name, description, position;
	int distance, time;

	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_MAP));
		GetObject(hBitmap, sizeof BITMAP, &bitmap);
		cxSource = bitmap.bmWidth;
		cySource = bitmap.bmHeight;
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		g_hRedPen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
		g_hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
		g_hBluePen = CreatePen(PS_SOLID, 4, RGB(0, 0, 255));
		hwndEditName = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | ES_LEFT, 
			cxSource + 7 * cxChar, cyChar, 32 * cxChar, cyChar + 4, 
			hWnd, (HMENU)ID_EDIT_NAME, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndEditDescription = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER |ES_LEFT, 
			cxSource + 7 * cxChar, cyChar + 2 * 7 * cyChar / 4, 32 * cxChar, cyChar + 4,
			hWnd, (HMENU)ID_EDIT_DESCRIPTION, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndEditPosition = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | ES_LEFT, 
			cxSource + 7 * cxChar, cyChar + 3 * 7 * cyChar / 4, 32 * cxChar, cyChar + 4,
			hWnd, (HMENU)ID_EDIT_POSITION, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndEditDistance = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | ES_LEFT,
			cxSource + 7 * cxChar, cyChar + 2 * 7 * cyChar / 4, 32 * cxChar, cyChar + 4,
			hWnd, (HMENU)ID_EDIT_DISTANCE, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndEditTime = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | ES_LEFT,
			cxSource + 7 * cxChar, cyChar + 3 * 7 * cyChar / 4, 32 * cxChar, cyChar + 4,
			hWnd, (HMENU)ID_EDIT_TIME, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndEditMessage = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
			cxSource + 7 * cxChar, cyChar + 6 * 7 * cyChar / 4, 32 * cxChar, 15 * cyChar + 4,
			hWnd, (HMENU)ID_EDIT_TIME, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndComboBoxSrc = CreateWindow(TEXT("combobox"), NULL, 
			WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
			cxSource + 7 * cxChar, cyChar, 32 * cxChar, 20 * cyChar,
			hWnd, (HMENU)ID_COMBOBOX_SRC, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndComboBoxDst = CreateWindow(TEXT("combobox"), NULL,
			WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
			cxSource + 7 * cxChar, cyChar + 7 * cyChar / 4, 32 * cxChar, 20 * cyChar,
			hWnd, (HMENU)ID_COMBOBOX_DST, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndComboBoxVertexs = CreateWindow(TEXT("combobox"), NULL,
			WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL,
			cxSource + 7 * cxChar, cyChar, 32 * cxChar, 20 * cyChar,
			hWnd, (HMENU)ID_COMBOBOX_VERTEXS, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndButtonAdd = CreateWindow(TEXT("button"), NULL,
			WS_CHILD | BS_PUSHBUTTON,
			cxSource + 7 * cxChar, cyChar + 4 * 7 * cyChar / 4, 8 * cxChar, 7 * cyChar / 4,
			hWnd, (HMENU)ID_BUTTON_ADD, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hwndButtonReset = CreateWindow(TEXT("button"), TEXT("重置"),
			WS_CHILD | BS_PUSHBUTTON,
			cxSource + 16 * cxChar, cyChar + 4 * 7 * cyChar / 4, 8 * cxChar, 7 * cyChar / 4,
			hWnd, (HMENU)ID_BUTTON_RESET, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		bDrawLine = false;
		g_bShowAll = false;
		g_iState = S_ADD_VERTEX;
		HideWindows(phwndAll, num);
		ClearText(phwndAll, EDIT_NUM);
		xSelect = ySelect = -1;
		SetWindowText(hwndButtonAdd, TEXT("添加"));
		ShowWindow(hwndButtonAdd, SW_SHOWNORMAL);
		ShowWindow(hwndButtonReset, SW_SHOWNORMAL);
		ShowWindow(hwndEditName, SW_SHOWNORMAL);
		ShowWindow(hwndEditDescription, SW_SHOWNORMAL);
		ShowWindow(hwndEditPosition, SW_SHOWNORMAL);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择及子窗口事件: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_OPEN:
			if (!OpenFile(graph)) ReportError("Open file error.");
			graph.GetVertexNameVector(vertexs);
			if (g_iState == S_QUERY) graph.GetRealVertexsNameVector(src);
			else graph.GetVertexNameVector(src);
			dst.clear();
			if (g_iState == S_ADD_EDGE) graph.GetVertexNameVector(dst);
			else if (g_iState == S_QUERY) graph.GetRealVertexsNameVector(dst);
			FillComboBox(hwndComboBoxVertexs, vertexs);
			FillComboBox(hwndComboBoxSrc, src);
			FillComboBox(hwndComboBoxDst, dst);
			break;
		case IDM_SAVE:
			if (!SaveFile(graph)) ReportError("Save file error");
			break;
		case IDM_ADD_VERTEX:
			g_iState = S_ADD_VERTEX;
			HideWindows(phwndAll, num);
			ClearText(phwndAll, EDIT_NUM);
			xSelect = ySelect = -1;
			SetWindowText(hwndButtonAdd, TEXT("添加"));
			ShowWindow(hwndButtonAdd, SW_SHOWNORMAL);
			ShowWindow(hwndButtonReset, SW_SHOWNORMAL);
			ShowWindow(hwndEditName, SW_SHOWNORMAL);
			ShowWindow(hwndEditDescription, SW_SHOWNORMAL);
			ShowWindow(hwndEditPosition, SW_SHOWNORMAL);
			break;
		case IDM_ADD_EDGE:
			g_iState = S_ADD_EDGE;
			bDrawLine = false;
			HideWindows(phwndAll, num);
			ClearText(phwndAll, EDIT_NUM);
			SetWindowText(hwndButtonAdd, TEXT("添加"));
			src = dst = vertexs;
			FillComboBox(hwndComboBoxSrc, src);
			FillComboBox(hwndComboBoxDst, dst);
			ShowWindow(hwndButtonAdd, SW_SHOWNORMAL);
			ShowWindow(hwndButtonReset, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxSrc, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxDst, SW_SHOWNORMAL);
			ShowWindow(hwndEditDistance, SW_SHOWNORMAL);
			ShowWindow(hwndEditTime, SW_SHOWNORMAL);
			break;
		case IDM_DELETE_VERTEX:
			g_iState = S_DELETE_VERTEX;
			HideWindows(phwndAll, num);
			SetWindowText(hwndButtonAdd, TEXT("删除"));
			ShowWindow(hwndButtonAdd, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxVertexs, SW_SHOWNORMAL);
			break;
		case IDM_DELETE_EDGE:
			g_iState = S_DELETE_EDGE;
			bDrawLine = false;
			CurEdge.distance = CurEdge.time = -1;
			HideWindows(phwndAll, num);
			SetWindowText(hwndButtonAdd, TEXT("删除"));
			src = vertexs;
			dst.clear();
			FillComboBox(hwndComboBoxSrc, src);
			FillComboBox(hwndComboBoxDst, dst);
			ShowWindow(hwndButtonAdd, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxSrc, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxDst, SW_SHOWNORMAL);
			break;
		case IDM_EDIT_VERTEX:
			g_iState = S_EDIT_VERTEX;
			HideWindows(phwndAll, num);
			SetWindowText(hwndButtonAdd, TEXT("保存"));
			if ((iIndex = SendMessage(hwndComboBoxVertexs, CB_GETCURSEL, 0, 0)) != CB_ERR)
			{
				CurVertex = graph.GetVertex(vertexs[iIndex]);
				SetWindowText(hwndEditDescription, CurVertex.description.c_str());
				SetWindowText(hwndEditPosition, CurVertex.position.c_str());
				xSelect = CurVertex.x;
				ySelect = CurVertex.y;
			}
			else
				ClearText(phwndAll, EDIT_NUM);
			ShowWindow(hwndButtonAdd, SW_SHOWNORMAL);
			ShowWindow(hwndButtonReset, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxVertexs, SW_SHOWNORMAL);
			ShowWindow(hwndEditDescription, SW_SHOWNORMAL);
			ShowWindow(hwndEditPosition, SW_SHOWNORMAL);
			break;
		case IDM_EDIT_EDGE:
			g_iState = S_EDIT_EDGE;
			bDrawLine = false;
			HideWindows(phwndAll, num);
			ClearText(phwndAll, EDIT_NUM);
			SetWindowText(hwndButtonAdd, TEXT("保存"));
			src = vertexs;
			dst.clear();
			FillComboBox(hwndComboBoxSrc, src);
			FillComboBox(hwndComboBoxDst, dst);
			ShowWindow(hwndButtonAdd, SW_SHOWNORMAL);
			ShowWindow(hwndButtonReset, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxSrc, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxDst, SW_SHOWNORMAL);
			ShowWindow(hwndEditDistance, SW_SHOWNORMAL);
			ShowWindow(hwndEditTime, SW_SHOWNORMAL);
			break;
		case IDM_QUERY:
			g_iState = S_QUERY;
			bDrawLine = false;
			CurEdge.distance = CurEdge.time = -1;
			HideWindows(phwndAll, num);
			ClearText(phwndAll, EDIT_NUM);
			SetWindowText(hwndButtonAdd, TEXT("查询"));
			graph.GetRealVertexsNameVector(src);
			dst = src;
			FillComboBox(hwndComboBoxSrc, src);
			FillComboBox(hwndComboBoxDst, dst);
			ShowWindow(hwndEditMessage, SW_SHOWNORMAL);
			ShowWindow(hwndButtonAdd, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxSrc, SW_SHOWNORMAL);
			ShowWindow(hwndComboBoxDst, SW_SHOWNORMAL);
			break;
		case IDM_SHOW_HIDE:
			g_bShowAll = !g_bShowAll;
			break;
		case ID_BUTTON_ADD:
			switch (wmEvent)
			{
			case BN_CLICKED:
				switch (g_iState)
				{
				case S_ADD_VERTEX:
					if (GetInput(hwndEditName, name) && GetInput(hwndEditDescription, description) && GetInput(hwndEditPosition, position))
					{
						if (graph.VertexIsExist(name))
						{
							ReportError("Repeated vertex.");
							break;
						}
						graph.InsertVertex(name, description, position, xSelect, ySelect);
						vertexs.push_back(name);
						SendMessage(hwndComboBoxVertexs, CB_INSERTSTRING, vertexs.size() - 1, (LPARAM)name.c_str());
					}
					break;
				case S_ADD_EDGE:
					if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) == CB_ERR)
					{
						ReportError("Src is empty.");
						break;
					}
					srcVertex = graph.GetVertex(src[iIndex]);
					if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) == CB_ERR)
					{
						ReportError("Dst is empty.");
						break;
					}
					dstVertex = graph.GetVertex(dst[iIndex]);
					if (srcVertex.name == dstVertex.name)
					{
						ReportError("Src and dst are same.");
						break;
					}
					if (graph.EdgeIsExist(srcVertex.name, dstVertex.name))
					{
						ReportError("Repeated edge.");
						break;
					}
					if(GetNum(hwndEditDistance, distance) && GetNum(hwndEditTime, time))
						graph.InsertEdge(srcVertex.name, dstVertex.name, distance, time);
					break;
				case S_DELETE_VERTEX:
					if ((iIndex = SendMessage(hwndComboBoxVertexs, CB_GETCURSEL, 0, 0)) != CB_ERR)
					{
						graph.DeleteVertex(vertexs[iIndex]);
						graph.GetVertexNameVector(vertexs);
						FillComboBox(hwndComboBoxVertexs, vertexs);
					}
					if ((iIndex = SendMessage(hwndComboBoxVertexs, CB_GETCURSEL, 0, 0)) != CB_ERR)
					{
						CurVertex = graph.GetVertex(vertexs[iIndex]);
						SetWindowText(hwndEditDescription, CurVertex.description.c_str());
						SetWindowText(hwndEditPosition, CurVertex.position.c_str());
						xSelect = CurVertex.x;
						ySelect = CurVertex.y;
					}
					else
					{
						CurVertex.name = TEXT("");
						CurVertex.description = TEXT("");
						CurVertex.position = TEXT("");
						CurVertex.x = CurVertex.y = xSelect = ySelect = -1;						
					}
					break;
				case S_DELETE_EDGE:
					if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) == CB_ERR)
					{
						ReportError("Src is empty");
						break;
					}
					srcVertex = graph.GetVertex(src[iIndex]);
					if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) == CB_ERR)
					{
						ReportError("Dst is empty");
						break;
					}
					dstVertex = graph.GetVertex(dst[iIndex]);
					if (srcVertex.name == dstVertex.name)
					{
						ReportError("Src and dst are same.");
						break;
					}
					graph.DeleteEdge(srcVertex.name, dstVertex.name);
					graph.GetVertexNameVector(src);
					FillComboBox(hwndComboBoxSrc, src);
					dst.clear();
					FillComboBox(hwndComboBoxDst, dst);
					CurEdge.distance = CurEdge.time = -1;
					bDrawLine = false;
					break;
				case S_EDIT_VERTEX:
					if (((iIndex = SendMessage(hwndComboBoxVertexs, CB_GETCURSEL, 0, 0)) != CB_ERR) &&
						GetInput(hwndEditDescription, description) && GetInput(hwndEditPosition, position))
					{
						name = vertexs[iIndex];
						graph.InsertVertex(name, description, position, xSelect, ySelect);
					}
					break;
				case S_EDIT_EDGE:
					if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) == CB_ERR)
					{
						ReportError("Src is empty");
						break;
					}
					srcVertex = graph.GetVertex(src[iIndex]);
					if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) == CB_ERR)
					{
						ReportError("Dst is empty");
						break;
					}
					dstVertex = graph.GetVertex(dst[iIndex]);
					if (srcVertex.name == dstVertex.name)
					{
						ReportError("Src and dst are same.");
						break;
					}
					GetNum(hwndEditDistance, distance);
					GetNum(hwndEditTime, time);
					graph.InsertEdge(srcVertex.name, dstVertex.name, distance, time);
					break;
				case S_QUERY:
					CurEdge.distance = CurEdge.time = -1;
					if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) == CB_ERR)
					{
						ReportError("Src is empty");
						break;
					}
					srcVertex = graph.GetVertex(src[iIndex]);
					if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) == CB_ERR)
					{
						ReportError("Dst is empty");
						break;
					}
					dstVertex = graph.GetVertex(dst[iIndex]);
					if (srcVertex.name == dstVertex.name)
					{
						ReportError("Src and dst are same.");
						break;
					}
					if (graph.Dijkstra(srcVertex.name, dstVertex.name, path))
					{
						CurEdge.src = srcVertex.name;
						CurEdge.dst = dstVertex.name;
						CurEdge.distance = CurEdge.time = 0;
						apt = new POINT[aptnum = path.size()];
						for (size_t i = 0; i < path.size(); ++i)
						{
							SVertex t = graph.GetVertex(path[i]);
							apt[i].x = t.x;
							apt[i].y = t.y;
						}
						std::tstring output_s;
						/*
						for (size_t i = 1; i < path.size(); ++i)
						{
							SEdge t = graph.GetEdge(path[i], path[i - 1]);
							CurEdge.distance += t.distance;
							CurEdge.time += t.time;
						}*/
						for (size_t i = path.size()-1; i > 0; --i)
						{
							SEdge t = graph.GetEdge(path[i], path[i - 1]);
							CurEdge.distance += t.distance;
							CurEdge.time += t.time;
							wsprintf(buffer, TEXT("%s -> %s  distance:%d time:%d\r\n"), path[i].c_str(), path[i - 1].c_str(),
								CurEdge.distance, CurEdge.time);
							output_s += buffer;
						}
						SetWindowText(hwndEditMessage, output_s.c_str());
						bDrawLine = true;
					}
					else
					{
						bDrawLine = false;
						ReportError("Path is not exist.");
					}
					break;
				}
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case ID_BUTTON_RESET:
			ClearText(phwndAll, EDIT_NUM);
			break;
		case ID_COMBOBOX_VERTEXS:
			switch (wmEvent)
			{
			case CBN_SELCHANGE:
				if ((iIndex = SendMessage(hwndComboBoxVertexs, CB_GETCURSEL, 0, 0)) != CB_ERR)
				{
					CurVertex = graph.GetVertex(vertexs[iIndex]);
					SetWindowText(hwndEditDescription, CurVertex.description.c_str());
					SetWindowText(hwndEditPosition, CurVertex.position.c_str());
					xSelect = CurVertex.x;
					ySelect = CurVertex.y;
				}
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case ID_COMBOBOX_SRC:
			switch (wmEvent)
			{
			case CBN_SELCHANGE:
				if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) != CB_ERR)
				{
					srcVertex = graph.GetVertex(src[iIndex]);
					if (g_iState == S_ADD_EDGE)
					{
						if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) != CB_ERR)
						{
							dstVertex = graph.GetVertex(dst[iIndex]);
							int t = PixelToMeter(Distance(srcVertex.x, srcVertex.y, dstVertex.x, dstVertex.y));
							wsprintf(buffer, TEXT("%d"), t);
							SetWindowText(hwndEditDistance, buffer);
							t = Time(t);
							wsprintf(buffer, TEXT("%d"), t);
							SetWindowText(hwndEditTime, buffer);
							bDrawLine = true;
						}
						else
							bDrawLine = false;
					}
					else
					if (g_iState != S_QUERY)
					{
						bDrawLine = false;
						ClearText(phwndAll, EDIT_NUM);
						graph.GetEdgesNameVector(srcVertex.name, dst);
						FillComboBox(hwndComboBoxDst, dst);
					}
				}
				else
				{
					bDrawLine = false;
					if (g_iState != S_ADD_EDGE)
						ClearText(phwndAll, EDIT_NUM);
				}
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case ID_COMBOBOX_DST:
			switch (wmEvent)
			{
			case CBN_SELCHANGE:
				if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) != CB_ERR)
				{
					dstVertex = graph.GetVertex(dst[iIndex]);
					if (g_iState == S_ADD_EDGE)
					{
						if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) != CB_ERR)
						{
							srcVertex = graph.GetVertex(src[iIndex]);
							int t = PixelToMeter(Distance(srcVertex.x, srcVertex.y, dstVertex.x, dstVertex.y));
							wsprintf(buffer, TEXT("%d"), t);
							SetWindowText(hwndEditDistance, buffer);
							t = Time(t);
							wsprintf(buffer, TEXT("%d"), t);
							SetWindowText(hwndEditTime, buffer);
							bDrawLine = true;
						}
						else
							bDrawLine = false;
					}
					else
					if (g_iState != S_QUERY)
					{
						bDrawLine = true;
						CurEdge = graph.GetEdge(srcVertex.name, dstVertex.name);
						wsprintf(buffer, TEXT("%d"), CurEdge.distance);
						SetWindowText(hwndEditDistance, buffer);
						wsprintf(buffer, TEXT("%d"), CurEdge.time);
						SetWindowText(hwndEditTime, buffer);
					}
				}
				else
				{
					bDrawLine = false;
					if (g_iState != S_ADD_EDGE)
						ClearText(phwndAll, EDIT_NUM);
				}
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hBitmap);
		BitBlt(hdc, 0, 0, cxSource, cySource, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
		if (g_bShowAll)
			DrawAll(hdc, graph);
		if (g_iState == S_QUERY ||	//查询状态
			(g_iState & MASK_TYPE))	//边操作状态
		{
			TextOut(hdc, cxSource + 2 * cxChar, cyChar, TEXT("起始:"), lstrlen(TEXT("起始:")));
			TextOut(hdc, cxSource + 2 * cxChar, cyChar + 7 * cyChar / 4, TEXT("终止:"), lstrlen(TEXT("终止:")));
			if ((g_iState == S_DELETE_EDGE && !(CurEdge.distance == -1 && CurEdge.time == -1)) ||
				(g_iState == S_QUERY &&  bDrawLine))
			{
				length = wsprintf(buffer, TEXT("距离:%d"), CurEdge.distance);
				TextOut(hdc, cxSource + 2 * cxChar, cyChar + 2 * 7 * cyChar / 4, buffer, length);
				length = wsprintf(buffer, TEXT("时间:%d"), CurEdge.time);
				TextOut(hdc, cxSource + 2 * cxChar, cyChar + 3 * 7 * cyChar / 4, buffer, length);
			}
			else
			{
				TextOut(hdc, cxSource + 2 * cxChar, cyChar + 2 * 7 * cyChar / 4, TEXT("距离:"), lstrlen(TEXT("距离:")));
				TextOut(hdc, cxSource + 2 * cxChar, cyChar + 3 * 7 * cyChar / 4, TEXT("时间:"), lstrlen(TEXT("时间:")));
			}
			if (bDrawLine)
			{
				if (g_iState == S_QUERY)
				{
					DrawPolyline(hdc, apt, aptnum);
					DrawSrc(hdc, srcVertex.x, srcVertex.y);
					DrawDst(hdc, dstVertex.x, dstVertex.y);
				}
				else DrawLine(hdc, srcVertex.x, srcVertex.y, dstVertex.x, dstVertex.y);
			}
		}
		else //点操作状态
		{
			TextOut(hdc, cxSource + 2 * cxChar, cyChar, TEXT("名称:"), lstrlen(TEXT("名称:")));
			if (!(xSelect == -1 && ySelect == -1))
				length = wsprintf(buffer, TEXT("坐标:(%d,%d)"), xSelect, ySelect);
			else
				length = wsprintf(buffer, TEXT("坐标:"));
			TextOut(hdc, cxSource + 2 * cxChar, cyChar + 7 * cyChar / 4, buffer, length);
			if (g_iState == S_DELETE_VERTEX)
			{
				length = wsprintf(buffer, TEXT("描述:%s"),CurVertex.description.c_str());
				TextOut(hdc, cxSource + 2 * cxChar, cyChar + 2 * 7 * cyChar / 4, buffer, length);
				length = wsprintf(buffer, TEXT("位置:%s"), CurVertex.position.c_str());
				TextOut(hdc, cxSource + 2 * cxChar, cyChar + 3 * 7 * cyChar / 4, buffer, length);
			}
			else
			{
				TextOut(hdc, cxSource + 2 * cxChar, cyChar + 2 * 7 * cyChar / 4, TEXT("描述:"), lstrlen(TEXT("描述:")));
				TextOut(hdc, cxSource + 2 * cxChar, cyChar + 3 * 7 * cyChar / 4, TEXT("位置:"), lstrlen(TEXT("位置:")));
			}
			if (!(xSelect == -1 && ySelect == -1))
				DrawPoint(hdc, xSelect, ySelect);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		if ((g_iState == S_ADD_VERTEX || g_iState == S_EDIT_VERTEX) && LOWORD(lParam) < cxSource && HIWORD(lParam) < cySource)
		{
			xSelect = LOWORD(lParam);
			ySelect = HIWORD(lParam);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else
		if (g_iState == S_ADD_EDGE && g_bShowAll && LOWORD(lParam) < cxSource && HIWORD(lParam) < cySource)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			int t = -1;
			int min = 0x7f7f7f7f;
			SVertex v;
			for (int i = 0; i < (int)vertexs.size(); ++i)
			{
				v = graph.GetVertex(vertexs[i]);
				if (DistSqu(x, y, v.x, v.y) < RADIUSSQU && DistSqu(x, y, v.x, v.y) < min)
				{
					t = i;
					min = DistSqu(x, y, v.x, v.y);
				}
			}
			SendMessage(hwndComboBoxSrc, CB_SETCURSEL, t, 0);
			/*
			if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) != CB_ERR)
			{
				srcVertex = graph.GetVertex(src[iIndex]);
				if (g_iState == S_ADD_EDGE)
				{
					if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) != CB_ERR)
					{
						dstVertex = graph.GetVertex(dst[iIndex]);
						int t = PixelToMeter(Distance(srcVertex.x, srcVertex.y, dstVertex.x, dstVertex.y));
						wsprintf(buffer, TEXT("%d"), t);
						SetWindowText(hwndEditDistance, buffer);
						t = Time(t);
						wsprintf(buffer, TEXT("%d"), t);
						SetWindowText(hwndEditTime, buffer);
						bDrawLine = true;
					}
					else
						bDrawLine = false;
				}
				else
				if (g_iState != S_QUERY)
				{
					bDrawLine = false;
					ClearText(phwndAll, EDIT_NUM);
					graph.GetEdgesNameVector(srcVertex.name, dst);
					FillComboBox(hwndComboBoxDst, dst);
				}
			}
			else
			{
				bDrawLine = false;
				if (g_iState != S_ADD_EDGE)
					ClearText(phwndAll, EDIT_NUM);
			}
			*/
			//InvalidateRect(hWnd, NULL, TRUE);
		}
		else
		if (g_iState == S_DELETE_VERTEX && g_bShowAll && LOWORD(lParam) < cxSource && HIWORD(lParam) < cySource)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			int t = -1;
			int min = 0x7f7f7f7f;
			SVertex v;
			CurVertex.name = CurVertex.description = CurVertex.position = TEXT("");
			CurVertex.x = CurVertex.y = xSelect = ySelect = -1;
			for (int i = 0; i < (int)vertexs.size(); ++i)
			{
				v = graph.GetVertex(vertexs[i]);
				if (DistSqu(x, y, v.x, v.y) < RADIUSSQU && DistSqu(x, y, v.x, v.y) < min)
				{
					t = i;
					min = DistSqu(x, y, v.x, v.y);
					CurVertex = v;
					xSelect = v.x;
					ySelect = v.y;
				}
			}
			SendMessage(hwndComboBoxVertexs, CB_SETCURSEL, t, 0);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else
		if ((g_iState == S_DELETE_EDGE || g_iState == S_EDIT_EDGE ) && g_bShowAll && LOWORD(lParam) < cxSource && HIWORD(lParam) < cySource)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			int t = -1;
			int min = 0x7f7f7f7f;
			SVertex v;
			for (int i = 0; i < (int)vertexs.size(); ++i)
			{
				v = graph.GetVertex(vertexs[i]);
				if (DistSqu(x, y, v.x, v.y) < RADIUSSQU && DistSqu(x, y, v.x, v.y) < min)
				{
					t = i;
					min = DistSqu(x, y, v.x, v.y);
					srcVertex = v;
				}
			}
			SendMessage(hwndComboBoxSrc, CB_SETCURSEL, t, 0);
			ClearText(phwndAll, EDIT_NUM);
			graph.GetEdgesNameVector(srcVertex.name, dst);
			FillComboBox(hwndComboBoxDst, dst);
		}
		break;
	case WM_LBUTTONUP:
		if (g_iState == S_ADD_EDGE && g_bShowAll && LOWORD(lParam) < cxSource && HIWORD(lParam) < cySource)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			int t = -1;
			int min = 0x7f7f7f7f;
			SVertex v;
			for (int i = 0; i < (int)vertexs.size(); ++i)
			{
				v = graph.GetVertex(vertexs[i]);
				if (DistSqu(x, y, v.x, v.y) < RADIUSSQU && DistSqu(x, y, v.x, v.y) < min)
				{
					t = i;
					min = DistSqu(x, y, v.x, v.y);
				}
			}
			SendMessage(hwndComboBoxDst, CB_SETCURSEL, t, 0);
			if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) != CB_ERR)
			{
				dstVertex = graph.GetVertex(dst[iIndex]);
				if (g_iState == S_ADD_EDGE)
				{
					if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) != CB_ERR)
					{
						srcVertex = graph.GetVertex(src[iIndex]);
						int t = PixelToMeter(Distance(srcVertex.x, srcVertex.y, dstVertex.x, dstVertex.y));
						wsprintf(buffer, TEXT("%d"), t);
						SetWindowText(hwndEditDistance, buffer);
						t = Time(t);
						wsprintf(buffer, TEXT("%d"), t);
						SetWindowText(hwndEditTime, buffer);
						bDrawLine = true;
					}
					else
						bDrawLine = false;
				}
				else
				if (g_iState != S_QUERY)
				{
					bDrawLine = true;
					CurEdge = graph.GetEdge(srcVertex.name, dstVertex.name);
					wsprintf(buffer, TEXT("%d"), CurEdge.distance);
					SetWindowText(hwndEditDistance, buffer);
					wsprintf(buffer, TEXT("%d"), CurEdge.time);
					SetWindowText(hwndEditTime, buffer);
				}
			}
			else
			{
				bDrawLine = false;
				if (g_iState != S_ADD_EDGE)
					ClearText(phwndAll, EDIT_NUM);
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else
		if ((g_iState == S_DELETE_EDGE || g_iState == S_EDIT_EDGE) && g_bShowAll && LOWORD(lParam) < cxSource && HIWORD(lParam) < cySource)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			int t = -1;
			int min = 0x7f7f7f7f;
			SVertex v;
			for (int i = 0; i < (int)dst.size(); ++i)
			{
				v = graph.GetVertex(dst[i]);
				if (DistSqu(x, y, v.x, v.y) < RADIUSSQU && DistSqu(x, y, v.x, v.y) < min)
				{
					t = i;
					min = DistSqu(x, y, v.x, v.y);
				}
			}
			SendMessage(hwndComboBoxDst, CB_SETCURSEL, t, 0);
			if ((iIndex = SendMessage(hwndComboBoxDst, CB_GETCURSEL, 0, 0)) != CB_ERR)
			{
				dstVertex = graph.GetVertex(dst[iIndex]);
				if (g_iState == S_ADD_EDGE)
				{
					if ((iIndex = SendMessage(hwndComboBoxSrc, CB_GETCURSEL, 0, 0)) != CB_ERR)
					{
						srcVertex = graph.GetVertex(src[iIndex]);
						int t = PixelToMeter(Distance(srcVertex.x, srcVertex.y, dstVertex.x, dstVertex.y));
						wsprintf(buffer, TEXT("%d"), t);
						SetWindowText(hwndEditDistance, buffer);
						t = Time(t);
						wsprintf(buffer, TEXT("%d"), t);
						SetWindowText(hwndEditTime, buffer);
						bDrawLine = true;
					}
					else
						bDrawLine = false;
				}
				else
				if (g_iState != S_QUERY)
				{
					bDrawLine = true;
					CurEdge = graph.GetEdge(srcVertex.name, dstVertex.name);
					wsprintf(buffer, TEXT("%d"), CurEdge.distance);
					SetWindowText(hwndEditDistance, buffer);
					wsprintf(buffer, TEXT("%d"), CurEdge.time);
					SetWindowText(hwndEditTime, buffer);
				}
			}
			else
			{
				bDrawLine = false;
				if (g_iState != S_ADD_EDGE)
					ClearText(phwndAll, EDIT_NUM);
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_RBUTTONDOWN:
		if ((g_iState == S_ADD_VERTEX) && LOWORD(lParam) < cxSource && HIWORD(lParam) < cySource)
		{
			xSelect = LOWORD(lParam);
			ySelect = HIWORD(lParam);
			wsprintf(buffer, TEXT("#null"));
			SetWindowText(hwndEditDescription, buffer);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_DESTROY:
		DeleteObject(g_hRedPen);
		DeleteObject(g_hRedBrush);
		DeleteObject(g_hBluePen);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
