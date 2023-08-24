#pragma once
#include "effect.h"

class effectemission : public effect
{
	BYTE blur; // ‚Ú‚©‚µ
	BYTE power; // ‹­“x
	Gdiplus::Color color;
	BYTE inner; //“à‘¤

public:
	effectemission();
	~effectemission();

	virtual BOOL isemission() const override { return TRUE; }
	virtual effect* copy() const override;

	int getblur()const { return blur; }
	void setblur(int n){ blur = n; }

	int getpower()const { return power; }
	void setpower(int n){ power = n; }

	Gdiplus::Color getcolor()const { return color; }
	void setcolor(Gdiplus::Color n){ color = n; }

	BOOL getinner()const { return inner; }
	void setinner(BOOL n){ inner = n; }

	virtual void getliststring(LPTSTR, int) const override;

	virtual void draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, float width, float height) const override;

	virtual void save(HANDLE hFile) const override;
	virtual void open(HANDLE hFile) override;

	virtual KIND drawkind() const override { return inner ? After : Before; }; // •`‰æ
};

