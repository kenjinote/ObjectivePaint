#pragma once

#define WM_COLOR WM_APP // WPARAM にCOLORREF、LPARAMは0
#define WM_CHANGE_MODE (WM_APP+1) // WPARAM LPARAM ともに0
#define WM_CALCPARAMETER (WM_APP+2)
#define WM_INVALIDATEITEM (WM_APP+3)
#define WM_ENSUREVISIBLE (WM_APP+4)
#define WM_TEMPLATE_OPEN (WM_APP+5)
#define WM_PICTURELIST_CLICK (WM_APP+6)
#define WM_OPEN_DIRECTORY (WM_APP+7)
#define WM_SET_OBJECT_VALUE_FOR_PALLET (WM_APP+8)
#define WM_SET_MODIFYFLAG (WM_APP+9)
#define WM_OPEN_TEMPLATE (WM_APP+10)
#define WM_CREATED (WM_APP+11)
#define WM_ADD_FOLDER (WM_APP+12) //lparamがフォルダ
#define WM_OPEN_FILE (WM_APP+13)

#define WINDOW_WIDTH 1092
#define WINDOW_HEIGHT 680

#define PI 3.14159265358979323846264338327950288419716939f
#define MASK_SCALE 0.5

enum MODE
{
	MODE_DESIGN,
	MODE_STRING,
	MODE_PICTURE
};

#define myround(x) (int)((x)+0.5)

class util
{
	static void converter(LPCTSTR lpszInputFilePath);
	static void convertfiles(TCHAR *pszBasePath, void(*func)(LPCTSTR));
	static inline void util::gaussBlur_4(BYTE* scl, BYTE* tcl, int w, int h, float rx, float ry, int bpp, int stride);
	static inline void util::boxBlur_4(BYTE* scl, BYTE* tcl, int w, int h, int rx, int ry, int bpp, int stride);
	static inline void util::boxBlurT_4(BYTE* scl, BYTE* tcl, int w, int h, int r, int bpp, int stride);
	static inline void util::boxBlurH_4(BYTE* scl, BYTE* tcl, int w, int h, int r, int bpp, int stride);
	static inline void util::boxesForGauss(double sigma, int *sizes, int n);


public:
	static Gdiplus::Bitmap* loadpng(int nID);
	static BOOL IsOnLine(const POINT* Begin, const POINT* End, const POINT* Point);
	static BOOL IsEqual(double a, double b);
	static HICON CreateArrowCursor(float fAngle);
	static VOID DrawArrow(HDC hdc, float angle, int size);
	static void GetHolesMask(const Gdiplus::Bitmap*pBitmap, int *pnHoles, Gdiplus::Bitmap**pBitmapOut);
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	static void SaveImageByteData(HANDLE hFile, const HGLOBAL hGlobal);
	static void OpenImageByteData(HANDLE hFile, HGLOBAL* lphGlobal);
	static Gdiplus::Bitmap* GetImageFromByteData(HGLOBAL hGlobal);
	static void SaveImage(HANDLE hFile, Gdiplus::Bitmap* pBitmap);
	static void OpenImage(HANDLE hFile, Gdiplus::Bitmap** ppBitmap);
	static void png2pcp();
	static void GetIniFilePath(LPTSTR lpszIniFilePath);
	static void GetProductName(LPTSTR lpszProductName, DWORD dwSize);
	static void GetProductGuid(LPTSTR lpszGuid, DWORD dwSize);
	static void DoGaussianBlur(Gdiplus::Bitmap* img, float radiusX, float radiusY);
	static void DoGaussianBlurPower(Gdiplus::Bitmap* img, float radiusX, float radiusY, int nPower);
	static Gdiplus::Region* util::CreateRegionFromImage8bpp(Gdiplus::Bitmap* image);
	static Gdiplus::Region* util::CreateRegionFromImage(Gdiplus::Bitmap* image);
};

