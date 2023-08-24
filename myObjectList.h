#pragma once

#include "myObject.h"

#include <list>
#include <vector>
#include <algorithm>
#define DATALIST_SIZE				3	// リストの数
#define DATALIST_PRIMITIVE			0

typedef std::list<myObject*> MDDATAPTRLIST;

class myObjectList
{
public:
	typedef MDDATAPTRLIST::iterator iterator;
	typedef MDDATAPTRLIST::const_iterator const_iterator;
	typedef MDDATAPTRLIST::reverse_iterator reverse_iterator;
	typedef MDDATAPTRLIST::const_reverse_iterator const_reverse_iterator;

protected:
	UINT	m_uViewGen;
	UINT	m_uCurrGen;
	MDDATAPTRLIST list;
	std::vector<HGLOBAL> m_listPictureHandlePool; // 画像を保持しておく配列
	std::vector<Gdiplus::Bitmap*> m_listPictureBitmapPool; // 画像を保持しておく配列

public:
	myObjectList();
	virtual ~myObjectList();

	UINT GetViewGen() const;
	UINT GetCurrGen() const;

	BOOL undo();
	BOOL redo();
	BOOL IsUndoAvailable() const;
	BOOL IsRedoAvailable() const;
	void reset();
	size_t size() const
	{
		return list.size();
	}
	virtual void add(myObject* newData);
	virtual void print(HDC hdc, float x, float y, float nImageWidth, float nImageHeight, float dScale, Gdiplus::Bitmap* pBitmapBack, Gdiplus::Bitmap* pBitmapFront, bool isprinting = false) const;
	void EditStart();
	void newlife(myObjectList* pOrg);
	void clear();
	BOOL savefile(LPCTSTR lpszFilePath, Gdiplus::Bitmap* pBitmapBack, Gdiplus::Bitmap* pBitmapFront, std::vector<Gdiplus::Bitmap*>*listMask) const;
	BOOL openfile(LPCTSTR lpszFilePath, Gdiplus::Bitmap** pBitmapBack, Gdiplus::Bitmap** pBitmapFront, std::vector<Gdiplus::Bitmap*>*listMask);
	void allunselect();
	int getpicturecount() const;
	int getstringcount() const;
	void addpicture(HGLOBAL hImageOriginal, Gdiplus::Bitmap* pBitmap);

	iterator begin() { return list.begin(); }
	const_iterator begin() const
	{
		return list.begin();
	}
	iterator end()
	{
		return list.end();
	}
	const_iterator end() const
	{
		return list.end();
	}
	const_iterator getiterator(const myObject* obj)
	{
		for (const_iterator it = begin(); it != end(); ++it)
		{
			if (*it == obj)
				return it;
		}
		return end();
	}
	void insert(const_iterator it, myObject* obj)
	{
		list.insert(it, obj);
	}
	const_reverse_iterator rbegin()
	{
		return list.rbegin();
	}
	const_reverse_iterator rend()
	{
		return list.rend();
	}
};

