#include <Windows.h>
#include <Gdiplus.h>
#include <vector>
#include "myObject.h"
#include "myObjectList.h"
#include "string.h"
#include "picture.h"
#include "util.h"

myObjectList::myObjectList()
{
	m_uViewGen = 0;
	m_uCurrGen = 0;
}

myObjectList::~myObjectList()
{
	clear();
}

void myObjectList::clear()
{
	for (myObject* obj : list)
	{
		delete obj;
	}
	list.clear();
	for (HGLOBAL hGlobal : m_listPictureHandlePool)
	{
		GlobalFree(hGlobal);
	}
	m_listPictureHandlePool.clear();
	for (Gdiplus::Bitmap* bmp : m_listPictureBitmapPool)
	{
		delete bmp;
	}
	m_listPictureBitmapPool.clear();
}

UINT myObjectList::GetViewGen() const
{
	return m_uViewGen;
}

UINT myObjectList::GetCurrGen() const
{
	return m_uCurrGen;
}

BOOL myObjectList::undo()
{
	if (m_uViewGen > 0)
	{
		--m_uViewGen;
		for (myObject* obj : list)
		{
			if (!obj->IsAlive(this))
			{
				obj->deleteTemp();
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL myObjectList::redo()
{
	if (m_uViewGen < m_uCurrGen)
	{
		++m_uViewGen;
		for (myObject* obj : list)
		{
			if (!obj->IsAlive(this))
			{
				obj->deleteTemp();
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL myObjectList::IsUndoAvailable() const
{
	return (m_uViewGen > 0) ? TRUE : FALSE;
}

BOOL myObjectList::IsRedoAvailable() const
{
	return (m_uViewGen < m_uCurrGen) ? TRUE : FALSE;
}

void myObjectList::reset()
{
	m_uViewGen = 0;
	m_uCurrGen = 0;
}

void myObjectList::add(myObject* newData)
{
	return list.push_back(newData);
}

void myObjectList::EditStart()
{
	if (m_uViewGen < m_uCurrGen)
	{
		for (myObject* obj : list)
		{
			obj->Resurrect(this);
			obj->Bury(this);
		}
		m_uCurrGen = m_uViewGen + 1;
	}
	else
	{
		++m_uCurrGen;
	}
	m_uViewGen = m_uCurrGen;
}

void myObjectList::newlife(myObjectList* pOrg)
{
	for (myObject* obj : list)
	{
		obj->newlife(pOrg);
	}
}

void myObjectList::print(HDC hdc, float x, float y, float nImageWidth, float nImageHeight, float dScale, Gdiplus::Bitmap* pBitmapBack, Gdiplus::Bitmap* pBitmapFront, bool isprinting) const
{
	Gdiplus::Graphics g(hdc);

	g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	g.TranslateTransform(x, y);
	g.ScaleTransform(dScale, dScale);

	// 以下の塗りつぶしは背景に透過領域がある場合だけでよい
	Gdiplus::RectF rectClip;
	if (isprinting)
	{
		rectClip.Width = nImageWidth;
		rectClip.Height = nImageHeight;
	}
	else
	{
		rectClip.Width = nImageWidth - 0.5f;
		rectClip.Height = nImageHeight - 0.5f;
		g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color::White), rectClip);
	}

	BOOL bIsPictureSelected = FALSE;
	BOOL bIsStringSelected = FALSE;

	for (const myObject* obj : list)
	{
		if (!obj->IsAlive(this))continue;
		if (!obj->IsPicture())continue;
		if (obj->IsMoving())continue;
		if (obj->IsSelected() && !isprinting)
		{
			obj->draw(&g, isprinting);
			bIsPictureSelected = TRUE;
		}
		else
		{
			// トリミング
			g.SetClip(rectClip);
			obj->draw(&g, isprinting);
			g.ResetClip();
		}
	}
	for (const myObject* obj : list)
	{
		if (!obj->IsAlive(this))continue;
		if (!obj->IsPicture())continue;
		if (!obj->IsMoving())continue;
		obj->draw(&g, isprinting);
		bIsPictureSelected = TRUE;
	}
	if (pBitmapBack)
	{
		if (bIsPictureSelected && !isprinting)
		{
			Gdiplus::ImageAttributes attr;
			Gdiplus::ColorMatrix cmat = {
				1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
				0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
				0.0f, 0.0f, 0.0f, 0.7f, 0.0f,   // Alpha (70%)
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
			};
			attr.SetColorMatrix(&cmat);
			g.DrawImage(pBitmapBack, Gdiplus::RectF(0.0f, 0.0f, nImageWidth, nImageHeight), 0, 0, (float)pBitmapBack->GetWidth(), (float)pBitmapBack->GetHeight(), Gdiplus::UnitPixel, &attr, 0, 0);
		}
		else
		{
			if (isprinting)
			{
				g.DrawImage(pBitmapBack, Gdiplus::RectF(0.0f, 0.0f, nImageWidth / dScale, nImageHeight / dScale), 0, 0, (float)pBitmapBack->GetWidth(), (float)pBitmapBack->GetHeight(), Gdiplus::UnitPixel);
			}
			else
			{
				g.DrawImage(pBitmapBack, Gdiplus::RectF(0.0f, 0.0f, nImageWidth, nImageHeight), 0, 0, (float)pBitmapBack->GetWidth(), (float)pBitmapBack->GetHeight(), Gdiplus::UnitPixel);
			}
		}
	}
	for (const myObject* obj : list)
	{
		if (!obj->IsAlive(this))continue;
		if (!obj->IsString())continue;
		if (obj->IsMoving())continue;
		if (obj->IsSelected() && !isprinting)
		{
			obj->draw(&g, isprinting);
			bIsStringSelected = TRUE;
		}
		else
		{
			// トリミング
			g.SetClip(rectClip);
			obj->draw(&g, isprinting);
			g.ResetClip();
		}
	}
	for (const myObject* obj : list)
	{
		if (!obj->IsAlive(this))continue;
		if (!obj->IsString())continue;
		if (!obj->IsMoving())continue;
		obj->draw(&g, isprinting);
		bIsStringSelected = TRUE;
	}
	if (pBitmapFront)
	{
		if ((bIsPictureSelected || bIsStringSelected) && !isprinting)
		{
			Gdiplus::ImageAttributes attr;
			Gdiplus::ColorMatrix cmat = {
				1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Red
				0.0f, 1.0f, 0.0f, 0.0f, 0.0f,   // Green
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // Blue
				0.0f, 0.0f, 0.0f, 0.7f, 0.0f,   // Alpha (70%)
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f    // must be 1
			};
			attr.SetColorMatrix(&cmat);
			g.DrawImage(pBitmapFront, Gdiplus::RectF(0, 0, nImageWidth, nImageHeight), 0, 0, (float)pBitmapFront->GetWidth(), (float)pBitmapFront->GetHeight(), Gdiplus::UnitPixel, &attr, 0, 0);
		}
		else
		{
			if (isprinting)
			{
				g.DrawImage(pBitmapFront, Gdiplus::RectF(0.0f, 0.0f, nImageWidth / dScale, nImageHeight / dScale), 0, 0, (float)pBitmapFront->GetWidth(), (float)pBitmapFront->GetHeight(), Gdiplus::UnitPixel);
			}
			else
			{
				g.DrawImage(pBitmapFront, Gdiplus::RectF(0.0f, 0.0f, nImageWidth, nImageHeight), 0, 0, (float)pBitmapFront->GetWidth(), (float)pBitmapFront->GetHeight(), Gdiplus::UnitPixel);
			}
		}
	}
}

void myObjectList::allunselect()
{
	for (myObject* obj : list)
	{
		obj->SetSelect(FALSE);
	}
}

BOOL myObjectList::savefile(LPCTSTR lpszFilePath, Gdiplus::Bitmap* pBitmapBack, Gdiplus::Bitmap* pBitmapFront, std::vector<Gdiplus::Bitmap*>*listMask) const
{
	DWORD dwWriteSize;
	HANDLE hFile = CreateFile(lpszFilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//サムネイル保存
	if (pBitmapBack)
	{
		float w = (float)pBitmapBack->GetWidth();
		float h = (float)pBitmapBack->GetHeight();
		float scale = 98.0f / max(w, h);
		w *= scale;
		h *= scale;
		float x, y;
		if (w > h)
		{
			x = 6.0f;
			y = 49.0f - h / 2.0f + 6.0f;
		}
		else
		{
			x = 49.0f - w / 2.0f + 6.0f;
			y = 6.0f;
		}
		Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(110, 110,/*pBitmapBack->GetWidth(), pBitmapBack->GetHeight(),*/ PixelFormat32bppARGB);
		Gdiplus::Bitmap* pBitmapBig = new Gdiplus::Bitmap(pBitmapBack->GetWidth(), pBitmapBack->GetHeight(), PixelFormat32bppARGB);

		{
			Gdiplus::Graphics g(pBitmapBig);
			HDC hdc = g.GetHDC();
			print(hdc, 0, 0, (float)pBitmapBack->GetWidth(), (float)pBitmapBack->GetHeight(), 1.0, pBitmapBack, pBitmapFront);
			g.ReleaseHDC(hdc);
		}

		{
			Gdiplus::Graphics g(pBitmap);

			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

			g.Clear(Gdiplus::Color(0, 255, 255, 255));

			Gdiplus::GraphicsPath path;
			path.AddRectangle(Gdiplus::RectF(x + 1, y + 1, w, h));
			g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(6 * 10, 0, 0, 0)), &path);
			for (int i = 1; i < 6; ++i)
			{
				Gdiplus::Pen pen(Gdiplus::Color(10, 0, 0, 0), (Gdiplus::REAL)(i));
				pen.SetLineJoin(Gdiplus::LineJoin::LineJoinRound);
				g.DrawPath(&pen, &path);
			}

			g.DrawImage(pBitmapBig, x, y, w, h);
		}
		util::SaveImage(hFile, pBitmap);
		delete pBitmap;
		delete pBitmapBig;
	}
	else
	{
		DWORD size = 0;
		WriteFile(hFile, &size, sizeof(DWORD), &dwWriteSize, 0);
	}

	//背景保存
	util::SaveImage(hFile, pBitmapBack);

	//前景保存
	util::SaveImage(hFile, pBitmapFront);

	//マスク画像の数
	const DWORD nMaskCount = (DWORD)(listMask ? listMask->size() : 0);
	WriteFile(hFile, &nMaskCount, sizeof(DWORD), &dwWriteSize, 0);
	//マスク画像
	if (listMask)
	{
		for (auto mask : *listMask)
		{
			util::SaveImage(hFile, mask);
		}
	}

	// オブジェクト保存

	// オブジェクトの数を保存
	const DWORD nObjectCount = (DWORD)list.size();
	WriteFile(hFile, &nObjectCount, sizeof(DWORD), &dwWriteSize, 0);

	for (const myObject* obj : list)
	{
		if (!obj->IsAlive(this))continue;
		obj->save(hFile);
	}

	CloseHandle(hFile);
	return TRUE;
}

BOOL myObjectList::openfile(LPCTSTR lpszFilePath, Gdiplus::Bitmap** pBitmapBack, Gdiplus::Bitmap** pBitmapFront, std::vector<Gdiplus::Bitmap*>*listMask)
{
	DWORD dwReadSize;
	HANDLE hFile = CreateFile(lpszFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//サムネイルは無視
	{
		DWORD size;
		ReadFile(hFile, &size, sizeof(DWORD), &dwReadSize, 0);
		SetFilePointer(hFile, size, NULL, FILE_CURRENT);
	}

	//背景を開く
	delete *pBitmapBack;
	*pBitmapBack = NULL;
	util::OpenImage(hFile, pBitmapBack);

	//前景を開く
	delete *pBitmapFront;
	*pBitmapFront = NULL;
	util::OpenImage(hFile, pBitmapFront);

	//マスク画像の数
	DWORD nMaskCount;
	ReadFile(hFile, &nMaskCount, sizeof(DWORD), &dwReadSize, 0);
	//マスク画像
	if (listMask)
	{
		for (DWORD i = 0; i < nMaskCount; ++i)
		{
			Gdiplus::Bitmap* pBitmap;
			util::OpenImage(hFile, &pBitmap);
			listMask->push_back(pBitmap);
		}
	}

	clear();
	reset();

	// オブジェクトの数を読み込む
	DWORD nObjectCount;
	ReadFile(hFile, &nObjectCount, sizeof(DWORD), &dwReadSize, 0);

	TCHAR ObjectType;
	for (DWORD i = 0; i < nObjectCount; ++i)
	{
		ObjectType = TEXT('\0');

		const BOOL bRead = ReadFile(hFile, &ObjectType, sizeof(TCHAR), &dwReadSize, 0);
		if (!bRead)break;

		myObject* obj = 0;
		if (ObjectType == TEXT('S'))
		{
			obj = new string;
		}
		else if (ObjectType == TEXT('P'))
		{
			obj = new picture;
		}
		if (obj)
		{
			obj->open(hFile);
			add(obj);
		}
	}

	CloseHandle(hFile);
	return TRUE;
}

int myObjectList::getpicturecount() const
{
	int count = 0;
	for (const myObject* obj : list)
	{
		if (!obj->IsAlive(this))continue;
		if (obj->IsPicture()) ++count;
	}
	return count;
}

int myObjectList::getstringcount() const
{
	int count = 0;
	for (const myObject* obj : list)
	{
		if (!obj->IsAlive(this))continue;
		if (!obj->IsAlive(this))continue;
		if (obj->IsString()) ++count;
	}
	return count;
}

void myObjectList::addpicture(HGLOBAL hImageOriginal, Gdiplus::Bitmap* pBitmap)
{
	m_listPictureHandlePool.push_back(hImageOriginal);
	m_listPictureBitmapPool.push_back(pBitmap);
}