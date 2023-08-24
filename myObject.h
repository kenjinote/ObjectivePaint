#pragma once

#include <GdiPlus.h>

class effect;
class effectlist;
class myObjectList;

class myObject
{
	UINT m_uBirth;
	UINT m_uDeath;

protected:
	Gdiplus::REAL x;
	Gdiplus::REAL y;
	Gdiplus::REAL angle;
	Gdiplus::REAL width;// ŽÀ¡
	Gdiplus::REAL height;
	Gdiplus::REAL width_scale;//scale
	Gdiplus::REAL height_scale;
	BOOL bLeftRight;
	BOOL bUpDown;
	BOOL m_bSelected;
	BOOL m_bMoving;
	effectlist* effectList;

	mutable Gdiplus::Bitmap* pTempBitmapBack; // ‰e‚È‚Ç”wŒi‚Ì‰æ‘œ
	mutable Gdiplus::Bitmap* pTempBitmapFront; // ƒxƒxƒ‹‚È‚Ç‘OŒi‚Ì‰æ‘œ

public:
	myObject();
	virtual ~myObject();
	myObject(const myObject& other);
	virtual myObject* Copy() const = 0;

	virtual BOOL IsAlive(const myObjectList* pList) const;
	virtual void newlife(myObjectList* pList);
	virtual void Kill(myObjectList* pList);
	virtual void Resurrect(myObjectList* pList);
	virtual void Bury(myObjectList* pList);
	virtual void Replace(myObjectList* pList, myObject* pNew);

	virtual void draw(Gdiplus::Graphics* g, bool isprinting = false) const {};
	virtual BOOL HitTest(Gdiplus::REAL, Gdiplus::REAL) const;
	virtual BOOL HitTestRotate(Gdiplus::REAL, Gdiplus::REAL) const;
	virtual INT HitTestResize(Gdiplus::REAL, Gdiplus::REAL) const;
	virtual HRGN GetDisplayRegion()const;
	virtual BOOL IsSelected()const = 0;
	virtual VOID SetSelect(BOOL) = 0;
	virtual BOOL IsPicture() const { return FALSE; }
	virtual BOOL IsString() const { return FALSE; }
	virtual BOOL IsMoving() const { return m_bMoving; }
	virtual VOID SetMoving(BOOL bMoving) { m_bMoving = bMoving; }
	virtual void SetAlpha(Gdiplus::REAL alpha) = 0;
	virtual Gdiplus::REAL GetAlpha() const = 0;
	virtual void setx(Gdiplus::REAL x)
	{
		this->x = x;
	}
	virtual void sety(Gdiplus::REAL y)
	{
		this->y = y;
	}
	virtual Gdiplus::REAL GetXPos()const { return x; };
	virtual Gdiplus::REAL GetYPos()const { return y; };
	virtual void SetWidthScale(Gdiplus::REAL width_scale)
	{
		this->width_scale = width_scale;
	}
	virtual Gdiplus::REAL GetWidthScale() const
	{
		return width_scale;
	}
	virtual void SetHeightScale(Gdiplus::REAL height_scale)
	{
		this->height_scale = height_scale;
	}
	virtual Gdiplus::REAL GetHeightScale() const
	{
		return height_scale;
	}
	virtual void SetAngle(Gdiplus::REAL angle)
	{
		this->angle = (Gdiplus::REAL)angle;
	}
	virtual Gdiplus::REAL GetAngle() const
	{
		return angle;
	}
	virtual void SetLeftRight(BOOL bLeftRight)
	{
		this->bLeftRight = bLeftRight;
	}
	virtual BOOL GetLeftRight() const
	{
		return bLeftRight;
	}
	virtual void SetUpDown(BOOL bUpDown)
	{
		this->bUpDown = bUpDown;
	}
	virtual BOOL GetUpDown() const
	{
		return bUpDown;
	}
	virtual Gdiplus::REAL GetWidth() const { return width; }
	virtual Gdiplus::REAL GetHeight() const { return height; }
	virtual void save(HANDLE hFile) const;
	virtual void open(HANDLE hFile);
	effect* geteffect(int i)const;
	size_t geteffectsize()const;
	void deleteeffect(int i);
	void addeffect(effect* e);
	void inserteffect(int n, effect* e);
	void replaceeffect(int n, effect* e);
	void deleteTemp()
	{
		delete pTempBitmapBack;
		pTempBitmapBack = NULL;
		delete pTempBitmapFront;
		pTempBitmapFront = NULL;
	}
};