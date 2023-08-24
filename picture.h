#pragma once

#include "myObject.h"

class picture : public myObject
{
public:
	picture();
	~picture();
	picture(const picture& other);
	virtual myObject* Copy() const override;
	BOOL LoadPicture(LPCTSTR lpszFilePath);
	virtual void draw(Gdiplus::Graphics* g, bool isprinting = false) const override;
	virtual BOOL IsSelected()const override { return m_bSelected; }
	virtual VOID SetSelect(BOOL bSelect) override { m_bSelected = bSelect; }
	virtual BOOL IsPicture() const override { return TRUE; }
	virtual void SetAlpha(Gdiplus::REAL alpha) override
	{
		this->alpha = alpha;
	}
	virtual Gdiplus::REAL GetAlpha() const override
	{
		return alpha;
	}
	BOOL GetSmoothing() const
	{
		return m_bSmoothing;
	}
	VOID SetSmoothing(BOOL bSmoothing)
	{
		m_bSmoothing = bSmoothing;
	}
	INT GetFrameNumber() const { return m_nFrameNumber; }
	VOID SetFrameNumber(int n) { m_nFrameNumber = n; }
	virtual void save(HANDLE hFile) const override;
	virtual void open(HANDLE hFile) override;
	void setbitmapfromoriginal(HGLOBAL hImageOriginal);
	void getbitmap(Gdiplus::Bitmap** pBitmap, HGLOBAL* pBitmapOriginal) const
	{
		*pBitmap = m_pBitmap;
		*pBitmapOriginal = m_hImageOriginal;
	}
	void setbitmap(Gdiplus::Bitmap* pBitmap, HGLOBAL pBitmapOriginal)
	{
		m_pBitmap = pBitmap;
		m_hImageOriginal = pBitmapOriginal;
	}

private:
	BOOL m_bSmoothing;
	Gdiplus::Bitmap* m_pBitmap; // 参照だけ管理はobjectlistで
	//Gdiplus::Bitmap* m_pBitmapOriginal; // 参照だけ管理はobjectlistで // 今回削除予定
	HGLOBAL m_hImageOriginal; // オリジナルイメージのバイトデータ
	Gdiplus::REAL alpha;
	INT m_nFrameNumber;
};
