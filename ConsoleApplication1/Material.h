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
		int state;           // ����״̬
		int id;              
		std::string name;    // ����
		int  attr;           // ����,��ɫģʽ����ɫ�����������������
		int color;            // ��ɫ
		float ka, kd, ks, power; // �Ի����⡢ɢ��⡢���淴���ķ���ϵ�������淴��ָ��

		int ra, rd, rs;       // Ԥ�ȼ���õ�����ɫ�뷴��ϵ����

		std::string texturePath;  // ����λ��
		BitMap texture;    // actual texture map (if any)

		int   iaux1, iaux2;      //��������
		float faux1, faux2;
		void *ptr;

	};
}