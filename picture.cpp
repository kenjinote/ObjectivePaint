#include <Windows.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include "math.h"
#include "myObjectList.h"
#include "picture.h"
#include "PictureFrame.h"
#include "effectlist.h"
#include "effect.h"
#include "util.h"

extern PictureFrame* picfrm;
extern myObjectList* pDataList;

picture::picture() :myObject(), m_pBitmap(0), m_hImageOriginal(0), alpha(100.0f), m_bSmoothing(TRUE), m_nFrameNumber(0)
{
}

picture::~picture()
{
}

picture::picture(const picture& other) : myObject((myObject&)other)
{
	m_bSmoothing = other.m_bSmoothing;
	m_pBitmap = other.m_pBitmap;
	m_hImageOriginal = other.m_hImageOriginal;
	alpha = other.alpha;
	m_nFrameNumber = other.m_nFrameNumber;
}

myObject* picture::Copy() const
{
	return (myObject*)new picture(*this);
}

void picture::draw(Gdiplus::Graphics* g, bool isprinting) const
{
	if (!this) return;

	if (!m_bSelected || isprinting)
	{
		if (m_nFrameNumber > 0)
		{
			Gdiplus::Region* mask = picfrm->getFrameDataAt(m_nFrameNumber - 1);
			Gdiplus::Matrix mx;
			g->GetTransform(&mx);
			g->ScaleTransform(MASK_SCALE, MASK_SCALE);
			g->SetClip(mask);
			g->SetTransform(&mx);
		}
	}

	Gdiplus::Matrix mx;
	g->GetTransform(&mx);
	g->TranslateTransform(x + width / 2.0f, y + height / 2.0f);
	g->RotateTransform(angle);
	Gdiplus::Matrix mx2;
	g->GetTransform(&mx2);
	g->ScaleTransform(bLeftRight ? -width_scale : width_scale, bUpDown ? -height_scale : height_scale);

	const Gdiplus::InterpolationMode nOldInterpolationMode = g->GetInterpolationMode();
	if (m_bSmoothing || isprinting)
	{
		g->SetInterpolationMode(Gdiplus::InterpolationModeDefault); //Gdiplus::SmoothingModeAntiAlias
	}
	else
	{
		g->SetInterpolationMode(Gdiplus::InterpolationModeLowQuality); //Gdiplus::SmoothingModeHighSpeed
	}

	Gdiplus::ImageAttributes attr;
	const Gdiplus::ColorMatrix cmat = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, alpha / 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	attr.SetColorMatrix(&cmat);
	Gdiplus::RectF rect(-width / 2.0f, -height / 2.0f, width, height);
	if (!effectList || effectList->size() < 1)
	{
		if (isprinting)
		{
			Gdiplus::Bitmap *pBitmap = util::GetImageFromByteData(m_hImageOriginal);
			g->DrawImage(pBitmap, rect, 0, 0, (Gdiplus::REAL)pBitmap->GetWidth(), (Gdiplus::REAL)pBitmap->GetHeight(), Gdiplus::UnitPixel, &attr, 0, 0);
			delete pBitmap;
		}
		else
		{
			g->DrawImage(m_pBitmap, rect, 0, 0, (Gdiplus::REAL)m_pBitmap->GetWidth(), (Gdiplus::REAL)m_pBitmap->GetHeight(), Gdiplus::UnitPixel, &attr, 0, 0);
		}
	}
	else
	{
		Gdiplus::RectF rect2(width / 2.0f, height / 2.0f, width, height);
		if (effectList->IsIncludedBlur())
		{
			if (!pTempBitmapBack)
			{
				delete pTempBitmapBack;
				pTempBitmapBack = new Gdiplus::Bitmap((int)(width * 2.0f * 2.0f), (int)(height * 2.0f * 2.0f), PixelFormat32bppARGB);
				Gdiplus::Graphics g(pTempBitmapBack);
				g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
				g.Clear(Gdiplus::Color(0, 0, 0, 0));
				g.ScaleTransform(2.0f, 2.0f);
				{
					Gdiplus::GraphicsPath path;
					path.AddRectangle(rect2);
					effectList->draw(&g, &path, effect::Before, width, height);
					if (isprinting)
					{
						Gdiplus::Bitmap *pBitmap = util::GetImageFromByteData(m_hImageOriginal);
						g.DrawImage(pBitmap, rect2, 0, 0, (Gdiplus::REAL)pBitmap->GetWidth(), (Gdiplus::REAL)pBitmap->GetHeight(), Gdiplus::UnitPixel, &attr, 0, 0);
						delete pBitmap;
					}
					else
					{
						g.DrawImage(m_pBitmap, rect2, 0, 0, (Gdiplus::REAL)m_pBitmap->GetWidth(), (Gdiplus::REAL)m_pBitmap->GetHeight(), Gdiplus::UnitPixel, &attr, 0, 0);
					}
					effectList->draw(&g, &path, effect::After, width, height);
					effectList->blur(pTempBitmapBack);
				}
			}
			g->DrawImage(pTempBitmapBack, -width, -height, width * 2, height * 2);
		}
		else
		{
			if (!pTempBitmapBack)
			{
				delete pTempBitmapBack;
				pTempBitmapBack = new Gdiplus::Bitmap((int)(width * 2.0f * 2.0f), (int)(height * 2.0f * 2.0f), PixelFormat32bppARGB);
				Gdiplus::Graphics g(pTempBitmapBack);
				g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
				g.Clear(Gdiplus::Color(0, 0, 0, 0));
				g.ScaleTransform(2.0f, 2.0f);
				{
					Gdiplus::GraphicsPath path;
					path.AddRectangle(rect2);
					effectList->draw(&g, &path, effect::Before, width, height);
				}
			}
			if (!pTempBitmapFront)
			{
				delete pTempBitmapFront;
				pTempBitmapFront = new Gdiplus::Bitmap((int)(width * 2.0f * 2.0f), (int)(height * 2.0f * 2.0f), PixelFormat32bppARGB);
				Gdiplus::Graphics g(pTempBitmapFront);
				g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
				g.Clear(Gdiplus::Color(0, 0, 0, 0));
				g.ScaleTransform(2.0f, 2.0f);
				{
					Gdiplus::GraphicsPath path;
					path.AddRectangle(rect2);
					effectList->draw(&g, &path, effect::After, width, height);
				}
			}
			g->DrawImage(pTempBitmapBack, -width, -height, width * 2, height * 2);
			if (isprinting)
			{
				Gdiplus::Bitmap *pBitmap = util::GetImageFromByteData(m_hImageOriginal);
				g->DrawImage(pBitmap, rect, 0, 0, (Gdiplus::REAL)pBitmap->GetWidth(), (Gdiplus::REAL)pBitmap->GetHeight(), Gdiplus::UnitPixel, &attr, 0, 0);
				delete pBitmap;
			}
			else
			{
				g->DrawImage(m_pBitmap, rect, 0, 0, (Gdiplus::REAL)m_pBitmap->GetWidth(), (Gdiplus::REAL)m_pBitmap->GetHeight(), Gdiplus::UnitPixel, &attr, 0, 0);
			}
			g->DrawImage(pTempBitmapFront, -width, -height, width * 2, height * 2);
		}
	}

	g->SetTransform(&mx2);

	if (m_bSelected && !isprinting)
	{
		const Gdiplus::Color color(0, 153, 204); // ‘I‘ð˜g
		const Gdiplus::Pen pen(color, 1.5f);
		g->DrawEllipse(&pen, (-width / 2.0f * 2.0f + width) / 2.0f - 4.0f, -height / 2.0f * height_scale - 12.0f, 8.0f, 8.0f);
		g->DrawLine(&pen, (-width / 2.0f * 2.0f + width) / 2.0f, -height / 2.0f * height_scale - 12.0f + 8.0f, (-width / 2.0f * 2.0f + width) / 2.0f, -height / 2.0f * height_scale);
		g->DrawRectangle(&pen, -width / 2.0f * width_scale, -height / 2.0f * height_scale, width * width_scale, height * height_scale);
	}
	else
	{
		if (m_nFrameNumber > 0)
		{
			g->ResetClip();
		}
	}
	g->SetTransform(&mx);
	g->SetInterpolationMode(nOldInterpolationMode);
}

void picture::setbitmapfromoriginal(HGLOBAL hImageOriginal)
{
	Gdiplus::Bitmap* pBitmap = util::GetImageFromByteData(hImageOriginal);
	if (pBitmap)
	{
		width = (float)pBitmap->GetWidth();
		height = (float)pBitmap->GetHeight();
		const float f = 1748.0f / 4.0f; // 1748.0f / 4.0f;
		if (width > height)
		{
			if (width > f)
			{
				height = height * (f / width);
				width = f;
			}
		}
		else
		{
			if (height > f)
			{
				width = width * (f / height);
				height = f;
			}
		}
		m_pBitmap = (Gdiplus::Bitmap*)new Gdiplus::Bitmap((int)width, (int)height, pBitmap->GetPixelFormat());// pBitmap->Clone(0.0f, 0.0f, width, height, pBitmap->GetPixelFormat());
		{
			Gdiplus::Graphics g(m_pBitmap);
			g.DrawImage(pBitmap, 0.0f, 0.0f, width, height);
		}
		pDataList->addpicture(hImageOriginal, m_pBitmap);
		delete pBitmap;
	}
}

BOOL picture::LoadPicture(LPCTSTR lpszFilePath)
{
	if (!this) return FALSE;
	if (lpszFilePath && PathFileExists(lpszFilePath))
	{
		HANDLE hFile = CreateFile(lpszFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		DWORD dwReadSize;
		DWORD dwImageSize;
		dwImageSize = GetFileSize(hFile, 0);
		if (dwImageSize)
		{
			m_hImageOriginal = GlobalAlloc(0, dwImageSize);
			ReadFile(hFile, m_hImageOriginal, dwImageSize, &dwReadSize, 0);
		}
		CloseHandle(hFile);

		if (m_hImageOriginal)
		{
			setbitmapfromoriginal(m_hImageOriginal);
			return TRUE;
		}
	}
	return FALSE;
}

void picture::save(HANDLE hFile) const
{
	DWORD dwWriteSize;
	TCHAR ObjectType = TEXT('P');
	WriteFile(hFile, &ObjectType, sizeof(TCHAR), &dwWriteSize, 0);

	myObject::save(hFile);

	WriteFile(hFile, &m_bSmoothing, sizeof(BOOL), &dwWriteSize, 0);
	WriteFile(hFile, &alpha, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	WriteFile(hFile, &m_nFrameNumber, sizeof(INT), &dwWriteSize, 0);
	util::SaveImageByteData(hFile, m_hImageOriginal);
}

void picture::open(HANDLE hFile)
{
	DWORD dwReadSize;

	myObject::open(hFile);

	ReadFile(hFile, &m_bSmoothing, sizeof(BOOL), &dwReadSize, 0);
	ReadFile(hFile, &alpha, sizeof(Gdiplus::REAL), &dwReadSize, 0);
	ReadFile(hFile, &m_nFrameNumber, sizeof(INT), &dwReadSize, 0);
	util::OpenImageByteData(hFile, &m_hImageOriginal);
	if (m_hImageOriginal)
	{
		setbitmapfromoriginal(m_hImageOriginal);
	}
}
