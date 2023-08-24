#define _USE_MATH_DEFINES

#include <Windows.h>
#include <gdiplus.h>
#include <math.h>
#include <vector>
#include "util.h"
#include "effectlist.h"
#include "myObject.h"
#include "myObjectList.h"

myObject::myObject() : m_uBirth(0), m_uDeath(UINT_MAX), x(0), y(0), width(0), width_scale(1.0), height(0), height_scale(1.0), angle(0.0f), bLeftRight(FALSE), bUpDown(FALSE), m_bSelected(FALSE), m_bMoving(FALSE), pTempBitmapBack(0), pTempBitmapFront(0)
{
	effectList = new effectlist;
}

myObject::myObject(const myObject& other)
{
	m_uBirth = other.m_uBirth;
	m_uDeath = other.m_uDeath;
	x = other.x;
	y = other.y;
	angle = other.angle;
	width = other.width;
	width_scale = other.width_scale;
	height = other.height;
	height_scale = other.height_scale;
	bLeftRight = other.bLeftRight;
	bUpDown = other.bUpDown;
	m_bSelected = other.m_bSelected;
	m_bMoving = FALSE;
	effectList = other.effectList->copy();
	pTempBitmapBack = nullptr;
	pTempBitmapFront = nullptr;
}

myObject::~myObject()
{
	delete effectList;
	delete pTempBitmapBack;
	delete pTempBitmapFront;
}

void myObject::newlife(myObjectList* pList)
{
	m_uBirth = pList->GetCurrGen();
	m_uDeath = UINT_MAX;
}

BOOL myObject::IsAlive(const myObjectList* pList) const
{
	const UINT uViewGen = pList->GetViewGen();
	return ((m_uBirth <= uViewGen) && ((m_uDeath == UINT_MAX) || (uViewGen <= m_uDeath))) ? TRUE : FALSE;
}

void myObject::Kill(myObjectList* pList)
{
	m_bSelected = FALSE;
	delete pTempBitmapBack; // ‰e‚È‚Ç”wŒi‚Ì‰æ‘œ
	pTempBitmapBack = NULL;
	delete pTempBitmapFront; // ƒxƒxƒ‹‚È‚Ç‘OŒi‚Ì‰æ‘œ
	pTempBitmapFront = NULL;
	m_uDeath = pList->GetCurrGen() - 1;
}

void myObject::Resurrect(myObjectList* pList)
{
	if (pList->GetViewGen() <= m_uDeath)
	{
		m_uDeath = UINT_MAX;
	}
}

void myObject::Bury(myObjectList* pList)
{
	if (pList->GetViewGen() < m_uBirth)
	{
		m_uBirth = UINT_MAX;
	}
}

HRGN myObject::GetDisplayRegion() const
{
	if (!this) return FALSE;
	const double radian = angle * M_PI / 180.0;
	const POINT point[4] =
	{
		(LONG)(width_scale*(-width / 2.0) * cos(radian) - height_scale*(-height / 2.0) * sin(radian) + this->x + width / 2.0),
		(LONG)(width_scale*(-width / 2.0) * sin(radian) + height_scale*(-height / 2.0) * cos(radian) + this->y + height / 2.0),
		(LONG)(width_scale*(-width / 2.0) * cos(radian) - height_scale*(height / 2.0) * sin(radian) + this->x + width / 2.0),
		(LONG)(width_scale*(-width / 2.0) * sin(radian) + height_scale*(height / 2.0) * cos(radian) + this->y + height / 2.0),
		(LONG)(width_scale*(width / 2.0) * cos(radian) - height_scale*(height / 2.0) * sin(radian) + this->x + width / 2.0),
		(LONG)(width_scale*(width / 2.0) * sin(radian) + height_scale*(height / 2.0) * cos(radian) + this->y + height / 2.0),
		(LONG)(width_scale*(width / 2.0) * cos(radian) - height_scale*(-height / 2.0) * sin(radian) + this->x + width / 2.0),
		(LONG)(width_scale*(width / 2.0) * sin(radian) + height_scale*(-height / 2.0) * cos(radian) + this->y + height / 2.0),
	};
	return CreatePolygonRgn(point, 4, WINDING);
}

BOOL myObject::HitTest(Gdiplus::REAL x, Gdiplus::REAL y) const
{
	if (!this) return FALSE;
	const HRGN hrgn = GetDisplayRegion();
	const BOOL b = PtInRegion(hrgn, (int)x, (int)y);
	DeleteObject(hrgn);
	return b;
}

BOOL myObject::HitTestRotate(Gdiplus::REAL x, Gdiplus::REAL y) const
{
	if (!this) return FALSE;
	const double radian = angle * M_PI / 180.0;
	const POINT ptRotate =
	{
		(LONG)((height_scale*height / 2.0 + 8)* sin(radian) + this->x + width / 2.0),
		(LONG)(-(height_scale*height / 2.0 + 8) * cos(radian) + this->y + height / 2.0),
	};
	const HRGN hrgn = CreateEllipticRgn(ptRotate.x - 6, ptRotate.y - 6, ptRotate.x + 6, ptRotate.y + 6);
	const BOOL b = PtInRegion(hrgn, (int)x, (int)y);
	DeleteObject(hrgn);
	return b;
}

INT myObject::HitTestResize(Gdiplus::REAL x, Gdiplus::REAL y) const
{
	if (!this) return FALSE;
	const double radian = angle * M_PI / 180.0;
	const POINT ptLeftTop =
	{
		(LONG)(width_scale*(-width / 2.0) * cos(radian) + height_scale*(height / 2.0) * sin(radian) + this->x + width / 2.0),
		(LONG)(width_scale*(-width / 2.0) * sin(radian) - height_scale*(height / 2.0) * cos(radian) + this->y + height / 2.0),
	};
	const POINT ptLeftBottom =
	{
		(LONG)(width_scale*(-width / 2.0) * cos(radian) + height_scale*(-height / 2.0) * sin(radian) + this->x + width / 2.0),
		(LONG)(width_scale*(-width / 2.0) * sin(radian) - height_scale*(-height / 2.0) * cos(radian) + this->y + height / 2.0),
	};
	const POINT ptRightTop =
	{
		(LONG)(width_scale*(width / 2.0) * cos(radian) + height_scale*(height / 2.0) * sin(radian) + this->x + width / 2.0),
		(LONG)(width_scale*(width / 2.0) * sin(radian) - height_scale*(height / 2.0) * cos(radian) + this->y + height / 2.0),
	};
	const POINT ptRightBottom =
	{
		(LONG)(width_scale*(width / 2.0) * cos(radian) + height_scale*(-height / 2.0) * sin(radian) + this->x + width / 2.0),
		(LONG)(width_scale*(width / 2.0) * sin(radian) - height_scale*(-height / 2.0) * cos(radian) + this->y + height / 2.0),
	};

	//Špã
	HRGN hrgn = CreateEllipticRgn(ptLeftTop.x - 10, ptLeftTop.y - 10, ptLeftTop.x + 10, ptLeftTop.y + 10);
	BOOL b = PtInRegion(hrgn, (int)x, (int)y);
	DeleteObject(hrgn);
	if (b) return 1;
	hrgn = CreateEllipticRgn(ptLeftBottom.x - 10, ptLeftBottom.y - 10, ptLeftBottom.x + 10, ptLeftBottom.y + 10);
	b = PtInRegion(hrgn, (int)x, (int)y);
	DeleteObject(hrgn);
	if (b) return 2;
	hrgn = CreateEllipticRgn(ptRightTop.x - 10, ptRightTop.y - 10, ptRightTop.x + 10, ptRightTop.y + 10);
	b = PtInRegion(hrgn, (int)x, (int)y);
	DeleteObject(hrgn);
	if (b) return 3;
	hrgn = CreateEllipticRgn(ptRightBottom.x - 10, ptRightBottom.y - 10, ptRightBottom.x + 10, ptRightBottom.y + 10);
	b = PtInRegion(hrgn, (int)x, (int)y);
	DeleteObject(hrgn);
	if (b) return 4;

	//•Óã
	POINT point = { (LONG)x, (LONG)y };
	if (util::IsOnLine(&ptLeftTop, &ptLeftBottom, &point)) return 5;
	if (util::IsOnLine(&ptLeftBottom, &ptRightBottom, &point)) return 6;
	if (util::IsOnLine(&ptRightBottom, &ptRightTop, &point)) return 7;
	if (util::IsOnLine(&ptRightTop, &ptLeftTop, &point)) return 8;

	return 0;
}

void myObject::Replace(myObjectList* pList, myObject* pNew)
{
	Kill(pList);
	myObjectList::const_iterator it = pList->getiterator(this);
	pList->insert(it, pNew);
}

void myObject::deleteeffect(int i)
{
	effectList->delete_effect(i);
}

effect* myObject::geteffect(int i)const { return effectList->at(i); }
size_t myObject::geteffectsize()const { return effectList->size(); }

void myObject::addeffect(effect* e)
{
	effectList->add(e);
}

void myObject::inserteffect(int n, effect* e)
{
	effectList->insert_effect(n, e);
}

void myObject::replaceeffect(int n, effect* e)
{
	effectList->delete_effect(n);
	effectList->insert_effect(n, e);
}

void myObject::save(HANDLE hFile) const
{
	DWORD dwWriteSize;

	WriteFile(hFile, &x, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	WriteFile(hFile, &y, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	WriteFile(hFile, &angle, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	WriteFile(hFile, &width, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	WriteFile(hFile, &height, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	WriteFile(hFile, &width_scale, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	WriteFile(hFile, &height_scale, sizeof(Gdiplus::REAL), &dwWriteSize, 0);
	WriteFile(hFile, &bLeftRight, sizeof(BOOL), &dwWriteSize, 0);
	WriteFile(hFile, &bUpDown, sizeof(BOOL), &dwWriteSize, 0);

	effectList->save(hFile);
}

void myObject::open(HANDLE hFile)
{
	DWORD dwReadSize;

	ReadFile(hFile, &x, sizeof(Gdiplus::REAL), &dwReadSize, 0);
	ReadFile(hFile, &y, sizeof(Gdiplus::REAL), &dwReadSize, 0);
	ReadFile(hFile, &angle, sizeof(Gdiplus::REAL), &dwReadSize, 0);
	ReadFile(hFile, &width, sizeof(Gdiplus::REAL), &dwReadSize, 0);
	ReadFile(hFile, &height, sizeof(Gdiplus::REAL), &dwReadSize, 0);
	ReadFile(hFile, &width_scale, sizeof(Gdiplus::REAL), &dwReadSize, 0);
	ReadFile(hFile, &height_scale, sizeof(Gdiplus::REAL), &dwReadSize, 0);
	ReadFile(hFile, &bLeftRight, sizeof(BOOL), &dwReadSize, 0);
	ReadFile(hFile, &bUpDown, sizeof(BOOL), &dwReadSize, 0);

	effectList->open(hFile);
}
