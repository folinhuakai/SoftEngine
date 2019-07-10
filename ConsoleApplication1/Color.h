#pragma once
namespace maki {
	/***********************ÑÕÉ«***************************/
	class Color {
	public:
		union {
			int rgba;
			unsigned char arr[4];
			struct
			{
				unsigned char r, g, b, a;
			};
		};
		Color() = default;
		Color(const Color &_col):rgba(_col.rgba){}
		Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255) :r(_r), g(_g), b(_b), a(_a) {}
		Color(int _rgba) :rgba(_rgba) {}
		void SetColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255) {
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}
	};
}