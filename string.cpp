#include <Windows.h>
#include <gdiplus.h>
#include "math.h"
#include "string.h"
#include "effectlist.h"
#include "effect.h"
#include "util.h"


string::string() : myObject(), m_lpszText(0), m_font(0), size(16), color(Gdiplus::Color::Black), m_Tategaki(0), style(Gdiplus::FontStyle::FontStyleRegular), m_Alignment(Gdiplus::StringAlignment::StringAlignmentNear), m_nLineSpace(0), m_nCharSpace(0), m_bRotateHalfChar(0)
{
	m_lpszText = new TCHAR[1];
	m_lpszText[0] = TEXT('\0');
	lstrcpy(m_szFontName, TEXT("ÇlÇr ÉSÉVÉbÉN"));
	m_font = new Gdiplus::Font(m_szFontName, size, style);
	CalcSize();
}

string::string(const string& other) : myObject((myObject&)other)
{
	if (other.m_lpszText)
	{
		const int nSize = lstrlen(other.m_lpszText);
		m_lpszText = new TCHAR[nSize + 1];
		lstrcpy(m_lpszText, other.m_lpszText);
	}
	else
	{
		m_lpszText = new TCHAR[1];
		m_lpszText[0] = TEXT('\0');
	}
	m_font = other.m_font->Clone();
	size = other.size;
	color = other.color;
	style = other.style;
	lstrcpy(m_szFontName, other.m_szFontName);
	m_Tategaki = other.m_Tategaki;
	m_Alignment = other.m_Alignment;
	m_nLineSpace = other.m_nLineSpace;
	m_nCharSpace = other.m_nCharSpace;
	m_bRotateHalfChar = other.m_bRotateHalfChar;
}

string::~string()
{
	delete[]m_lpszText;
	delete m_font;
}

myObject* string::Copy() const
{
	return (myObject*)new string(*this);
}

void string::draw(Gdiplus::Graphics* g, bool isprinting) const
{
	if (!this) return;
	Gdiplus::Matrix mx;
	g->GetTransform(&mx);
	g->TranslateTransform(x + width / 2.0f, y + height / 2.0f);
	g->RotateTransform(angle);
	if (height_scale > 0.05)
	{
		Gdiplus::Matrix mx2;
		g->GetTransform(&mx2);
		g->ScaleTransform(bLeftRight ? -width_scale : width_scale, bUpDown ? -height_scale : height_scale);
		LPCTSTR lpszText = isNullString() ? TEXT("ç∂ÇÃÉpÉlÉãÇ≈\r\nï∂èÕÇê›íËÇµÇƒÇ≠ÇæÇ≥Ç¢ÅB") : m_lpszText;
		if (!effectList || effectList->size() < 1)
		{
			drawstring(g, -width / 2.0f, -height / 2.0f, width, height, lpszText, m_nCharSpace, m_nLineSpace, m_font, &Gdiplus::SolidBrush(color), m_Tategaki);
		}
		else
		{
			if (effectList->IsIncludedBlur())
			{
				if (!pTempBitmapFront)
				{
					delete pTempBitmapFront;
					pTempBitmapFront = new Gdiplus::Bitmap((int)(width * 2.0f * 2.0f), (int)(height * 2.0f * 2.0f), PixelFormat32bppARGB);
					Gdiplus::Graphics g(pTempBitmapFront);
					g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
					g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
					g.Clear(Gdiplus::Color(0, 0, 0, 0));
					g.ScaleTransform(2.0f, 2.0f);
					{
						Gdiplus::GraphicsPath path;
						getpath(&g, width / 2.0f, height / 2.0f, width, height, lpszText, m_nCharSpace, m_nLineSpace, m_font, m_Tategaki, style, &path);
						effectList->draw(&g, &path, effect::Before, width, height);
						drawstring(&g, width / 2.0f, height / 2.0f, width, height, lpszText, m_nCharSpace, m_nLineSpace, m_font, &Gdiplus::SolidBrush(color), m_Tategaki);
						effectList->draw(&g, &path, effect::After, width, height);
						effectList->blur(pTempBitmapFront);
					}
				}
				g->DrawImage(pTempBitmapFront, -width, -height, width * 2, height * 2);
			}
			else
			{
				if (!pTempBitmapBack)
				{
					delete pTempBitmapBack;
					pTempBitmapBack = new Gdiplus::Bitmap((int)(width * 2.0f * 2.0f), (int)(height * 2.0f * 2.0f), PixelFormat32bppARGB);
					Gdiplus::Graphics g(pTempBitmapBack);
					g.Clear(Gdiplus::Color(0, 0, 0, 0));
					g.ScaleTransform(2.0f, 2.0f);
					{
						Gdiplus::GraphicsPath path;
						getpath(&g, width / 2.0f, height / 2.0f, width, height, lpszText, m_nCharSpace, m_nLineSpace, m_font, m_Tategaki, style, &path);
						effectList->draw(&g, &path, effect::Before, width, height);
					}
				}
				if (!pTempBitmapFront)
				{
					delete pTempBitmapFront;
					pTempBitmapFront = new Gdiplus::Bitmap((int)(width * 2.0f * 2.0f), (int)(height * 2.0f * 2.0f), PixelFormat32bppARGB);
					Gdiplus::Graphics g(pTempBitmapFront);
					g.Clear(Gdiplus::Color(0, 0, 0, 0));
					g.ScaleTransform(2.0f, 2.0f);
					{
						Gdiplus::GraphicsPath path;
						getpath(&g, width / 2.0f, height / 2.0f, width, height, lpszText, m_nCharSpace, m_nLineSpace, m_font, m_Tategaki, style, &path);
						effectList->draw(&g, &path, effect::After, width, height);
					}
				}
				g->DrawImage(pTempBitmapBack, -width, -height, width * 2, height * 2);
				drawstring(g, -width / 2.0f, -height / 2.0f, width, height, lpszText, m_nCharSpace, m_nLineSpace, m_font, &Gdiplus::SolidBrush(color), m_Tategaki);
				g->DrawImage(pTempBitmapFront, -width, -height, width * 2, height * 2);
			}
		}
		g->SetTransform(&mx2);
	}
	if (m_bSelected && !isprinting)
	{
		const Gdiplus::Color color(0, 153, 204); // ëIëòg
		const Gdiplus::Pen pen(color, 1.5f);
		g->DrawEllipse(&pen, (-width / 2.0f * 2.0f + width) / 2.0f - 4.0f, -height / 2.0f * height_scale - 12.0f, 8.0f, 8.0f);
		g->DrawLine(&pen, (-width / 2.0f * 2.0f + width) / 2.0f, -height / 2.0f * height_scale - 12.0f + 8.0f, (-width / 2.0f * 2.0f + width) / 2.0f, -height / 2.0f * height_scale);
		g->DrawRectangle(&pen, -width / 2.0f * width_scale, -height / 2.0f * height_scale, width * width_scale, height * height_scale);
	}
	g->SetTransform(&mx);
}

void string::CalcSize()
{
	const HDC hdc = GetDC(0);
	Gdiplus::Graphics g(hdc);
	const LPCTSTR lpszText = isNullString() ? TEXT("ç∂ÇÃÉpÉlÉãÇ≈\r\nï∂èÕÇê›íËÇµÇƒÇ≠ÇæÇ≥Ç¢ÅB") : m_lpszText;
	calcsize(&g, lpszText, -1, m_nCharSpace, m_nLineSpace, m_font, m_Tategaki, &width, &height);
	ReleaseDC(0, hdc);
}

void string::setstring(LPCTSTR lpszNewString)
{
	if (!this) return;
	delete[]m_lpszText;
	if (lpszNewString)
	{
		const int nSize = lstrlen(lpszNewString);
		m_lpszText = new TCHAR[nSize + 1];
		lstrcpy(m_lpszText, lpszNewString);
	}
	else
	{
		m_lpszText = new TCHAR[1];
		m_lpszText[0] = TEXT('\0');
	}
	CalcSize();
}

void string::ChangeSize(int nSize)
{
	if (!this) return;
	if (nSize > 0)
	{
		size = (Gdiplus::REAL)nSize;
		delete m_font;
		m_font = new Gdiplus::Font(m_szFontName, size, style);
		CalcSize();
	}
}

void string::ChangeFont(LPCTSTR lpszFontName)
{
	if (!this) return;
	if (lstrlen(lpszFontName))
	{
		lstrcpy(m_szFontName, lpszFontName);
		delete m_font;
		m_font = new Gdiplus::Font(m_szFontName, size, style);
		CalcSize();
	}
}

void string::SetBold(BOOL bBold)
{
	if (!this) return;
	delete m_font;
	if (bBold)
	{
		style = (Gdiplus::FontStyle)(style | Gdiplus::FontStyle::FontStyleBold);
	}
	else
	{
		style = (Gdiplus::FontStyle)(style & (~Gdiplus::FontStyle::FontStyleBold));
	}
	m_font = new Gdiplus::Font(m_szFontName, size, style);
	CalcSize();
}

BOOL string::GetBold() const
{
	return (m_font->GetStyle() & Gdiplus::FontStyle::FontStyleBold);
}

void string::SetItalic(BOOL bItalic)
{
	if (!this) return;
	delete m_font;
	if (bItalic)
	{
		style = (Gdiplus::FontStyle)(style | Gdiplus::FontStyle::FontStyleItalic);
	}
	else
	{
		style = (Gdiplus::FontStyle)(style & (~Gdiplus::FontStyle::FontStyleItalic));
	}
	m_font = new Gdiplus::Font(m_szFontName, size, style);
	CalcSize();
}

BOOL string::GetItalic() const
{
	return (m_font->GetStyle() & Gdiplus::FontStyle::FontStyleItalic);
}

void string::save(HANDLE hFile) const
{
	DWORD dwWriteSize;
	TCHAR ObjectType = TEXT('S');
	WriteFile(hFile, &ObjectType, sizeof(TCHAR), &dwWriteSize, 0);
	myObject::save(hFile);
	UINT nSize = lstrlen(m_lpszText) + 1;
	WriteFile(hFile, &nSize, sizeof(UINT), &dwWriteSize, 0);
	WriteFile(hFile, &m_lpszText[0], (sizeof(TCHAR)*nSize), &dwWriteSize, 0);
	WriteFile(hFile, &size, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	DWORD argb = color.GetValue();
	WriteFile(hFile, &argb, sizeof(DWORD), &dwWriteSize, 0);
	WriteFile(hFile, &style, sizeof(INT), &dwWriteSize, 0);
	nSize = lstrlen(m_szFontName) + 1;
	WriteFile(hFile, &nSize, sizeof(UINT), &dwWriteSize, 0);
	WriteFile(hFile, &m_szFontName, (sizeof(TCHAR)*nSize), &dwWriteSize, 0);
	WriteFile(hFile, &m_Tategaki, sizeof(BOOL), &dwWriteSize, 0);
	WriteFile(hFile, &m_Alignment, sizeof(INT), &dwWriteSize, 0);
	WriteFile(hFile, &m_nLineSpace, sizeof(int), &dwWriteSize, 0);
	WriteFile(hFile, &m_nCharSpace, sizeof(int), &dwWriteSize, 0);
	WriteFile(hFile, &m_bRotateHalfChar, sizeof(BOOL), &dwWriteSize, 0);
}

void string::open(HANDLE hFile)
{
	DWORD dwReadSize;
	myObject::open(hFile);
	UINT nSize;
	ReadFile(hFile, &nSize, sizeof(UINT), &dwReadSize, 0);// lstrlen(m_lpszText)
	if (m_lpszText)
	{
		delete[]m_lpszText;
	}
	m_lpszText = new TCHAR[nSize];
	ReadFile(hFile, &m_lpszText[0], (sizeof(TCHAR)*nSize), &dwReadSize, 0);//  m_lpszText;
	ReadFile(hFile, &size, sizeof(Gdiplus::REAL), &dwReadSize, 0);//  size;
	DWORD argb;
	ReadFile(hFile, &argb, sizeof(DWORD), &dwReadSize, 0);//  color;(ARGB)
	color.SetValue(argb);
	ReadFile(hFile, &style, sizeof(INT), &dwReadSize, 0);//  style;
	ReadFile(hFile, &nSize, sizeof(UINT), &dwReadSize, 0);// lstrlen(szFontName)
	ReadFile(hFile, &m_szFontName, (sizeof(TCHAR)*nSize), &dwReadSize, 0);//  szFontName;
	delete m_font;
	m_font = new Gdiplus::Font(m_szFontName, size, style);
	ReadFile(hFile, &m_Tategaki, sizeof(BOOL), &dwReadSize, 0);//  m_Tategaki;
	ReadFile(hFile, &m_Alignment, sizeof(INT), &dwReadSize, 0);//  m_Alignment;
	ReadFile(hFile, &m_nLineSpace, sizeof(int), &dwReadSize, 0);//  m_nLineSpace;
	ReadFile(hFile, &m_nCharSpace, sizeof(int), &dwReadSize, 0);//  m_nCharSpace;
	ReadFile(hFile, &m_bRotateHalfChar, sizeof(BOOL), &dwReadSize, 0);//  m_bRotateHalfChar;
	CalcSize();
}

void string::calcsize(Graphics *g, LPCTSTR lpszText, int nTextLength, int charspace, int linespace, Font* font, BOOL bTate, float* width, float* height) const
{
	const int size = (nTextLength >= 0) ? nTextLength : lstrlen(lpszText);
	Gdiplus::RectF rectchar;
	const StringFormat* pStringFormat = StringFormat::GenericTypographic();
	StringFormat MeasureFormat(pStringFormat);
	float x = 0;
	float y = 0;
	*width = 0;
	*height = 0;
	RectF rect;
	float maxwidth = 0;
	float maxheight = 0;
	LPTSTR pLineHead = (LPTSTR)lpszText;
	LPTSTR pLineEnd = wcschr(pLineHead, TEXT('\r'));
	if (pLineEnd == NULL)
	{
		pLineEnd = pLineHead + lstrlen(pLineHead);
	}
	bool linehead = true;
	if (bTate)
	{
		for (int i = 0; i < size; ++i)
		{
			if (lpszText[i] == TEXT('\r')) { x -= (maxwidth + linespace); continue; }
			if (lpszText[i] == TEXT('\n')) { pLineHead = (LPTSTR)&(lpszText[i + 1]); linehead = true; continue; }
			if (linehead)
			{
				linehead = false;
				y = 0;
				maxwidth = 0;
			}
			CharacterRange range(0, 1);
			MeasureFormat.SetMeasurableCharacterRanges(1, &range);
			Gdiplus::Region reg;
			TCHAR c;
			switch (lpszText[i])
			{
			case TEXT(' '):
				c = TEXT('A');
				break;
			case TEXT('Å@'):
				c = TEXT('Ç†');
				break;
			default:
				c = lpszText[i];
				break;
			}
			g->MeasureCharacterRanges(&c, 1, font, RectF(0, 0, REAL_MAX, REAL_MAX), &MeasureFormat, 1, &reg);
			reg.GetBounds(&rectchar, g);
			WORD pType[1];
			GetStringTypeEx(LOCALE_SYSTEM_DEFAULT, CT_CTYPE3, &(lpszText[i]), 1, pType);
			if (m_bRotateHalfChar) // îºäpï∂éöÇ™Ç†ÇÍÇŒÇªÇÃï∂éöÇå©ÇΩñ⁄Ç≈ècÅií èÌÇ…ï`âÊÅjÇ…Ç∑ÇÈ
			{
				if ((pType[0] & C3_HALFWIDTH))
				{
					float temp = rectchar.Width;
					rectchar.Width = rectchar.Height;
					rectchar.Height = temp;
				}
			}
			if (rectchar.Height > maxwidth)
			{
				maxwidth = rectchar.Height;
			}
			if (rectchar.Width > maxwidth)
			{
				maxwidth = rectchar.Width;
			}
			y += (rectchar.Width + charspace);
			if (y > maxheight)
			{
				maxheight = y;
			}
		}
		if (y > maxheight)
		{
			maxheight = y;
		}

		*width = -(x - maxwidth); // å©ÇΩñ⁄ÇÃâ°
		*height = maxheight; // å©ÇΩñ⁄ÇÃçÇÇ≥
	}
	else
	{
		for (int i = 0; i < size; ++i)
		{
			if (lpszText[i] == TEXT('\r')) continue;
			if (lpszText[i] == TEXT('\n'))
			{
				if (x > maxwidth)
				{
					maxwidth = x;
				}
				x = 0;
				y += rectchar.Height + linespace;
				continue;
			}
			CharacterRange range(0, 1);
			MeasureFormat.SetMeasurableCharacterRanges(1, &range);
			Gdiplus::Region reg;
			TCHAR c;
			switch (lpszText[i])
			{
			case TEXT(' '):
				c = TEXT('A');
				break;
			case TEXT('Å@'):
				c = TEXT('Ç†');
				break;
			default:
				c = lpszText[i];
				break;
			}
			g->MeasureCharacterRanges(&c, 1, font, RectF(0, 0, REAL_MAX, REAL_MAX), &MeasureFormat, 1, &reg);
			reg.GetBounds(&rectchar, g);
			x += rectchar.Width + charspace;
		}
		if (x> maxwidth)
		{
			maxwidth = x;
		}
		*width = maxwidth;
		*height = y + rectchar.Height;
	}
}

void string::getpath(Graphics *g, float x, float y, float width, float height, LPCTSTR lpszText, int charspace, int linespace, Font* font, BOOL bTate, FontStyle style, GraphicsPath* path) const
{
	const int size = lstrlen(lpszText);
	Gdiplus::RectF rectchar;
	const StringFormat* pStringFormat = StringFormat::GenericTypographic();
	StringFormat MeasureFormat(pStringFormat);
	Gdiplus::FontFamily fontfamily;
	font->GetFamily(&fontfamily);
	LPTSTR pLineHead = (LPTSTR)lpszText;
	LPTSTR pLineEnd = wcschr(pLineHead, TEXT('\r'));
	if (pLineEnd == NULL)
	{
		pLineEnd = pLineHead + lstrlen(pLineHead);
	}
	bool linehead = true;
	if (bTate)
	{
		float lx = x + width;
		float ly = y;
		float maxwidth = 0;
		for (int i = 0; i < size; ++i)
		{
			if (lpszText[i] == TEXT('\r')) { lx -= (maxwidth + linespace); continue; }
			if (lpszText[i] == TEXT('\n')) { pLineHead = (LPTSTR)&(lpszText[i + 1]); linehead = true; continue; }
			if (linehead)
			{
				linehead = false;
				if (m_Alignment == Gdiplus::StringAlignmentNear)
				{
					ly = y;
				}
				else if (m_Alignment == Gdiplus::StringAlignmentCenter)
				{
					float lwidth, lheight;
					pLineEnd = wcschr(pLineHead, TEXT('\r'));
					if (pLineEnd == NULL)
					{
						pLineEnd = pLineHead + lstrlen(pLineHead);
					}
					calcsize(g, pLineHead, (int)(pLineEnd - pLineHead), charspace, linespace, font, bTate, &lwidth, &lheight);
					ly = y + (height - lheight) / 2.0f;
				}
				else if (m_Alignment == Gdiplus::StringAlignmentFar)
				{
					float lwidth, lheight;
					pLineEnd = wcschr(pLineHead, TEXT('\r'));
					if (pLineEnd == NULL)
					{
						pLineEnd = pLineHead + lstrlen(pLineHead);
					}
					calcsize(g, pLineHead, (int)(pLineEnd - pLineHead), charspace, linespace, font, bTate, &lwidth, &lheight);
					ly = y + height - lheight;
				}
			}
			CharacterRange range(0, 1);
			MeasureFormat.SetMeasurableCharacterRanges(1, &range);
			Gdiplus::Region reg;
			g->MeasureCharacterRanges(&(lpszText[i]), -1, font, RectF(0, 0, REAL_MAX, REAL_MAX), &MeasureFormat, 1, &reg);
			reg.GetBounds(&rectchar, g);
			StringFormat Format2(pStringFormat);
			Format2.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
			WORD pType[1];
			GetStringTypeEx(LOCALE_SYSTEM_DEFAULT, CT_CTYPE3, &(lpszText[i]), 1, pType);
			if (m_bRotateHalfChar)
			{
				if ((pType[0] & C3_HALFWIDTH))
				{
					path->AddString(&(lpszText[i]), 1, &fontfamily, style, (96.0f * this->size) / 72.0f, PointF(lx - rectchar.Height / 2.0f, ly), &Format2);
					float temp = rectchar.Width;
					rectchar.Width = rectchar.Height;
					rectchar.Height = temp;
				}
				else
				{
					Format2.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsDirectionVertical | Gdiplus::StringFormatFlags::StringFormatFlagsDirectionRightToLeft);
					path->AddString(&(lpszText[i]), 1, &fontfamily, style, (96.0f * this->size) / 72.0f, PointF(lx, ly + rectchar.Width / 2.0f), &Format2);
				}
			}
			else
			{
				Format2.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsDirectionVertical | Gdiplus::StringFormatFlags::StringFormatFlagsDirectionRightToLeft);
				path->AddString(&(lpszText[i]), 1, &fontfamily, style, (96.0f * this->size) / 72.0f, PointF(lx, ly + rectchar.Width / 2.0f), &Format2);
			}
			if (rectchar.Height > maxwidth)
			{
				maxwidth = rectchar.Height;
			}
			ly += (rectchar.Width + charspace);
		}
	}
	else
	{
		float lx = x;
		float ly = y;
		for (int i = 0; i < size; ++i)
		{
			if (lpszText[i] == TEXT('\r')) { ly += rectchar.Height + linespace; continue; }
			if (lpszText[i] == TEXT('\n')) { pLineHead = (LPTSTR)&(lpszText[i + 1]); linehead = true; continue; }
			if (linehead)
			{
				linehead = false;
				if (m_Alignment == Gdiplus::StringAlignmentNear)
				{
					lx = x;
				}
				else if (m_Alignment == Gdiplus::StringAlignmentCenter)
				{
					float lwidth, lheight;
					pLineEnd = wcschr(pLineHead, TEXT('\r'));
					if (pLineEnd == NULL)
					{
						pLineEnd = pLineHead + lstrlen(pLineHead);
					}
					calcsize(g, pLineHead, (int)(pLineEnd - pLineHead), charspace, linespace, font, bTate, &lwidth, &lheight);
					lx = x + (width - lwidth) / 2.0f;
				}
				else if (m_Alignment == Gdiplus::StringAlignmentFar)
				{
					float lwidth, lheight;
					pLineEnd = wcschr(pLineHead, TEXT('\r'));
					if (pLineEnd == NULL)
					{
						pLineEnd = pLineHead + lstrlen(pLineHead);
					}
					calcsize(g, pLineHead, (int)(pLineEnd - pLineHead), charspace, linespace, font, bTate, &lwidth, &lheight);
					lx = x + width - lwidth;
				}
			}
			CharacterRange range(0, 1);
			MeasureFormat.SetMeasurableCharacterRanges(1, &range);
			Gdiplus::Region reg;
			g->MeasureCharacterRanges(&(lpszText[i]), -1, font, RectF(0, 0, REAL_MAX, REAL_MAX), &MeasureFormat, 1, &reg);
			path->AddString(&(lpszText[i]), 1, &fontfamily, style, (96.0f * this->size) / 72.0f, PointF(lx, ly), &MeasureFormat);
			reg.GetBounds(&rectchar, g);
			lx += rectchar.Width + charspace;
#ifdef _DEBUG
			{
				TCHAR szText[256];
				swprintf_s(szText, TEXT("path %f\r\n"), rectchar.Width);
				OutputDebugString(szText);
			}
#endif
		}
	}
}

void string::drawstring(Graphics *g, float x, float y, float width, float height, LPCTSTR lpszText, int charspace, int linespace, Font* font, Brush* brush, BOOL bTate) const
{
	const int size = lstrlen(lpszText);
	Gdiplus::RectF rectchar;
	const StringFormat* pStringFormat = StringFormat::GenericTypographic();
	StringFormat MeasureFormat(pStringFormat);
	LPTSTR pLineHead = (LPTSTR)lpszText;
	LPTSTR pLineEnd = wcschr(pLineHead, TEXT('\r'));
	if (pLineEnd == NULL)
	{
		pLineEnd = pLineHead + lstrlen(pLineHead);
	}
	bool linehead = true;
	if (bTate)
	{
		float lx = x + width;
		float ly = y;
		float maxwidth = 0;
		for (int i = 0; i < size; ++i)
		{
			if (lpszText[i] == TEXT('\r')) { lx -= (maxwidth + linespace); continue; }
			if (lpszText[i] == TEXT('\n')) { pLineHead = (LPTSTR)&(lpszText[i + 1]); linehead = true; continue; }
			if (linehead)
			{
				linehead = false;
				if (m_Alignment == Gdiplus::StringAlignmentNear)
				{
					ly = y;
				}
				else if (m_Alignment == Gdiplus::StringAlignmentCenter)
				{
					float lwidth, lheight;
					pLineEnd = wcschr(pLineHead, TEXT('\r'));
					if (pLineEnd == NULL)
					{
						pLineEnd = pLineHead + lstrlen(pLineHead);
					}
					calcsize(g, pLineHead, (int)(pLineEnd - pLineHead), charspace, linespace, font, bTate, &lwidth, &lheight);
					ly = y + (height - lheight) / 2.0f;
				}
				else if (m_Alignment == Gdiplus::StringAlignmentFar)
				{
					float lwidth, lheight;
					pLineEnd = wcschr(pLineHead, TEXT('\r'));
					if (pLineEnd == NULL)
					{
						pLineEnd = pLineHead + lstrlen(pLineHead);
					}
					calcsize(g, pLineHead, (int)(pLineEnd - pLineHead), charspace, linespace, font, bTate, &lwidth, &lheight);
					ly = y + height - lheight;
				}
			}
			CharacterRange range(0, 1);
			MeasureFormat.SetMeasurableCharacterRanges(1, &range);
			Gdiplus::Region reg;
			g->MeasureCharacterRanges(&(lpszText[i]), -1, font, RectF(0, 0, REAL_MAX, REAL_MAX), &MeasureFormat, 1, &reg);
			reg.GetBounds(&rectchar, g);
			StringFormat Format2(pStringFormat);
			Format2.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
			WORD pType[1];
			GetStringTypeEx(LOCALE_SYSTEM_DEFAULT, CT_CTYPE3, &(lpszText[i]), 1, pType);
			if (m_bRotateHalfChar) // îºäpï∂éöÇ™Ç†ÇÍÇŒÇªÇÃï∂éöÇå©ÇΩñ⁄Ç≈ècÅií èÌÇ…ï`âÊÅjÇ…Ç∑ÇÈ
			{
				if ((pType[0] & C3_HALFWIDTH))
				{
					g->DrawString(&(lpszText[i]), 1, font, PointF(lx - rectchar.Height / 2.0f, ly), &Format2, brush);
					float temp = rectchar.Width;
					rectchar.Width = rectchar.Height;
					rectchar.Height = temp;
				}
				else
				{
					Format2.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsDirectionVertical | Gdiplus::StringFormatFlags::StringFormatFlagsDirectionRightToLeft);
					g->DrawString(&(lpszText[i]), 1, font, PointF(lx, ly + rectchar.Width / 2.0f), &Format2, brush);
				}
			}
			else
			{
				Format2.SetFormatFlags(Gdiplus::StringFormatFlags::StringFormatFlagsDirectionVertical | Gdiplus::StringFormatFlags::StringFormatFlagsDirectionRightToLeft);
				g->DrawString(&(lpszText[i]), 1, font, PointF(lx, ly + rectchar.Width / 2.0f), &Format2, brush);
			}
			if (rectchar.Height > maxwidth)
			{
				maxwidth = rectchar.Height; // å©ÇΩñ⁄ÇÃâ°
			}
			ly += (rectchar.Width + charspace); // å©ÇΩñ⁄ÇÃèc
		}
	}
	else
	{
		float lx = x;
		float ly = y;
		for (int i = 0; i < size; ++i)
		{
			if (lpszText[i] == TEXT('\r')) { ly += rectchar.Height + linespace; continue; }
			if (lpszText[i] == TEXT('\n')) { pLineHead = (LPTSTR)&(lpszText[i + 1]); linehead = true; continue; }
			if (linehead)
			{
				linehead = false;
				if (m_Alignment == Gdiplus::StringAlignmentNear)
				{
					lx = x;
				}
				else if (m_Alignment == Gdiplus::StringAlignmentCenter)
				{
					float lwidth, lheight;
					pLineEnd = wcschr(pLineHead, TEXT('\r'));
					if (pLineEnd == NULL)
					{
						pLineEnd = pLineHead + lstrlen(pLineHead);
					}
					calcsize(g, pLineHead, (int)(pLineEnd - pLineHead), charspace, linespace, font, bTate, &lwidth, &lheight);
					lx = x + (width - lwidth) / 2.0f;
				}
				else if (m_Alignment == Gdiplus::StringAlignmentFar)
				{
					float lwidth, lheight;
					pLineEnd = wcschr(pLineHead, TEXT('\r'));
					if (pLineEnd == NULL)
					{
						pLineEnd = pLineHead + lstrlen(pLineHead);
					}
					calcsize(g, pLineHead, (int)(pLineEnd - pLineHead), charspace, linespace, font, bTate, &lwidth, &lheight);
					lx = x + width - lwidth;
				}
			}
			CharacterRange range(0, 1);
			MeasureFormat.SetMeasurableCharacterRanges(1, &range);
			Gdiplus::Region reg;
			g->MeasureCharacterRanges(&(lpszText[i]), -1, font, RectF(0, 0, REAL_MAX, REAL_MAX), &MeasureFormat, 1, &reg);
			g->DrawString(&(lpszText[i]), 1, font, PointF(lx, ly), &MeasureFormat, brush);
			reg.GetBounds(&rectchar, g);
			lx += rectchar.Width + charspace;
#ifdef _DEBUG
			{
				TCHAR szText[256];
				swprintf_s(szText, TEXT("draw %f\r\n"), rectchar.Width);
				OutputDebugString(szText);
			}
#endif
		}
	}
}

BOOL string::isNullString() const
{
	const int n = lstrlen(m_lpszText);
	if (n == 0) return TRUE;
	for (int i = 0; i < n; ++i)
	{
		if (m_lpszText[i] != TEXT('\r') && m_lpszText[i] != TEXT('\n'))
			return FALSE;
	}
	return TRUE;
}