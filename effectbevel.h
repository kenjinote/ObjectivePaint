#pragma once
#include "effect.h"

class effectbevel : public effect
{
	BYTE blur;
	BYTE power;
	short angle;
	BYTE width;
	BYTE alpha;
	Gdiplus::Color color1;
	Gdiplus::Color color2;

public:
	effectbevel();
	~effectbevel();

	virtual BOOL isbevel() const override { return TRUE; }
	virtual effect* copy() const override;

	int getblur()const { return blur; }
	void setblur(int n){ blur = n; }

	int getpower()const { return power; }
	void setpower(int n){ power = n; }

	int getangle()const { return angle; }
	void setangle(int n){ angle = n; }

	int getwidth()const { return width; }
	void setwidth(int n){ width = n; }

	int getalpha()const { return alpha; }
	void setalpha(int n){ alpha = n; }

	Gdiplus::Color getcolor1()const { return color1; }
	void setcolor1(Gdiplus::Color n){ color1 = n; }

	Gdiplus::Color getcolor2()const { return color2; }
	void setcolor2(Gdiplus::Color n){ color2 = n; }

	virtual void getliststring(LPTSTR, int) const override;

	virtual void draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, float width, float height) const override;

	virtual void save(HANDLE hFile) const override;
	virtual void open(HANDLE hFile) override;

	virtual KIND drawkind() const override{ return After; }; // •`‰æ
};

