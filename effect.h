#pragma once

class effect
{
public:
	enum KIND
	{
		Before, // 本体を描く前に描画
		After,  // 本体を描いた後に描画
		None	// 本体を描かず自前で描画（ぼかし）
	};
	effect() {};
	virtual ~effect() {};

	virtual BOOL isshadow() const { return FALSE; }
	virtual BOOL isblur() const { return FALSE; }
	virtual BOOL isemission() const { return FALSE; }
	virtual BOOL isbevel() const { return FALSE; }
	
	virtual effect* copy() const = 0;
	virtual void getliststring(LPTSTR, int) const = 0;
	virtual KIND drawkind() const = 0; // 描画
	virtual void draw(Gdiplus::Graphics* g, Gdiplus::GraphicsPath* path, float width, float height) const = 0;
	virtual void blur(Gdiplus::Bitmap*pBitmap) const {};

	virtual void save(HANDLE hFile) const = 0;
	virtual void open(HANDLE hFile) = 0;
};

