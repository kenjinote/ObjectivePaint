#pragma once
#include "effect.h"

class effectblur : public effect
{
	BYTE horizontal; // êÖïΩ
	BYTE vertical; // êÇíº

public:
	effectblur();
	~effectblur();

	virtual BOOL isblur() const override { return TRUE; }
	virtual effect* copy() const override;

	int gethorizontal()const { return horizontal; }
	void sethorizontal(int n){ horizontal = n; }

	int getvertical()const { return vertical; }
	void setvertical(int n){ vertical = n; }

	virtual void getliststring(LPTSTR, int) const override;

	virtual void draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, float width, float height) const override;

	virtual void save(HANDLE hFile) const override;
	virtual void open(HANDLE hFile) override;

	virtual KIND drawkind() const override { return None; }; // ï`âÊ
	virtual void blur(Gdiplus::Bitmap*pBitmap) const override;
};
