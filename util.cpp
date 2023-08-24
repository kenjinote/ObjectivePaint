#include <windows.h>
#include <shlwapi.h>
#include <gdiplus.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "myObjectList.h"
#include "util.h"

Gdiplus::Bitmap* util::loadpng(int nID)
{
	Gdiplus::Bitmap* pImage = 0;
	const HMODULE hInst = GetModuleHandle(0);
	const HRSRC hResource = FindResource(hInst, MAKEINTRESOURCE(nID), TEXT("PNG"));
	if (!hResource)
		return 0;
	const DWORD dwImageSize = SizeofResource(hInst, hResource);
	if (!dwImageSize)
		return 0;
	const void* pResourceData = LockResource(LoadResource(hInst, hResource));
	if (!pResourceData)
		return 0;
	HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, dwImageSize);
	if (hBuffer)
	{
		void* pBuffer = GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, dwImageSize);
			IStream* pStream = NULL;
			if (CreateStreamOnHGlobal(hBuffer, TRUE, &pStream) == S_OK)
			{
				pImage = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
				if (pImage)
				{
					if (pImage->GetLastStatus() != Gdiplus::Ok)
					{
						delete pImage;
						pImage = NULL;
					}
				}
			}
			::GlobalUnlock(hBuffer);
		}
	}
	return pImage;
}

// 線上に点がのっかってるか
BOOL util::IsOnLine(const POINT* Begin, const POINT* End, const POINT* Point)
{
	double	y1, y2, sqrt_d, d, h, x, y;
	const double x0 = Point->x;
	const double y0 = Point->y;
	const double x1 = Begin->x;	y1 = Begin->y;
	const double x2 = End->x;	y2 = End->y;
	const double a = x2 - x1;
	const double b = y2 - y1;
	if (IsEqual(a, 0.0))
	{
		// Ｘ座標が同じ
		if (b == 0.0)	return FALSE;	// 始点と終点が同じ
		h = fabs(x0 - x1);
		x = x1;
		y = y0;
	}
	else if (IsEqual(b, 0.0))
	{
		// Ｙ座標が同じ
		h = fabs(y0 - y1);
		x = x0;
		y = y1;
	}
	else
	{
		sqrt_d = _hypot(a, b);
		d = sqrt_d * sqrt_d;
		if (d < 1.0e-10) return FALSE;
		h = fabs((a * (y1 - y0) - b * (x1 - x0)) / sqrt_d);
		x = (a * a * x0 + a * b * (y0 - y1) + b * b * x1) / d;
		y = (a * a * y1 + a * b * (x0 - x1) + b * b * y0) / d;
	}
	if (!IsEqual(h, 0.0)) return FALSE;
	double s1 = x - x1;
	double s2 = x2 - x;
	if (IsEqual(s1, 0.0) && IsEqual(s2, 0.0))
	{
		s1 = y - y1;	s2 = y2 - y;
	}
	if (((s1 >= 0.0 || IsEqual(s1, 0.0)) && (s2 >= 0.0 || IsEqual(s2, 0.0)))
		|| ((s1 <= 0.0 || IsEqual(s1, 0.0)) && (s2 <= 0.0 || IsEqual(s2, 0.0))))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL util::IsEqual(double a, double b)
{
	return (fabs(a - b) < 5.0);
}

HICON util::CreateArrowCursor(float angle)
{
	HDC hMemDC;
	BITMAPV5HEADER bi;
	HBITMAP hBitmap, hOldBitmap;
	void *lpBits;
	DWORD x, y;
	HICON hAlphaCursor = NULL;
	const DWORD dwWidth = 32;
	const DWORD dwHeight = 32;
	ZeroMemory(&bi, sizeof(BITMAPV5HEADER));
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = dwWidth;
	bi.bV5Height = dwHeight;
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	bi.bV5AlphaMask = 0xFF000000;
	HDC hdc = GetDC(NULL);
	hBitmap = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
		(void **)&lpBits, NULL, (DWORD)0);
	hMemDC = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	PatBlt(hMemDC, 0, 0, dwWidth, dwHeight, WHITENESS);
	SetBkMode(hMemDC, TRANSPARENT);
	RECT rect = { 0, 0, dwWidth, dwHeight };
	DrawArrow(hMemDC, angle, 18);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
	HBITMAP hMonoBitmap = CreateBitmap(dwWidth, dwHeight, 1, 1, NULL);
	DWORD *lpdwPixel;
	lpdwPixel = (DWORD *)lpBits;
	for (x = 0; x<dwWidth; ++x)
	{
		for (y = 0; y<dwHeight; ++y)
		{
			if (*lpdwPixel == 0xFFFFFFFF)
			{
				*lpdwPixel &= 0x00FFFFFF;
			}
			++lpdwPixel;
		}
	}
	ICONINFO ii;
	ii.fIcon = FALSE;
	ii.xHotspot = 9;
	ii.yHotspot = 9;
	ii.hbmMask = hMonoBitmap;
	ii.hbmColor = hBitmap;
	hAlphaCursor = CreateIconIndirect(&ii);
	DeleteObject(hBitmap);
	DeleteObject(hMonoBitmap);
	return hAlphaCursor;
}

void util::DrawArrow(HDC hdc, float angle, int size)
{
	const float radian = angle * (float)M_PI / 180.0f;
	const float half = size / 2.0f - 1.0f;
	const float half3 = sqrtf(2.0f) * (half / 2.0f);
	const float half4 = 1.0f / cos((float)M_PI / 12.0f) * (half / 2.0f);
	const Gdiplus::PointF center(half, half);
	const Gdiplus::PointF start(center.X + half*cosf(radian), center.Y + half*sinf(radian));
	Gdiplus::PointF point[10];
	point[0] = start;
	point[1].X = center.X + half3*cosf(radian + (float)M_PI / 4.0f);
	point[1].Y = center.Y + half3*sinf(radian + (float)M_PI / 4.0f);
	point[9].X = center.X + half3*cosf(radian - (float)M_PI / 4.0f);
	point[9].Y = center.Y + half3*sinf(radian - (float)M_PI / 4.0f);
	point[2].X = center.X + half4*cosf(radian + (float)M_PI / 12.0f);
	point[2].Y = center.Y + half4*sinf(radian + (float)M_PI / 12.0f);
	point[8].X = center.X + half4*cosf(radian - (float)M_PI / 12.0f);
	point[8].Y = center.Y + half4*sinf(radian - (float)M_PI / 12.0f);
	point[5].X = center.X - half*cosf(radian);
	point[5].Y = center.Y - half*sinf(radian);
	point[4].X = center.X - half3*cosf(radian - (float)M_PI / 4.0f);
	point[4].Y = center.Y - half3*sinf(radian - (float)M_PI / 4.0f);
	point[6].X = center.X - half3*cosf(radian + (float)M_PI / 4.0f);
	point[6].Y = center.Y - half3*sinf(radian + (float)M_PI / 4.0f);
	point[3].X = center.X - half4*cosf(radian - (float)M_PI / 12.0f);
	point[3].Y = center.Y - half4*sinf(radian - (float)M_PI / 12.0f);
	point[7].X = center.X - half4*cosf(radian + (float)M_PI / 12.0f);
	point[7].Y = center.Y - half4*sinf(radian + (float)M_PI / 12.0f);
	Gdiplus::Graphics g(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(0, 0, 0), 1);
	g.FillPolygon(&Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 254)), point, 10);
	g.DrawPolygon(&pen, point, 10);
}

int util::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0, size = 0;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0) return -1;

	Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)malloc(size);
	if (pImageCodecInfo == NULL) return -1;

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

void util::SaveImageByteData(HANDLE hFile, const HGLOBAL hGlobal)
{
	DWORD dwWriteSize;
	if (hGlobal)
	{
		const DWORD dwImageSize = (DWORD)GlobalSize(hGlobal);
		WriteFile(hFile, &dwImageSize, sizeof(DWORD), &dwWriteSize, 0);
		WriteFile(hFile, hGlobal, dwImageSize, &dwWriteSize, 0);
	}
	else
	{
		DWORD dwImageSize = 0;
		WriteFile(hFile, &dwImageSize, sizeof(DWORD), &dwWriteSize, 0);
	}
}

void util::OpenImageByteData(HANDLE hFile, HGLOBAL* lphGlobal)
{
	DWORD dwReadSize;
	DWORD dwImageSize;
	ReadFile(hFile, &dwImageSize, sizeof(DWORD), &dwReadSize, 0);
	*lphGlobal = GlobalAlloc(0, dwImageSize);
	ReadFile(hFile, *lphGlobal, dwImageSize, &dwReadSize, 0);
}

Gdiplus::Bitmap* util::GetImageFromByteData(HGLOBAL hGlobal)
{
	Gdiplus::Bitmap* pBtimap = 0;
	IStream* pStream = NULL;
	if (CreateStreamOnHGlobal(hGlobal, FALSE, &pStream) == S_OK)
	{
		pBtimap = Gdiplus::Bitmap::FromStream(pStream);
		pStream->Release();
	}
	return pBtimap;
}

void util::SaveImage(HANDLE hFile, Gdiplus::Bitmap* pBitmap)
{
	DWORD dwWriteSize;
	if (pBitmap)
	{
		CLSID clsid;
		util::GetEncoderClsid(L"image/png", &clsid);
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, 0);
		if (hGlobal)
		{
			IStream* pStream = NULL;
			if (CreateStreamOnHGlobal(hGlobal, FALSE, &pStream) == S_OK)
			{
				pBitmap->Save(pStream, &clsid);
				void* pBuffer = GlobalLock(hGlobal);
				const DWORD dwImageSize = (DWORD)GlobalSize(hGlobal);
				WriteFile(hFile, &dwImageSize, sizeof(DWORD), &dwWriteSize, 0);
				WriteFile(hFile, pBuffer, dwImageSize, &dwWriteSize, 0);
				pStream->Release();
				GlobalUnlock(hGlobal);
				GlobalFree(hGlobal);
			}
		}
	}
	else
	{
		DWORD dwImageSize = 0;
		WriteFile(hFile, &dwImageSize, sizeof(DWORD), &dwWriteSize, 0);
	}
}

void util::OpenImage(HANDLE hFile, Gdiplus::Bitmap** ppBitmap)
{
	DWORD dwReadSize;
	DWORD dwImageSize;
	ReadFile(hFile, &dwImageSize, sizeof(DWORD), &dwReadSize, 0);
	LPBYTE pBuffer1 = (LPBYTE)GlobalAlloc(0, dwImageSize);
	ReadFile(hFile, pBuffer1, dwImageSize, &dwReadSize, 0);
	IStream* pStream = NULL;
	if (CreateStreamOnHGlobal(pBuffer1, TRUE, &pStream) == S_OK)
	{
		*ppBitmap = Gdiplus::Bitmap::FromStream(pStream);
		pStream->Release();
	}
}

void util::convertfiles(TCHAR *pszBasePath, void(*func)(LPCTSTR))
{
	TCHAR szSearchPath[MAX_PATH + 1];
	TCHAR szSubPath[MAX_PATH + 1];
	TCHAR szFileName[MAX_PATH + 1];
	WIN32_FIND_DATA fd;
	lstrcpy(szSearchPath, pszBasePath);
	if (szSearchPath[lstrlen(szSearchPath) - 1] != TEXT('*'))
	{
		lstrcat(szSearchPath, TEXT("*"));
	}
	HANDLE hFind = FindFirstFile(szSearchPath, &fd);
	do
	{
		if (lstrcmpi(fd.cFileName, TEXT("..")) != 0 &&
			lstrcmpi(fd.cFileName, TEXT(".")) != 0)
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				lstrcpy(szSubPath, pszBasePath);
				lstrcat(szSubPath, fd.cFileName);
				lstrcat(szSubPath, TEXT("\\"));
				convertfiles(szSubPath, func);
			}
			else
			{
				lstrcpy(szFileName, pszBasePath);
				lstrcat(szFileName, fd.cFileName);
				if (lstrcmpi(PathFindExtension(szFileName), TEXT(".png")) == 0)
				{
					func(szFileName);
				}
			}
		}
	} while (FindNextFile(hFind, &fd));
	FindClose(hFind);
}

void util::converter(LPCTSTR lpszInputFilePath)
{
	LPTSTR p;
	if ((p = StrStr(lpszInputFilePath, TEXT("back.png"))) != NULL)
	{
		TCHAR szFrontFilePath[MAX_PATH];
		lstrcpy(szFrontFilePath, lpszInputFilePath);
		szFrontFilePath[p - lpszInputFilePath] = TEXT('\0');
		lstrcat(szFrontFilePath, TEXT("chara.png"));
		TCHAR szSaveFilePath[MAX_PATH];
		lstrcpy(szSaveFilePath, lpszInputFilePath);
		szSaveFilePath[p - lpszInputFilePath - 1] = TEXT('\0');
		lstrcat(szSaveFilePath, TEXT(".dcpw"));
		Gdiplus::Bitmap* pBitmapBack = Gdiplus::Bitmap::FromFile((LPCTSTR)lpszInputFilePath);
		Gdiplus::Bitmap* pBitmapFront = Gdiplus::Bitmap::FromFile((LPCTSTR)szFrontFilePath);
		std::vector<Gdiplus::Bitmap*> listMask;
		for (int i = 0;; ++i)
		{
			TCHAR szFrameFilePath[MAX_PATH];
			lstrcpy(szFrameFilePath, lpszInputFilePath);
			szFrameFilePath[p - lpszInputFilePath] = TEXT('\0');

			TCHAR szFrameWithNumber[MAX_PATH];
			wsprintf(szFrameWithNumber, TEXT("frame%d.png"), i + 1);
			lstrcat(szFrameFilePath, szFrameWithNumber);
			Gdiplus::Bitmap* pFrame = Gdiplus::Bitmap::FromFile((LPCTSTR)szFrameFilePath);
			if (!pFrame || pFrame->GetLastStatus() != Gdiplus::Ok)
			{
				delete pFrame;
				if (i == 0)
				{
					lstrcpy(szFrameFilePath, lpszInputFilePath);
					szFrameFilePath[p - lpszInputFilePath] = TEXT('\0');
					lstrcat(szFrameFilePath, TEXT("frame.png"));
					pFrame = Gdiplus::Bitmap::FromFile((LPCTSTR)szFrameFilePath);
					if (!pFrame || pFrame->GetLastStatus() != Gdiplus::Ok)
					{
						delete pFrame;
						break;
					}
				}
				else
				{
					break;
				}
			}
			Gdiplus::Bitmap* pBitmapFront = (Gdiplus::Bitmap*)pFrame->GetThumbnailImage((UINT)(pFrame->GetWidth()/* / 2.0*/), (UINT)(pFrame->GetHeight()/* / 2.0*/)); // ここは調整必要
			delete pFrame;
			Gdiplus::Bitmap* pgrayScale = pBitmapFront->Clone(0.0f, 0.0f, (float)pBitmapFront->GetWidth(), (float)pBitmapFront->GetHeight(), PixelFormat8bppIndexed);
			delete pBitmapFront;
			listMask.push_back(pgrayScale);
		}
		myObjectList* pDataList = new myObjectList;
		pDataList->clear();
		pDataList->reset();
		pDataList->savefile(szSaveFilePath, pBitmapBack, pBitmapFront, &listMask);
		for (auto mask : listMask)
		{
			delete mask;
		}
		listMask.clear();
		delete pBitmapBack;
		delete pBitmapFront;
	}
}

void util::png2pcp()
{
	TCHAR szFolderPath[MAX_PATH];
	GetModuleFileName(GetModuleHandle(0), szFolderPath, MAX_PATH);
	PathRemoveFileSpec(szFolderPath);
	PathAppend(szFolderPath, TEXT("templates"));
	PathAddBackslash(szFolderPath);
	convertfiles(szFolderPath, converter);
}

void util::GetIniFilePath(LPTSTR lpszIniFilePath)
{
	GetModuleFileName(GetModuleHandle(0), lpszIniFilePath, MAX_PATH);
	LPTSTR p = PathFindExtension(lpszIniFilePath);
	lstrcpy(p, TEXT(".ini"));
}

void util::GetProductName(LPTSTR lpszProductName, DWORD dwSize)
{
	TCHAR szIniFilePath[MAX_PATH];
	GetIniFilePath(szIniFilePath);
	GetPrivateProfileString(TEXT("AppInfo"), TEXT("ProductName"), TEXT("ディズニーカードプリント"), lpszProductName, dwSize, szIniFilePath);
}

void util::GetProductGuid(LPTSTR lpszGuid, DWORD dwSize)
{
	TCHAR szIniFilePath[MAX_PATH];
	GetIniFilePath(szIniFilePath);
	GetPrivateProfileString(TEXT("AppInfo"), TEXT("Guid"), TEXT("{6B44BA01-D412-47B7-9C25-7471B211F0A2}"), lpszGuid, dwSize, szIniFilePath);
}

inline void util::boxesForGauss(double sigma, int *sizes, int n)
{
	double wIdeal = sqrt((12 * sigma*sigma / n) + 1);
	int wl = (int)floor(wIdeal);
	if (wl % 2 == 0) --wl;

	const double wu = wl + 2;

	const double mIdeal = (12 * sigma*sigma - n*wl*wl - 4 * n*wl - 3 * n) / (-4 * wl - 4);
	const int m = myround(mIdeal);

	for (int i = 0; i<n; ++i)
		sizes[i] = int(i<m ? wl : wu);
}

inline void util::boxBlurH_4(BYTE* scl, BYTE* tcl, int w, int h, int r, int bpp, int stride)
{
	float iarr = (float)(1. / (r + r + 1));
	for (int i = 0; i<h; ++i)
	{
		int ti1 = i*stride;
		int ti2 = i*stride + 1;
		int ti3 = i*stride + 2;
		int ti4 = i*stride + 3;

		int li1 = ti1;
		int li2 = ti2;
		int li3 = ti3;
		int li4 = ti4;

		int ri1 = ti1 + r*bpp;
		int ri2 = ti2 + r*bpp;
		int ri3 = ti3 + r*bpp;
		int ri4 = ti4 + r*bpp;

		int fv1 = scl[ti1];
		int fv2 = scl[ti2];
		int fv3 = scl[ti3];
		int fv4 = scl[ti4];

		int lv1 = scl[ti1 + (w - 1)*bpp];
		int lv2 = scl[ti2 + (w - 1)*bpp];
		int lv3 = scl[ti3 + (w - 1)*bpp];
		int lv4 = scl[ti4 + (w - 1)*bpp];

		int val1 = (r + 1)*fv1;
		int val2 = (r + 1)*fv2;
		int val3 = (r + 1)*fv3;
		int val4 = (r + 1)*fv4;

		for (int j = 0; j<r; ++j)
		{
			val1 += scl[ti1 + j*bpp];
			val2 += scl[ti2 + j*bpp];
			val3 += scl[ti3 + j*bpp];
			val4 += scl[ti4 + j*bpp];
		}

		for (int j = 0; j <= r; ++j)
		{
			val1 += scl[ri1] - fv1;
			val2 += scl[ri2] - fv2;
			val3 += scl[ri3] - fv3;
			val4 += scl[ri4] - fv4;

			tcl[ti1] = myround(val1*iarr);
			tcl[ti2] = myround(val2*iarr);
			tcl[ti3] = myround(val3*iarr);
			tcl[ti4] = myround(val4*iarr);

			ri1 += bpp;
			ri2 += bpp;
			ri3 += bpp;
			ri4 += bpp;

			ti1 += bpp;
			ti2 += bpp;
			ti3 += bpp;
			ti4 += bpp;
		}

		for (int j = r + 1; j<w - r; ++j)
		{
			val1 += scl[ri1] - scl[li1];
			val2 += scl[ri2] - scl[li2];
			val3 += scl[ri3] - scl[li3];
			val4 += scl[ri4] - scl[li4];

			tcl[ti1] = myround(val1*iarr);
			tcl[ti2] = myround(val2*iarr);
			tcl[ti3] = myround(val3*iarr);
			tcl[ti4] = myround(val4*iarr);

			ri1 += bpp;
			ri2 += bpp;
			ri3 += bpp;
			ri4 += bpp;

			li1 += bpp;
			li2 += bpp;
			li3 += bpp;
			li4 += bpp;

			ti1 += bpp;
			ti2 += bpp;
			ti3 += bpp;
			ti4 += bpp;
		}

		for (int j = w - r; j<w; ++j)
		{
			val1 += lv1 - scl[li1];
			val2 += lv2 - scl[li2];
			val3 += lv3 - scl[li3];
			val4 += lv4 - scl[li4];

			tcl[ti1] = myround(val1*iarr);
			tcl[ti2] = myround(val2*iarr);
			tcl[ti3] = myround(val3*iarr);
			tcl[ti4] = myround(val4*iarr);

			li1 += bpp;
			li2 += bpp;
			li3 += bpp;
			li4 += bpp;

			ti1 += bpp;
			ti2 += bpp;
			ti3 += bpp;
			ti4 += bpp;
		}
	}
}

inline void util::boxBlurT_4(BYTE* scl, BYTE* tcl, int w, int h, int r, int bpp, int stride)
{
	float iarr = (float)(1.0f / (r + r + 1.0f));
	for (int i = 0; i<w; ++i)
	{
		int ti1 = i*bpp;
		int ti2 = i*bpp + 1;
		int ti3 = i*bpp + 2;
		int ti4 = i*bpp + 3;

		int li1 = ti1;
		int li2 = ti2;
		int li3 = ti3;
		int li4 = ti4;

		int ri1 = ti1 + r*stride;
		int ri2 = ti2 + r*stride;
		int ri3 = ti3 + r*stride;
		int ri4 = ti4 + r*stride;

		int fv1 = scl[ti1];
		int fv2 = scl[ti2];
		int fv3 = scl[ti3];
		int fv4 = scl[ti4];

		int lv1 = scl[ti1 + stride*(h - 1)];
		int lv2 = scl[ti2 + stride*(h - 1)];
		int lv3 = scl[ti3 + stride*(h - 1)];
		int lv4 = scl[ti4 + stride*(h - 1)];

		int val1 = (r + 1)*fv1;
		int val2 = (r + 1)*fv2;
		int val3 = (r + 1)*fv3;
		int val4 = (r + 1)*fv4;

		for (int j = 0; j<r; ++j)
		{
			val1 += scl[ti1 + j*stride];
			val2 += scl[ti2 + j*stride];
			val3 += scl[ti3 + j*stride];
			val4 += scl[ti4 + j*stride];
		}

		for (int j = 0; j <= r; ++j)
		{
			val1 += scl[ri1] - fv1;
			val2 += scl[ri2] - fv2;
			val3 += scl[ri3] - fv3;
			val4 += scl[ri4] - fv4;

			tcl[ti1] = myround(val1*iarr);
			tcl[ti2] = myround(val2*iarr);
			tcl[ti3] = myround(val3*iarr);
			tcl[ti4] = myround(val4*iarr);

			ri1 += stride;
			ri2 += stride;
			ri3 += stride;
			ri4 += stride;

			ti1 += stride;
			ti2 += stride;
			ti3 += stride;
			ti4 += stride;
		}

		for (int j = r + 1; j<h - r; ++j)
		{
			val1 += scl[ri1] - scl[li1];
			val2 += scl[ri2] - scl[li2];
			val3 += scl[ri3] - scl[li3];
			val4 += scl[ri4] - scl[li4];

			tcl[ti1] = myround(val1*iarr);
			tcl[ti2] = myround(val2*iarr);
			tcl[ti3] = myround(val3*iarr);
			tcl[ti4] = myround(val4*iarr);

			li1 += stride;
			li2 += stride;
			li3 += stride;
			li4 += stride;

			ri1 += stride;
			ri2 += stride;
			ri3 += stride;
			ri4 += stride;

			ti1 += stride;
			ti2 += stride;
			ti3 += stride;
			ti4 += stride;
		}

		for (int j = h - r; j<h; ++j)
		{
			val1 += lv1 - scl[li1];
			val2 += lv2 - scl[li2];
			val3 += lv3 - scl[li3];
			val4 += lv4 - scl[li4];

			tcl[ti1] = myround(val1*iarr);
			tcl[ti2] = myround(val2*iarr);
			tcl[ti3] = myround(val3*iarr);
			tcl[ti4] = myround(val4*iarr);

			li1 += stride;
			li2 += stride;
			li3 += stride;
			li4 += stride;

			ti1 += stride;
			ti2 += stride;
			ti3 += stride;
			ti4 += stride;
		}
	}
}

inline void util::boxBlur_4(BYTE* scl, BYTE* tcl, int w, int h, int rx, int ry, int bpp, int stride)
{
	memcpy(tcl, scl, stride*h);
	boxBlurH_4(tcl, scl, w, h, rx, bpp, stride);
	boxBlurT_4(scl, tcl, w, h, ry, bpp, stride);
}

inline void util::gaussBlur_4(BYTE* scl, BYTE* tcl, int w, int h, float rx, float ry, int bpp, int stride)
{
	int bxsX[4];
	boxesForGauss(rx, bxsX, 4);

	int bxsY[4];
	boxesForGauss(ry, bxsY, 4);

	boxBlur_4(scl, tcl, w, h, (bxsX[0] - 1) / 2, (bxsY[0] - 1) / 2, bpp, stride);
	boxBlur_4(tcl, scl, w, h, (bxsX[1] - 1) / 2, (bxsY[1] - 1) / 2, bpp, stride);
	boxBlur_4(scl, tcl, w, h, (bxsX[2] - 1) / 2, (bxsY[2] - 1) / 2, bpp, stride);
	boxBlur_4(scl, tcl, w, h, (bxsX[3] - 1) / 2, (bxsY[3] - 1) / 2, bpp, stride);
}

void util::DoGaussianBlur(Gdiplus::Bitmap* img, float radiusX, float radiusY)
{
	if (img == 0 || (radiusX == 0.0f && radiusY == 0.0f)) return;

	const int w = img->GetWidth();
	const int h = img->GetHeight();

	if (radiusX > w / 2)
	{
		radiusX = (float)(w / 2);
	}

	if (radiusY > h / 2)
	{
		radiusY = (float)(h / 2);
	}

	Gdiplus::Bitmap* temp = new Gdiplus::Bitmap(img->GetWidth(), img->GetHeight(), img->GetPixelFormat());

	Gdiplus::BitmapData bitmapData1;
	Gdiplus::BitmapData bitmapData2;
	Gdiplus::Rect rect(0, 0, img->GetWidth(), img->GetHeight());

	if (Gdiplus::Ok == img->LockBits(
		&rect,
		Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite,
		img->GetPixelFormat(),
		&bitmapData1
		)
		&&
		Gdiplus::Ok == temp->LockBits(
		&rect,
		Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite,
		temp->GetPixelFormat(),
		&bitmapData2
		))
	{
		BYTE* src = (BYTE*)bitmapData1.Scan0;
		BYTE* dst = (BYTE*)bitmapData2.Scan0;

		const int bpp = 4;
		const int stride = bitmapData1.Stride;

		gaussBlur_4(src, dst, w, h, radiusX, radiusY, bpp, stride);

		img->UnlockBits(&bitmapData1);
		temp->UnlockBits(&bitmapData2);
	}

	delete temp;
}

void util::DoGaussianBlurPower(Gdiplus::Bitmap* img, float radiusX, float radiusY, int nPower)
{
	Gdiplus::Bitmap* pBitmap = img->Clone(0, 0, img->GetWidth(), img->GetHeight(), PixelFormat32bppARGB);
	DoGaussianBlur(pBitmap, radiusX, radiusY);
	Gdiplus::Graphics g(pBitmap);
	for (int i = 0; i < 8; ++i)
	{
		g.DrawImage(pBitmap, 0, 0);
		if ((1 << i) & nPower)
		{
			Gdiplus::Graphics g(img);
			g.DrawImage(pBitmap, 0, 0);
		}
	}
	delete pBitmap;
}


Gdiplus::Region* util::CreateRegionFromImage(Gdiplus::Bitmap* image)
{
	Gdiplus::Region* region = new Gdiplus::Region();

	// リージョンを一旦空領域にする
	region->MakeEmpty();

	Gdiplus::BitmapData locked = { 0 };

	// 画像の各ピクセルを読み取り、α値が25%(0x40)未満の部分を透明とみなしてリージョンとする
	image->LockBits(&Gdiplus::Rect(0, 0, image->GetWidth(), image->GetHeight()),
		Gdiplus::ImageLockModeRead,
		image->GetPixelFormat(), &locked);

	for (unsigned int y = 0; y < image->GetHeight(); ++y)
	{
		Gdiplus::Rect opaque(0, y, 0, 1); // 連続する不透明ピクセル列の領域
		bool transparentPrev = false;
		bool transparentCurr = false;
		byte* pixel = (byte*)locked.Scan0 + y * locked.Stride;

		for (unsigned int x = 0; x < image->GetWidth(); ++x)
		{
			// このピクセルは白かどうか
			transparentCurr = (pixel[3] < 0x40);

			if (transparentCurr) {
				if (transparentPrev) {
					// 透明領域のまま
				}
				else {
					// このピクセルで透明領域に変わった
					if (0 < opaque.Width)
						// これまでの不透明ピクセル列の領域をリージョンに結合
						region->Union(opaque);
				}
			}
			else {
				if (transparentPrev) {
					// このピクセルで不透明領域に変わった
					// 不透明ピクセル列の開始点を初期化
					opaque.X = x;
					opaque.Width = 1;
				}
				else {
					// 不透明領域のまま
					// 不透明ピクセル列の幅を更新
					opaque.Width = x - opaque.X + 1;
				}
			}

			// 次のピクセルへ進む
			pixel += 4;

			// 直前のピクセルの透明/不透明の値を保存
			transparentPrev = transparentCurr;
		}

		// 最後のピクセルが不透明領域だった場合
		if (!transparentPrev && !transparentCurr)
			// これまでの不透明ピクセル列の領域をリージョンに結合
			region->Union(opaque);
	}
	image->UnlockBits(&locked);
	return region;
}

Gdiplus::Region* util::CreateRegionFromImage8bpp(Gdiplus::Bitmap* image)
{
	Gdiplus::Region* region = new Gdiplus::Region();

	// リージョンを一旦空領域にする
	region->MakeEmpty();

	Gdiplus::BitmapData locked = { 0 };

	// 画像の各ピクセルを読み取り、α値が25%(0x40)未満の部分を透明とみなしてリージョンとする
	image->LockBits(&Gdiplus::Rect(0, 0, image->GetWidth(), image->GetHeight()),
		Gdiplus::ImageLockModeRead,
		PixelFormat8bppIndexed, &locked);

	for (unsigned int y = 0; y < image->GetHeight(); ++y)
	{
		Gdiplus::Rect opaque(0, y, 0, 1); // 連続する不透明ピクセル列の領域
		bool transparentPrev = false;
		bool transparentCurr = false;
		byte* pixel = (byte*)locked.Scan0 + y * locked.Stride;

		for (unsigned int x = 0; x < image->GetWidth(); ++x)
		{
			// このピクセルは白かどうか
			transparentCurr = (pixel[0] != 0x00);
			//transparentCurr = image.GetPixel(x, y).A < 0x40;

			if (transparentCurr) {
				if (transparentPrev) {
					// 透明領域のまま
				}
				else {
					// このピクセルで透明領域に変わった
					if (0 < opaque.Width)
						// これまでの不透明ピクセル列の領域をリージョンに結合
						region->Union(opaque);
				}
			}
			else {
				if (transparentPrev) {
					// このピクセルで不透明領域に変わった
					// 不透明ピクセル列の開始点を初期化
					opaque.X = x;
					opaque.Width = 1;
				}
				else {
					// 不透明領域のまま
					// 不透明ピクセル列の幅を更新
					opaque.Width = x - opaque.X + 1;
				}
			}

			// 次のピクセルへ進む
			++pixel;

			// 直前のピクセルの透明/不透明の値を保存
			transparentPrev = transparentCurr;
		}

		// 最後のピクセルが不透明領域だった場合
		if (!transparentPrev && !transparentCurr)
			// これまでの不透明ピクセル列の領域をリージョンに結合
			region->Union(opaque);
	}
	image->UnlockBits(&locked);
	return region;
}