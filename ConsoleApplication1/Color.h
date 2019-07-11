#pragma once
namespace maki {
	/***********************颜色***************************/
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
		Color operator+= (const Color& c) {
			r = std::max(r + c.r, 255);
			g = std::max(g + c.g, 255);
			b = std::max(b + c.b, 255);
			a = 255;
			return *this;
		}

		//颜色乘法
		Color operator*= (const Color& c) {
			r = r * c.r >> 8;
			g = g * c.g >> 8;
			b = b * c.b >> 8;
			a = a * c.a >> 8;
			return *this;
		}

	};
	//颜色加法
	inline Color operator+ (const Color& c1,const Color& c2) {
		Color res;
		res.r = std::max(c1.r + c2.r,255);
		res.g = std::max(c1.g + c2.g,255);
		res.b = std::max(c1.b + c2.b,255);
		res.a = 255;
		return res;
	}

	//颜色乘法
	inline Color operator* (const Color& c1, const Color& c2){
		Color res;
		res.r = c1.r * c2.r >> 8;
		res.g = c1.g * c2.g >> 8;
		res.b = c1.b * c2.b >> 8;
		res.a = c1.a * c2.a >> 8;
		return res;
	}

	//颜色乘法
	inline Color operator* (const Color& c,float num){
		int r_base = c.r;
		int g_base = c.g;
		int b_base = c.b;
		int factor = static_cast<int>(num * 128);
		Color res;
		res.r = (r_base * factor) >> 7;
		res.g = (g_base * factor) >> 7;
		res.b = (b_base * factor) >> 7;
		res.a = 255;
		return res;
	}

	inline Color operator* (float num, const Color& c) {
		return c*num;
	}
}