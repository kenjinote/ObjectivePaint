#pragma once

#include "myObject.h"

using namespace Gdiplus;

class string : public myObject
{
public:
	string();
	~string();
	string(const string& other);
	virtual myObject* Copy() const override;
	void setstring(LPCTSTR lpszNewString);
	LPTSTR GetString() const { return m_lpszText; }
	void ChangeSize(int nSize);
	Gdiplus::REAL GetSize() const { return size; }
	void ChangeFont(LPCTSTR lpszFontName);
	Gdiplus::Font* GetFont()const { return m_font; }
	void SetBold(BOOL bBold);
	BOOL GetBold() const;
	void SetItalic(BOOL bItalic);
	BOOL GetItalic() const;
	void CalcSize();
	void SetRotateHalfChar(BOOL bRotate)
	{
		if (this->m_bRotateHalfChar != bRotate)
		{
			m_bRotateHalfChar = bRotate;
			CalcSize();
		}
	}
	BOOL GetRotateHalfChar() const
	{
		return m_bRotateHalfChar;
	}
	void SetTategaki(BOOL bTategaki)
	{
		if (this->m_Tategaki != bTategaki)
		{
			m_Tategaki = bTategaki;
			CalcSize();
		}
	}
	void SetCharSpace(int nSpace)
	{
		if (this->m_nCharSpace != nSpace)
		{
			m_nCharSpace = nSpace;
			CalcSize();
		}
	}
	int GetCharSpace()const{ return m_nCharSpace; }
	void SetLineSpace(int nSpace)
	{
		if (this->m_nLineSpace != nSpace)
		{
			m_nLineSpace = nSpace;
			CalcSize();
		}
	}
	int GetLineSpace()const{ return m_nLineSpace; }
	BOOL GetTategaki() const { return m_Tategaki; }
	void SetColor(Gdiplus::Color color)
	{
		this->color = color;
	}
	Gdiplus::Color GetColor() const
	{
		return color;
	}
	virtual void SetAlpha(Gdiplus::REAL alpha) override
	{
		color.SetValue(Gdiplus::Color::MakeARGB((BYTE)(alpha / 100.0*255.0), color.GetR(), color.GetG(), color.GetB()));
	}
	virtual Gdiplus::REAL GetAlpha() const override
	{
		return color.GetAlpha() / 255.0f * 100.0f;
	}
	virtual void draw(Gdiplus::Graphics* g, bool isprinting = false) const override;
	int GetAlignment()const { return m_Alignment; }
	void SetAlignment(int Alignment) { m_Alignment = (Gdiplus::StringAlignment)Alignment; }
	virtual BOOL IsSelected()const override { return m_bSelected; }
	virtual VOID SetSelect(BOOL bSelect) override { m_bSelected = bSelect; }
	virtual BOOL IsString() const override { return TRUE; }
	virtual void save(HANDLE hFile) const override;
	virtual void open(HANDLE hFile) override;
	void calcsize(Graphics *g, LPCTSTR lpszText, int nTextLength, int charspace, int linespace, Font* font, BOOL bTate, float* width, float* height) const;
	void getpath(Graphics *g, float x, float y, float width, float height, LPCTSTR lpszText, int charspace, int linespace, Font* font, BOOL bTate, FontStyle style, GraphicsPath* path) const;
	void drawstring(Graphics *g, float x, float y, float width, float height, LPCTSTR lpszText, int charspace, int linespace, Font* font, Brush* brush, BOOL bTate) const;
	BOOL isNullString() const;

private:
	LPTSTR m_lpszText;
	Gdiplus::Font* m_font;
	Gdiplus::REAL size;
	Gdiplus::Color color;
	Gdiplus::FontStyle style;
	TCHAR m_szFontName[LF_FACESIZE];
	BOOL m_Tategaki;
	Gdiplus::StringAlignment m_Alignment;
	int m_nLineSpace;
	int m_nCharSpace;
	BOOL m_bRotateHalfChar;
};

