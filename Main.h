#pragma once
#include "framework.h"
#include "resource.h"

//------------------------------------------------------------------------------------------------------------
struct SRGB
{
	SRGB(unsigned char r, unsigned char g, unsigned char b)
		: R(r), G(g), B(b)
	{};

	unsigned char R, G, B;
};
//------------------------------------------------------------------------------------------------------------
struct SPoint
{
	SPoint();
	SPoint(unsigned short x, unsigned short y);

	unsigned short X, Y;
};
//------------------------------------------------------------------------------------------------------------
struct SSize
{
	SSize();
	SSize(unsigned short Width, unsigned short Height);

	unsigned short Width, Height;
};
//------------------------------------------------------------------------------------------------------------
struct SBuf_Color
{
	SSize Buf_Size;
	unsigned int Color;
};
//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	~AsFrame_DC();
	AsFrame_DC();

	HDC Get_DC(HWND hwnd, HDC hdc);
	char *Get_Buf();

	void Create_Color_Palette();
	void Create_Two_Color_Palette(int start_index, const SRGB & color_1, const SRGB & color_2);
	void Create_Web_Palette();
	void Draw_Color_Palette(HDC hdc);

	void Draw_Web_Palette(HDC hdc);

	void Draw_Multi_Color_Palette(HDC hdc);

	void Draw_Grayscale_Palette(HDC hdc);


	//int Width, Height;
	SSize Buf_Size;
	HBRUSH BG_Brush;
	HPEN White_Pen;

	static const int Colors_Count = 400;

	int Palette_RGB[Colors_Count];
	int Palette_Web[Colors_Count];

private:
	HDC DC;
	HBITMAP Bitmap;
	char *Bitmap_Buf;

	int Color_To_RGB(int color);

	HPEN Palette_Pens[Colors_Count];
	HBRUSH Palette_Brush[Colors_Count];
};
//------------------------------------------------------------------------------------------------------------
extern "C" void Asm_Draw(char *video_buf, SSize size);
extern "C" void Asm_Draw_Line(char *video_buf, SPoint start_point, SPoint end_point, SBuf_Color buf_color);
extern "C" void Asm_Draw_Horizontal_Line(char *video_buf, SPoint start_point, int lenght, SBuf_Color buf_color);
