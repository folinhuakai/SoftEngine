#pragma once
#include<string>

namespace maki {
	using uchar = unsigned char;
	class BitMap {
		int state;          // state of bitmap
		int attr;           // attributes of bitmap
		int x, y;            // position of bitmap
		int width, height;  // size of bitmap
		int numBytes;      // total bytes of bitmap
		int bpp;            // bits per pixel
		uchar *buffer;      // pixels of bitmap
	};
	class Material {
		int state;           // 材质状态
		int id;              
		std::string name;    // 名称
		int  attr;           // 属性,着色模式、着色方法、环境及纹理等
		int color;            // 颜色
		float ka, kd, ks, power; // 对环境光、散射光、镜面反射光的反射系数、镜面反射指数

		int ra, rd, rs;       // 预先计算得到的颜色与反射系数积

		std::string texturePath;  // 纹理位置
		BitMap texture;    // actual texture map (if any)

		int   iaux1, iaux2;      //辅助变量
		float faux1, faux2;
		void *ptr;

	};
}