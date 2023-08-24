#include <Windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include "util.h"
#include "effectblur.h"


effectblur::effectblur() : effect(), horizontal(4), vertical(4)
{
}

effectblur::~effectblur()
{
}

effect* effectblur::copy() const
{
	return (effect*)new effectblur(*this);
}

void effectblur::getliststring(LPTSTR lpszText, int nSize) const
{
	swprintf_s(lpszText, nSize, TEXT("‚Ú‚©‚µ [%d, %d]"), horizontal, vertical);
}

void effectblur::draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, float width, float height) const
{
}

void effectblur::save(HANDLE hFile) const
{
	DWORD dwWriteSize;
	TCHAR ObjectType = TEXT('B');
	WriteFile(hFile, &ObjectType, sizeof(TCHAR), &dwWriteSize, 0);

	WriteFile(hFile, &horizontal, sizeof(BYTE), &dwWriteSize, 0);
	WriteFile(hFile, &vertical, sizeof(BYTE), &dwWriteSize, 0);
}

void effectblur::open(HANDLE hFile)
{
	DWORD dwReadSize;

	ReadFile(hFile, &horizontal, sizeof(BYTE), &dwReadSize, 0);
	ReadFile(hFile, &vertical, sizeof(BYTE), &dwReadSize, 0);
}

void effectblur::blur(Gdiplus::Bitmap*pBitmap) const
{
	util::DoGaussianBlur(pBitmap, horizontal / 2.0f, vertical / 2.0f);
}