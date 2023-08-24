#include <Windows.h>
#include <gdiplus.h>
#include <math.h>
#include <vector>
#include "util.h"
#include "effectemission.h"


effectemission::effectemission() :effect(), blur(6), power(2), color(Gdiplus::Color::Red), inner(FALSE)
{
}


effectemission::~effectemission()
{
}

effect* effectemission::copy() const
{
	return (effect*)new effectemission(*this);
}

void effectemission::getliststring(LPTSTR lpszText, int nSize) const
{
	swprintf_s(lpszText, nSize, TEXT("”­Œõ [#%02x%02x%02x]"),
		color.GetRed(), color.GetGreen(), color.GetBlue());
}

void effectemission::draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, float width, float height) const
{
	if (inner)
		g->SetClip(path);

	for (int i = 1; i<8; ++i)
	{
		Gdiplus::Pen pen(Gdiplus::Color((BYTE)(32 + (255.0f-32.0f)*(power / 255.0f)), color.GetRed(), color.GetGreen(), color.GetBlue()), (Gdiplus::REAL)(i*blur / 16.0));
		pen.SetLineJoin(Gdiplus::LineJoin::LineJoinRound);
		g->DrawPath(&pen, path);
	}

	if (inner)
		g->ResetClip();
}


void effectemission::save(HANDLE hFile) const
{
	DWORD dwWriteSize;
	TCHAR ObjectType = TEXT('E');
	WriteFile(hFile, &ObjectType, sizeof(TCHAR), &dwWriteSize, 0);

	WriteFile(hFile, &blur, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &power, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &color, sizeof(DWORD), &dwWriteSize, 0);
	WriteFile(hFile, &inner, sizeof(BYTE), &dwWriteSize, 0);
}

void effectemission::open(HANDLE hFile)
{
	DWORD dwReadSize;

	ReadFile(hFile, &blur, sizeof(BYTE), &dwReadSize, 0);
	ReadFile(hFile, &power, sizeof(BYTE), &dwReadSize, 0);
	ReadFile(hFile, &color, sizeof(DWORD), &dwReadSize, 0);
	ReadFile(hFile, &inner, sizeof(BYTE), &dwReadSize, 0);
}