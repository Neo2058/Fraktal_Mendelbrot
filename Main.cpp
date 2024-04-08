﻿#include "Main.h"

//SPoint
//------------------------------------------------------------------------------------------------------------
SPoint::SPoint()
	: X(0), Y(0)
{
};
//------------------------------------------------------------------------------------------------------------
SPoint::SPoint(unsigned short x, unsigned short y)
	: X(x), Y(y)
{
};
//------------------------------------------------------------------------------------------------------------




//SSize
//------------------------------------------------------------------------------------------------------------
SSize::SSize()
	: Width(0), Height(0)
{
};
//------------------------------------------------------------------------------------------------------------
SSize::SSize(unsigned short width, unsigned short height)
	: Width(width), Height(height)
{
};
//------------------------------------------------------------------------------------------------------------




// AsFrame_DC
//------------------------------------------------------------------------------------------------------------
AsFrame_DC::~AsFrame_DC()
{
	if (Bitmap != 0)
		DeleteObject(Bitmap);

	if (DC != 0)
		DeleteObject(DC);
}
//------------------------------------------------------------------------------------------------------------
AsFrame_DC::AsFrame_DC()
	: DC(0), Bitmap(0), BG_Brush(0), Bitmap_Buf(0)
{
	BG_Brush = CreateSolidBrush(RGB(0, 0, 0));
	White_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));

	/*Create_Color_Palette();
	Create_Web_Palette();*/
	Create_Two_Color_Palette(0, SRGB(255, 128, 64), SRGB(0, 128, 64) );
	Create_Two_Color_Palette(Colors_Count / 2, SRGB(0, 128, 64), SRGB(128, 0, 255) );
}
//------------------------------------------------------------------------------------------------------------
HDC AsFrame_DC::Get_DC(HWND hwnd, HDC hdc)
{
	int dc_width, dc_height;
	RECT rect;
	BITMAPINFO bmp_info{};
	GetClientRect(hwnd, &rect);

	dc_width = rect.right - rect.left;
	dc_height = rect.bottom - rect.top;

	if (dc_width != Buf_Size.Width && dc_height != Buf_Size.Height)
	{
		if (Bitmap != 0)
			DeleteObject(Bitmap);

		if (DC != 0)
			DeleteObject(DC);

		Buf_Size.Width = dc_width;
		Buf_Size.Height = dc_height;

		DC = CreateCompatibleDC(hdc);
		//Bitmap = CreateCompatibleBitmap(hdc, Width, Height);

		bmp_info.bmiHeader.biSize = sizeof(BITMAPINFO);
		bmp_info.bmiHeader.biWidth = Buf_Size.Width;
		bmp_info.bmiHeader.biHeight = Buf_Size.Height;
		bmp_info.bmiHeader.biPlanes = 1;
		bmp_info.bmiHeader.biBitCount = 32;
		bmp_info.bmiHeader.biCompression = BI_RGB;

		Bitmap = CreateDIBSection(hdc, &bmp_info, DIB_PAL_COLORS, (void **)&Bitmap_Buf, 0, 0);

		if(Bitmap != 0)
			SelectObject(DC, Bitmap);

		++rect.right;
		++rect.bottom;

		SelectObject(DC, BG_Brush);
		Rectangle(DC, rect.left, rect.top, rect.right, rect.bottom);
	}

	return DC;
}
//------------------------------------------------------------------------------------------------------------
char *AsFrame_DC::Get_Buf()
{
	return Bitmap_Buf;
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Create_Color_Palette()
{
	int i;
	int rgb_color;
	int color_angle;

	for (i = 0; i < Colors_Count; i++)
	{
		color_angle = (int)((double)i / (double)Colors_Count * 360);
		rgb_color = Color_To_RGB(color_angle);

		Palette_RGB[i] = rgb_color;

		Palette_Pens[i] = CreatePen(PS_SOLID, 0, rgb_color);
		Palette_Brush[i] = CreateSolidBrush(rgb_color);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Create_Two_Color_Palette(int start_index, const SRGB &color_1, const SRGB &color_2)
{
	int len = Colors_Count / 2;
	double delta_r, delta_g, delta_b;
	double curr_r, curr_g, curr_b;
	SRGB curr_color(0, 0, 0);

	delta_r = (double)(color_2.R - color_1.R) / (double)len;
	delta_g = (double)(color_2.G - color_1.G) / (double)len;
	delta_b = (double)(color_2.B - color_1.B) / (double)len;

	curr_r = (double)color_1.R;
	curr_g = (double)color_1.G;
	curr_b = (double)color_1.B;

	for (int i = 0; i < len; i++)
	{
		curr_color.R = (unsigned char)curr_r;
		curr_color.G = (unsigned char)curr_g;
		curr_color.B = (unsigned char)curr_b;

		curr_r += delta_r;
		curr_g += delta_g;
		curr_b += delta_b;

		Palette_RGB[start_index + i] = RGB(curr_color.R, curr_color.G, curr_color.B);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Create_Web_Palette()
{
	int pos = 0;
	unsigned char r, g, b;
	unsigned int base_color = 0;

	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 8; j++)
		{

			if (j & 1)
				r = 21;
			else
				r = 0;
			
			if (j & 2)
				g = 21;
			else
				g = 0;
			
			if (j & 4)
				b = 21;
			else
				b = 0;

			Palette_Web[pos++] = base_color + RGB(r, g, b);
		}
		base_color += 0x00151515;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Draw_Color_Palette(HDC hdc)
{
	int i;
	double x_pos = 0.0;
	double bar_width = (double)Buf_Size.Width / (double)Colors_Count;

	for (i = 0; i < Colors_Count; i++)
	{
		SelectObject(hdc, Palette_Pens[i]);
		SelectObject(hdc, Palette_Brush[i]);

		Rectangle(hdc, (int)x_pos, Buf_Size.Height / 2, (int)(x_pos + bar_width), Buf_Size.Height);

		x_pos += bar_width;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Draw_Web_Palette(HDC hdc)
{
	int i;
	int len = sizeof(Palette_Web) / sizeof(Palette_Web[0]);
	double x_pos = 0.0;
	double bar_width = (double)Buf_Size.Width / (double)len;
	HPEN pen;
	HBRUSH brush;

	for (i = 0; i < len; i++)
	{
		pen = CreatePen(PS_SOLID, 0, Palette_Web[i]);
		brush = CreateSolidBrush(Palette_Web[i]);

		SelectObject(hdc, pen);
		SelectObject(hdc, brush);

		Rectangle(hdc, (int)x_pos, Buf_Size.Height / 2, (int)(x_pos + bar_width), Buf_Size.Height);

		x_pos += bar_width;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Draw_Multi_Color_Palette(HDC hdc)
{
	int i;
	//int len = sizeof(Palette_RGB) / sizeof(Palette_Web[0]);
	double x_pos = 0.0;
	double bar_width = (double)Buf_Size.Width / (double)Colors_Count;
	HPEN pen;
	HBRUSH brush;

	for (i = 0; i < Colors_Count; i++)
	{
		pen = CreatePen(PS_SOLID, 0, Palette_RGB[i]);
		brush = CreateSolidBrush(Palette_RGB[i]);

		SelectObject(hdc, pen);
		SelectObject(hdc, brush);

		Rectangle(hdc, (int)x_pos, Buf_Size.Height / 2, (int)(x_pos + bar_width), Buf_Size.Height);

		x_pos += bar_width;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsFrame_DC::Draw_Grayscale_Palette(HDC hdc)
{
	int i;
	HPEN pen;
	HBRUSH brush;
	double x_pos = 0.0;
	double bar_width = (double)Buf_Size.Width / (double)Colors_Count;

	for (i = 0; i < Colors_Count; i++)
	{
		pen = CreatePen(PS_SOLID, 0, RGB(i, i, i));
		brush = CreateSolidBrush(RGB(i, i, i));

		SelectObject(hdc, pen);
		SelectObject(hdc, brush);

		Rectangle(hdc, (int)x_pos, 0, (int)(x_pos + bar_width), Buf_Size.Height / 2);

		x_pos += bar_width;
	}
}
//------------------------------------------------------------------------------------------------------------
int AsFrame_DC::Color_To_RGB(int color)
{//color = [0 .. 360) = H for HSV S = 1.0, V = 1.0

	unsigned char r, g, b;
	unsigned char v_inc, v_dec;

	int h = color / 60; //h = [0 .. 5]

	double a = (double)(color % 60) / 60.0; //a = [0 .. 1.0)

	v_inc = (unsigned char)(a * 255.0);
	v_dec = (unsigned char)((1.0 - a) * 255.0);

	switch (h)
	{
	case 0:
		r = 255;
		g = v_inc;
		b = 0;
		break;

	case 1:
		r = v_dec;
		g = 255;
		b = 0;
		break;

	case 2:
		r = 0;
		g = 255;
		b = v_inc;
		break;

	case 3:
		r = 0;
		g = v_dec;
		b = 255;
		break;

	case 4:
		r = v_inc;
		g = 0;
		b = 255;
		break;

	case 5:
		r = 255;
		g = 0;
		b = v_dec;
		break;

	default:
		throw 13;
	}

	return RGB(r, g, b);
}
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
#define MAX_LOADSTRING 100
// Глобальные переменные:
double Main_Scale = 2.0;
double Center_X = -0.5;
double Center_Y = 0.0;


AsFrame_DC Frame_DC;
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

//------------------------------------------------------------------------------------------------------------
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_FRACTALASSEMBLER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FRACTALASSEMBLER));

	MSG msg;

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
//------------------------------------------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FRACTALASSEMBLER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = Frame_DC.BG_Brush;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FRACTALASSEMBLER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
//------------------------------------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE instance, int command_show)
{
	HWND hwnd;
	RECT window_rect;

	hInst = instance;

	window_rect.left = 0;
	window_rect.top = 0;
	window_rect.right = 320 * 3;
	window_rect.bottom = 200 * 3;

	AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW - WS_THICKFRAME, TRUE);

	hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW - WS_THICKFRAME, 0, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, 0, 0, instance, 0);

	if (hwnd == 0)
		return FALSE;

	ShowWindow(hwnd, command_show);
	UpdateWindow(hwnd);

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------
void Clear_Screen(HDC frame_dc)
{
	char *buf;
	SPoint start_point(0, 0);
	SPoint end_point(300, 400);
	SBuf_Color buf_color;
	unsigned long long start_tick = 0;
	unsigned long long end_tick = 0;
	unsigned long long delta_tick = 0;

	buf = Frame_DC.Get_Buf();
	//Asm_Draw(buf, Frame_DC.Buf_Size);
	buf_color.Buf_Size = Frame_DC.Buf_Size;
	buf_color.Color = 0xffffffff;

	start_tick = __rdtsc();

	for (int i = 0; i < Frame_DC.Buf_Size.Height; i++)
	{
		start_point.Y = i;
		Asm_Draw_Horizontal_Line(buf, start_point, Frame_DC.Buf_Size.Width, buf_color);
	}
	/*SelectObject(frame_dc, Frame_DC.White_Pen);


	for (int i = 0; i < Frame_DC.Buf_Size.Height; i++)
	{
		MoveToEx(frame_dc, 0, i, 0);
		LineTo(frame_dc, Frame_DC.Buf_Size.Width, i);
	}*/

	end_tick = __rdtsc();

	delta_tick = end_tick - start_tick; //windows 10418752, 4240732, 6782907; assembler 1813211, 4816174, 1556961 количество тактов с использование функций
}
//------------------------------------------------------------------------------------------------------------
void Draw_Line(HDC frame_dc)
{
	char *buf;
	SPoint start_point(10, 0);
	SPoint end_point(910, 100);
	SBuf_Color buf_color;
	unsigned long long start_tick, end_tick, delta_tick;

	SelectObject(frame_dc, Frame_DC.White_Pen);
	
	start_tick = __rdtsc();

	/*for (int i = 0; i < Frame_DC.Buf_Size.Height - 100; i++)
	{
		MoveToEx(frame_dc, 10, i, 0);
		LineTo(frame_dc, 910, i + 100);
	}*/

	buf = Frame_DC.Get_Buf();
	buf_color.Buf_Size = Frame_DC.Buf_Size;
	buf_color.Color = 0xffffffff;

	for (int i = 0; i < Frame_DC.Buf_Size.Height - 100; i++)
	{
		start_point.Y = i;
		end_point.Y = i + 100;

		Asm_Draw_Line(buf, start_point, end_point, buf_color);
	}

	end_tick = __rdtsc();

	delta_tick = end_tick - start_tick;
}
//------------------------------------------------------------------------------------------------------------
void Draw_Mandelbrot(HDC frame_dc)
{
	int i = 0;
	double x_0, y_0;
	double x_n, y_n;
	double x_n1, y_n1;
	//double center_x = -0.994399990001;
	//double center_y = -0.3;
	double x_scale = (double)Frame_DC.Buf_Size.Width / (double)Frame_DC.Buf_Size.Height * Main_Scale;
	double distance;
	int color;
	unsigned long long start_tick, end_tick, delta_tick;

	start_tick = __rdtsc();

	for (int y = 0; y < Frame_DC.Buf_Size.Height; y++)
	{
		y_0 = (double)y / (double)Frame_DC.Buf_Size.Height - 0.5; //y_0 = [-0.5 .. 0.5)
		y_0 = y_0 * Main_Scale + Center_Y;

		for (int x = 0; x < Frame_DC.Buf_Size.Width; x++)
		{
			x_0 = (double)x / (double)Frame_DC.Buf_Size.Width - 0.5; //x_0 = [-0.5 .. 0.5)
			x_0 = x_0 * x_scale + Center_X;

			x_n = 0.0;
			y_n = 0.0;

			for (i = 0; i < AsFrame_DC::Colors_Count; i++)
			{
				x_n1 = x_n * x_n - y_n * y_n + x_0;
				y_n1 = 2.0 * x_n * y_n + y_0;

				distance = x_n1 * x_n1 + y_n1 * y_n1;
				if (distance > 4.0)
					break;

				x_n = x_n1;
				y_n = y_n1;
			}

			if (i == Frame_DC.Colors_Count)
				color = 0;
			else
				color = Frame_DC.Palette_RGB[i];

			SetPixel(frame_dc, x, y, color);
		}
	}
	
	end_tick = __rdtsc();

	delta_tick = end_tick - start_tick; // 2 044 401 997, 1 764 638 134, 1 793 496 651 // 
}
//------------------------------------------------------------------------------------------------------------
void On_Paint(HWND hwnd)
{
	HDC hdc, frame_dc;
	PAINTSTRUCT ps;
	
	hdc = BeginPaint(hwnd, &ps);
	frame_dc = Frame_DC.Get_DC(hwnd, hdc);

	GdiFlush();

	//Clear_Screen(frame_dc);
	//Main_Scale /= 2.0;

	Draw_Mandelbrot(frame_dc);

	//Frame_DC.Draw_Web_Palette(frame_dc);
	//Frame_DC.Draw_Grayscale_Palette(frame_dc);
	//Frame_DC.Draw_Color_Palette(frame_dc);
	//Frame_DC.Draw_Multi_Color_Palette(frame_dc);

	InvalidateRect(hwnd, &ps.rcPaint, FALSE);

	BitBlt(hdc, 0, 0, Frame_DC.Buf_Size.Width, Frame_DC.Buf_Size.Height, frame_dc, 0, 0, SRCCOPY);


	EndPaint(hwnd, &ps);
}
//------------------------------------------------------------------------------------------------------------
void On_Mouse_Down(unsigned int lParam)
{
	int x_pos, int y_pos;
	int window_center_x_pos = Frame_DC.Buf_Size.Width / 2;
	double center_x_offset;

	x_pos = lParam & 0xffff;
	y_pos = (lParam >> 16) & 0xffff;

	center_x_offset =(double)(x_pos - window_center_x_pos) / (double)Frame_DC.Buf_Size.Width;
}
//------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;


		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;


		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;


	case WM_PAINT:
		On_Paint(hWnd);
		break;


	case WM_LBUTTONDOWN:
		if (wParam == MK_LBUTTON)
			On_Mouse_Down( (unsigned int)lParam);
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
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
//------------------------------------------------------------------------------------------------------------