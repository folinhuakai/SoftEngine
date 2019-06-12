#pragma once
namespace maki {
	using uchar = unsigned char;
	extern int screenWidth;
	extern int screenHeight;
	void DrawClipLine(const Vector4D &pt1, const Vector4D &pt2, int color, uchar *dest_buffer, int lpitch);
}