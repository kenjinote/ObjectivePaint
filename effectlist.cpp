#include <Windows.h>
#include <Gdiplus.h>
#include <vector>
#include "effect.h"
#include "effectshadow.h"
#include "effectblur.h"
#include "effectemission.h"
#include "effectbevel.h"
#include "effectlist.h"
#include "util.h"

effectlist::effectlist()
{
}

effectlist::~effectlist()
{
	clear();
}

void effectlist::clear()
{
	for (effect* pObject : list)
	{
		delete pObject;
	}
	list.clear();
	list.shrink_to_fit();
}

void effectlist::add(effect* newEffect)
{
	return list.push_back(newEffect);
}

effectlist* effectlist::copy() const
{
	return (effectlist*)new effectlist(*this);
}

effectlist::effectlist(const effectlist& other)
{
	const size_t size = other.size();
	for (int i = 0; i < size; ++i)
	{
		effect* e = other.at(i);
		add(e->copy());
	}
}

void effectlist::delete_effect(int i)
{
	list.erase(list.begin() + i);
}

void effectlist::insert_effect(int n, effect* e)
{
	std::vector<effect*>::iterator b = list.begin();
	list.insert(b + n, e);
}

void effectlist::save(HANDLE hFile) const
{
	DWORD dwWriteSize;
	const DWORD nObjectCount = (DWORD)list.size();
	WriteFile(hFile, &nObjectCount, sizeof(DWORD), &dwWriteSize, 0);
	for (effect* pObject : list)
	{
		pObject->save(hFile);
	}
}

void effectlist::open(HANDLE hFile)
{
	DWORD dwReadSize;
	DWORD nObjectCount;
	ReadFile(hFile, &nObjectCount, sizeof(DWORD), &dwReadSize, 0);

	TCHAR ObjectType;
	effect* obj;
	for (DWORD i = 0; i < nObjectCount; ++i)
	{
		obj = nullptr;
		ObjectType = TEXT('\0');
		if (!ReadFile(hFile, &ObjectType, sizeof(TCHAR), &dwReadSize, 0))break;
		if (ObjectType == TEXT('H')) // shadow
		{
			obj = new effectshadow;
		}
		else if (ObjectType == TEXT('B')) // blur
		{
			obj = new effectblur;
		}
		else if (ObjectType == TEXT('E')) // emission
		{
			obj = new effectemission;
		}
		else if (ObjectType == TEXT('V')) // bevel
		{
			obj = new effectbevel;
		}
		if (obj != nullptr)
		{
			obj->open(hFile);
			add(obj);
		}
	}
}

void effectlist::draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, effect::KIND kind, float width, float height) const
{
	for (auto e : list)
	{
		if (e->drawkind() != kind) continue;
		e->draw(g, path, width, height);
	}
}

void effectlist::blur(Gdiplus::Bitmap* pBitmap) const
{
	for (auto e : list)
	{
		if (!e->isblur()) continue;
		e->blur(pBitmap);
	}
}

BOOL effectlist::IsIncludedBlur() const
{
	for (auto e : list)
	{
		if (e->isblur()) return TRUE;
	}
	return FALSE;
}
