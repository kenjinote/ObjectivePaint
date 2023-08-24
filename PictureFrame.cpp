#include <Windows.h>
#include <gdiplus.h>
#include <vector>
#include "PictureFrame.h"
#include "util.h"

void FloodFill(Gdiplus::Bitmap* bmp, Gdiplus::Point pt);

PictureFrame::PictureFrame() : _source(nullptr), _tmp(nullptr)
{
}

PictureFrame::~PictureFrame()
{
	_frameRects.clear();
	size_t size = _frameData.size();
	for (size_t i = 0; i < size; ++i)
	{
		delete _frameData[i];
	}
	_frameData.clear();
}

size_t PictureFrame::framesNum() const
{
	return _frameData.size();
}

Gdiplus::Region* PictureFrame::getFrameDataAt(UINT index) const
{
	return _frameData[index];
}

// 写真フレームを検索
void PictureFrame::search(Gdiplus::Bitmap* bitmapData)
{
	// 検出対象となる画像のビットマップデータ
	_source = bitmapData;

	// 検出された写真フレームの範囲を保持する配列をリセット
	_frameRects.clear();

	// 検出された写真フレームのデータを保持する配列をリセット
	for (auto region : _frameData)
	{
		delete region;
	}
	_frameData.clear();

	// 検出した透明部分を描画するための一時的なビットマップ画像
	delete _tmp;
	_tmp = new Gdiplus::Bitmap(bitmapData->GetWidth(), bitmapData->GetHeight(), PixelFormat32bppARGB); // 黒で塗りつぶされていると仮定
	{
		Gdiplus::Graphics g(_tmp);
		g.Clear(Gdiplus::Color(255, 255, 255));
	}

	for (UINT py = 0; py < _tmp->GetHeight() - _MIN_LENGTH; ++py)
	{
		for (UINT px = 0; px < _tmp->GetWidth() - _MIN_LENGTH; ++px)
		{
			if (isTransparence(px, py))
			{
				RECT rect;
				// (px, py)が透明
				// すでに検索済みの写真フレームの範囲内にある場合は無視する
				BOOL flag = true;
				for ( RECT r : _frameRects)
				{
					POINT point = { (LONG)px, (LONG)py };
					if (PtInRect(&r, point)) //(/*rect.contains(px, py)*/)
					{
						px = r.right;
						flag = false;
						rect = r;
						break;
					}
				}

				if (flag)
				{
					// 透明部分の境界をトレース
					rect = traceEdge(px, py);

					// エリアの右端からサーチを再開する
					px = rect.right;
				}
			}
		}
	}
}

void PictureFrame::setFrame(std::vector<Gdiplus::Bitmap*>* listMask, double scale)
{
	// 検出された写真フレームの範囲を保持する配列をリセット
	_frameRects.clear();

	// 検出された写真フレームのデータを保持する配列をリセット
	for (auto region : _frameData)
	{
		delete region;
	}
	_frameData.clear();

	for (auto mask : *listMask)
	{
		Gdiplus::Bitmap* pTemp = (Gdiplus::Bitmap*)mask->GetThumbnailImage((UINT)(mask->GetWidth() * scale), (UINT)(mask->GetHeight() * scale));
		Gdiplus::Region* region = util::CreateRegionFromImage8bpp(pTemp);
		delete pTemp;
		_frameData.push_back(region);
	}
}

// 透明部分の境界をトレース
RECT PictureFrame::traceEdge(UINT px, UINT py)
{
	POINT _startPoint = { (LONG)px, (LONG)py };

	// 検出した透明部分に赤いピクセルを描画していく
	_tmp->SetPixel(px, py, Gdiplus::Color(0, 0, 0)); //_tmp.setPixel32(px, py, 0xFFFF0000);

	// 調べる座標
	POINT _checkPoint = { (LONG)px, (LONG)py };

	// 調べる方向(上0, 右1, 下2, 左3)
	UINT _checkDirection = 1;

	POINT minPoint = { (LONG)px, (LONG)py };
	POINT maxPoint = { (LONG)px, (LONG)py };
	for (;;)
	{
		BOOL loopBreake = TRUE;
		for (UINT d = 0; d < 4; ++d)
		{
			// 次に調べる座標
			POINT p = pointOfDirection(_checkPoint, _checkDirection);

			if (p.x == _startPoint.x && p.y == _startPoint.y)
			{
				// スタート地点と同じなら終了
				break;
			}
			else if (isTransparence(p.x, p.y))
			{
				// 座標pが透明なので、tmp画像に赤いピクセルを描画
				_tmp->SetPixel(p.x, p.y, Gdiplus::Color(0, 0, 0)); //_tmp.setPixel32(px, py, 0xFFFF0000);

				// 座標pを新たな基点としてセット
				_checkPoint = p;
				_checkDirection = (_checkDirection + 3) % 4;
				if (p.x > maxPoint.x)
				{
					// 透明範囲の右端の座標が更新されるなら、更新する
					maxPoint.x = p.x;
				}
				else if (p.x < minPoint.x)
				{
					// 透明範囲の左端の座標が更新されるなら、更新する
					minPoint.x = p.x;
				}
				if (p.y > maxPoint.y)
				{
					// 透明範囲の下端の座標が更新されるなら、更新する
					maxPoint.y = p.y;
				}
				else if (p.y < minPoint.y)
				{
					// 透明範囲の上端の座標が更新されるなら、更新する
					minPoint.y = p.y;
				}

				loopBreake = FALSE;
				break;
			}
			else
			{
				// 透明じゃないので、方向を変える
				_checkDirection = (_checkDirection + 1) % 4;
			}
		}
		if (loopBreake)
		{
			break;
		}
	}

	// トレース完了
	static RECT rect;
	SetRect(&rect, minPoint.x, minPoint.y, minPoint.x + (maxPoint.x - minPoint.x) + 1, minPoint.y + (maxPoint.y - minPoint.y) + 1);
	if (rect.right - rect.left > _MIN_LENGTH && rect.bottom - rect.top > _MIN_LENGTH)
	{
		// 得られた範囲が_MIN_LENGTH より大きければ写真フレームとして処理する

		// 白枠で囲まれたエリアを白で塗りつぶす
		FloodFill(_tmp, Gdiplus::Point(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2));

		_frameRects.push_back(rect);

		// 写真フレームのサイズのみを切り出した画像を生成
		Gdiplus::Bitmap* clip = new Gdiplus::Bitmap(_tmp->GetWidth(), _tmp->GetHeight(), PixelFormat8bppIndexed);
		{
			Gdiplus::Graphics g(clip);
			g.DrawImage(_tmp, 0, 0, 0, 0, _tmp->GetWidth(), _tmp->GetHeight(), Gdiplus::Unit::UnitPixel);
		}

		// PNG画像化
		Gdiplus::Region *r = util::CreateRegionFromImage8bpp(clip);
		_frameData.push_back(r);
		delete clip;

		//tmp画像をクリアする
		{
			Gdiplus::Graphics g(_tmp);
			g.Clear(Gdiplus::Color(255, 255, 255));
		}
	}
	return rect;
}

// 座標pから方向directionにある座標を返す
POINT PictureFrame::pointOfDirection(POINT p, UINT direction)
{
	static POINT d;
	d.x = p.x;
	d.y = p.y;
	switch (direction)
	{
	case 0:
		d.y = p.y - 1;
		break;
	case 1:
		d.x = p.x + 1;
		break;
	case 2:
		d.y = p.y + 1;
		break;
	case 3:
		d.x = p.x - 1;
		break;
	}
	return d;
}

BOOL PictureFrame::isTransparence(UINT px, UINT py)
{
	if (px >= 0 && px < _source->GetWidth() && py >= 0 && py < _source->GetHeight())
	{
		// ピクセルのアルファ値を取得
		Gdiplus::Color pixelValue;
		_source->GetPixel(px, py, &pixelValue);
		UINT alphaValue = pixelValue.GetAlpha();

		if (px <= 2 || px >= _source->GetWidth() - 2 || py <= 2 || py >= _source->GetHeight() - 2)
		{
			// 画像の周囲がきちんと不透明にされていないケースが多々あるので、周囲2pxはアルファ0の場合のみ「透明」と判定
			return alphaValue == 0;
		}
		else
		{
			// 完全な不透明でなければ、「透明」と判定
			return alphaValue < 255;
		}
	}
	else
	{
		// 画像のエリア外は「透明じゃない」
		return FALSE;
	}
}

bool *PixelsChecked;

int CoordsToIndex(int x, int y, int stride)
{
	return (stride*y) + (x * 4);
}

BOOL CheckPixel(byte* px, byte* startcolor)
{
	return (px[0] == startcolor[0]);
}

void LinearFloodFill4(byte* scan0, int x, int y, Gdiplus::Size bmpsize, int stride, byte* startcolor)
{
	int* p = (int*)(scan0 + (CoordsToIndex(x, y, stride)));

	int LFillLoc = x;
	int* ptr = p;
	for (;;)
	{
		ptr[0] = Gdiplus::Color::Black;
		PixelsChecked[y*(bmpsize.Width + 1) + LFillLoc] = true;
		LFillLoc--;
		ptr -= 1;
		if (LFillLoc <= 0 ||
			!CheckPixel((byte*)ptr, startcolor) ||
			(PixelsChecked[y*(bmpsize.Width + 1) + LFillLoc]))
			break;
	}
	++LFillLoc;

	int RFillLoc = x;
	ptr = p;
	for (;;)
	{
		ptr[0] = Gdiplus::Color::Black;
		PixelsChecked[y*(bmpsize.Width + 1) + RFillLoc] = true;
		RFillLoc++;
		ptr += 1;
		if (RFillLoc >= bmpsize.Width ||
			!CheckPixel((byte*)ptr, startcolor) ||
			(PixelsChecked[y*(bmpsize.Width + 1) + RFillLoc]))
			break;
	}
	--RFillLoc;

	ptr = (int*)(scan0 + CoordsToIndex(LFillLoc, y, stride));
	for (int i = LFillLoc; i <= RFillLoc; ++i)
	{
		if (y>0 &&
			CheckPixel((byte*)(scan0 + CoordsToIndex(i, y - 1, stride)), startcolor) &&
			(!(PixelsChecked[(y - 1)*(bmpsize.Width + 1) + i])))
			LinearFloodFill4(scan0, i, y - 1, bmpsize, stride, startcolor);
		if (y<(bmpsize.Height - 1) &&
			CheckPixel((byte*)(scan0 + CoordsToIndex(i, y + 1, stride)), startcolor) &&
			(!(PixelsChecked[(y + 1)*(bmpsize.Width + 1) + i])))
			LinearFloodFill4(scan0, i, y + 1, bmpsize, stride, startcolor);
		ptr += 1;
	}
}

void FloodFill(Gdiplus::Bitmap* bmp, Gdiplus::Point pt)
{
	Gdiplus::BitmapData bmpData;
	bmp->LockBits(
		&Gdiplus::Rect(0, 0, bmp->GetWidth(), bmp->GetHeight()),
		Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite,
		PixelFormat32bppARGB, &bmpData);
	VOID* Scan0 = bmpData.Scan0;
	{
		byte * scan0 = (byte *)(void *)Scan0;
		int loc = CoordsToIndex(pt.X, pt.Y, bmpData.Stride);
		int color = *((int*)(scan0 + loc));
		PixelsChecked = new bool[(bmpData.Width + 1)*(bmpData.Height + 1)];
		ZeroMemory(PixelsChecked, sizeof(bool)*((bmpData.Width + 1)*(bmpData.Height + 1)));
		LinearFloodFill4(scan0, pt.X, pt.Y,
			Gdiplus::Size(bmpData.Width, bmpData.Height),
			bmpData.Stride,
			(byte*)&color);
		delete[]PixelsChecked;
	}
	bmp->UnlockBits(&bmpData);
}
