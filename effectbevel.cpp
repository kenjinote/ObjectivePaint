#include <Windows.h>
#include <gdiplus.h>
#include <math.h>
#include <vector>
#include "util.h"
#include "effectbevel.h"

effectbevel::effectbevel() : effect(), blur(4), power(1), angle(45), width(4), alpha(100), color1(Gdiplus::Color::White), color2(Gdiplus::Color::Black)
{
}


effectbevel::~effectbevel()
{
}

effect* effectbevel::copy() const
{
	return (effect*)new effectbevel(*this);
}

void effectbevel::getliststring(LPTSTR lpszText, int nSize) const
{
	swprintf_s(lpszText, nSize, TEXT("ƒxƒxƒ‹ [#%02x%02x%02x, #%02x%02x%02x]"),
		color1.GetRed(), color1.GetGreen(), color1.GetBlue(),
		color2.GetRed(), color2.GetGreen(), color2.GetBlue());
}

void effectbevel::draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, float w, float h) const
{
	g->SetClip(path);

	Gdiplus::GraphicsPath* path1 = path->Clone();
	Gdiplus::GraphicsPath* path2 = path->Clone();

	const float radian = angle * PI / 180.0f;

	Gdiplus::Matrix translateMatrix1;
	translateMatrix1.Translate(width * cos(radian) / 5.0f, width * sin(radian) / 5.0f);
	path1->Transform(&translateMatrix1);

	Gdiplus::Matrix translateMatrix2;
	translateMatrix2.Translate(-width * cos(radian) / 5.0f, -width * sin(radian) / 5.0f);
	path2->Transform(&translateMatrix2);

	Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap((INT)(4*w), (INT)(4*h), PixelFormat32bppARGB);
	{
		Gdiplus::Graphics gg(pBitmap);
		gg.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		gg.ScaleTransform(2.0f, 2.0f);

		gg.SetClip(path2, Gdiplus::CombineMode::CombineModeExclude);
		gg.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color((BYTE)(255.0f*(alpha / 100.0f)), color2.GetR(), color2.GetG(), color2.GetB())), path1);
		gg.ResetClip();

		gg.SetClip(path1, Gdiplus::CombineMode::CombineModeExclude);
		gg.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color((BYTE)(255.0f*(alpha / 100.0f)), color1.GetR(), color1.GetG(), color1.GetB())), path2);
		gg.ResetClip();
	}
	util::DoGaussianBlurPower(pBitmap, blur / 5.0f, blur / 5.0f, power);
	g->DrawImage(pBitmap, 0.0f, 0.0f, 2*w, 2*h);

	delete path1;
	delete path2;

	g->ResetClip();

	delete pBitmap;
}


void effectbevel::save(HANDLE hFile) const
{
	DWORD dwWriteSize;
	TCHAR ObjectType = TEXT('V');
	WriteFile(hFile, &ObjectType, sizeof(TCHAR), &dwWriteSize, 0);

	WriteFile(hFile, &blur, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &power, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &angle, sizeof(short), &dwWriteSize, 0);
	WriteFile(hFile, &width, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &alpha, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &color1, sizeof(DWORD), &dwWriteSize, 0);
	WriteFile(hFile, &color2, sizeof(DWORD), &dwWriteSize, 0);
}

void effectbevel::open(HANDLE hFile)
{
	DWORD dwReadSize;

	WriteFile(hFile, &blur, sizeof(BYTE), &dwReadSize, 0);
	WriteFile(hFile, &power, sizeof(BYTE), &dwReadSize, 0);
	WriteFile(hFile, &angle, sizeof(short), &dwReadSize, 0);
	WriteFile(hFile, &width, sizeof(BYTE), &dwReadSize, 0);
	WriteFile(hFile, &alpha, sizeof(BYTE), &dwReadSize, 0);
	WriteFile(hFile, &color1, sizeof(DWORD), &dwReadSize, 0);
	WriteFile(hFile, &color2, sizeof(DWORD), &dwReadSize, 0);
}

