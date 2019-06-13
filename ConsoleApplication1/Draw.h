#pragma once
namespace maki {
	using uchar = unsigned char;
	extern int screenWidth;
	extern int screenHeight;
	extern int byteStep;
	void DrawClipLine(const Vector4D &pt1, const Vector4D &pt2, int color, uchar *dest_buffer, int lpitch);
	bool DrawTopTri(float x1, float y1, float x2, float y2, float x3, float y3, int color, uchar *framePtr, int lpitch);
	void PrintTest(int *begin, int total, int color);
}