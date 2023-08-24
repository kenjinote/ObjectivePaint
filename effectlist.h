#pragma once

#include <vector>
#include "effect.h"

class effect;

typedef std::vector<effect*> EFFECTLIST;

class effectlist
{
	EFFECTLIST list;

public:
	effectlist();
	~effectlist();
	effectlist(const effectlist& other);
	virtual void add(effect* neweffect);
	size_t size() const
	{
		list.begin();
		return list.size();
	}
	effect* at(int i) const
	{
		return list.at(i);
	}
	void clear();
	effectlist* copy()const;
	void delete_effect(int i);
	void insert_effect(int n, effect* e);
	VOID save(HANDLE hFile) const;
	VOID open(HANDLE hFile);
	void draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, effect::KIND kind, float width, float height) const;
	void blur(Gdiplus::Bitmap* pBitmap) const;
	BOOL IsIncludedBlur() const;
};

