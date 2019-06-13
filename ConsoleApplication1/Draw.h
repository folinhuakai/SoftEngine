#pragma once
namespace maki {
	using uchar = unsigned char;
	extern int screenWidth;
	extern int screenHeight;
	extern int byteStep;
	void DrawClipLine(const Vector4D &pt1, const Vector4D &pt2, int color, uchar *dest_buffer, int lpitch);
	bool DrawTopTri(float x1, float y1, float x2, float y2, float x3, float y3, int color, uchar *framePtr, int lpitch);
	bool DrawBottomTri(float x1, float y1,//顶点1,2,3需要遵从一定是顺序
		float x2, float y2,
		float x3, float y3,
		int color,
		uchar *framePtr, int lpitch);
	void PrintTest(int *begin, int total, int color);
	inline void DrawLineHorizontal(int start, int end, uchar *destAddr, int color) {//destAddr 每行起始地址
		destAddr = destAddr + start * byteStep;
		for (; start <= end; ++start, destAddr += byteStep) {
			*(int *)destAddr = color;
		}
	}
}