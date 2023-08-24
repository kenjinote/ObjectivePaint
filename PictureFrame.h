#pragma once

#include <vector>

class PictureFrame
{
	static const UINT _MIN_LENGTH = 50;		// これより小さいエリアは、写真フレームとは判定しない

	Gdiplus::Bitmap* _source;
	Gdiplus::Bitmap* _tmp;
	std::vector<RECT> _frameRects;
	std::vector<Gdiplus::Region*> _frameData;

public:
	PictureFrame();
	~PictureFrame();
	size_t framesNum() const;
	Gdiplus::Region* getFrameDataAt(UINT index) const;
	void search(Gdiplus::Bitmap* bitmapData);
	void setFrame(std::vector<Gdiplus::Bitmap*>* listMask, double scale);
private:
	RECT traceEdge(UINT px, UINT py);
	POINT pointOfDirection(POINT p, UINT direction);
	BOOL isTransparence(UINT px, UINT py);
};

