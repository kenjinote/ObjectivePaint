#include <Windows.h>
#include <gdiplus.h>
#include <math.h>
#include <vector>
#include "util.h"
#include "effectshadow.h"


effectshadow::effectshadow() : effect(), blur(4), angle(45), distance(4), power(1), alpha(100), color(Gdiplus::Color::Black), inner(FALSE)
{
}


effectshadow::~effectshadow()
{
}


effect* effectshadow::copy() const
{
	return (effect*)new effectshadow(*this);
}

void effectshadow::getliststring(LPTSTR lpszText, int nSize) const
{
	swprintf_s(lpszText, nSize, TEXT("‰e [#%02x%02x%02x]"),
		color.GetRed(), color.GetGreen(), color.GetBlue());
}

void effectshadow::draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, float w, float h) const
{
	if (inner)
	{
		g->SetClip(path);
	}
	Gdiplus::Matrix translateMatrix;
	const float radian = angle * PI / 180.0f;
	translateMatrix.Translate(distance*cosf(radian) / 4.0f, distance*sinf(radian) / 4.0f);
	path->Transform(&translateMatrix);

	const BYTE a = (BYTE)(32.0f*(alpha / 100.0f));

	Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap((INT)(4 * w), (INT)(4 * h), PixelFormat32bppARGB);
	{
		Gdiplus::Graphics gg(pBitmap);
		gg.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		gg.ScaleTransform(2.0f, 2.0f);

		gg.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color((BYTE)(255.0f*(alpha / 100.0f)), color.GetR(), color.GetG(), color.GetB())), path);
	}

	util::DoGaussianBlurPower(pBitmap, blur / 5.0f, blur / 5.0f, power);
	g->DrawImage(pBitmap, 0.0f, 0.0f, 2 * w, 2 * h);
	delete pBitmap;

	if (inner)
	{
		g->ResetClip();
	}
}


void effectshadow::save(HANDLE hFile) const
{
	DWORD dwWriteSize;
	TCHAR ObjectType = TEXT('H');
	WriteFile(hFile, &ObjectType, sizeof(TCHAR), &dwWriteSize, 0);

	WriteFile(hFile, &blur, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &angle, sizeof(short), &dwWriteSize, 0);
	WriteFile(hFile, &distance, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &power, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &alpha, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &color, sizeof(DWORD), &dwWriteSize, 0);
	WriteFile(hFile, &inner, sizeof(BYTE), &dwWriteSize, 0);
}

void effectshadow::open(HANDLE hFile)
{
	DWORD dwReadSize;

	ReadFile(hFile, &blur, sizeof(BYTE), &dwReadSize, 0);
	ReadFile(hFile, &angle, sizeof(short), &dwReadSize, 0);
	ReadFile(hFile, &distance, sizeof(BYTE), &dwReadSize, 0);
	ReadFile(hFile, &power, sizeof(BYTE), &dwReadSize, 0);
	ReadFile(hFile, &alpha, sizeof(BYTE), &dwReadSize, 0);
	ReadFile(hFile, &color, sizeof(DWORD), &dwReadSize, 0);
	ReadFile(hFile, &inner, sizeof(BYTE), &dwReadSize, 0);
}

