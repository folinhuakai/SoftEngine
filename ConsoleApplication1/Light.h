#pragma once
#include"VectorXD.h"
#include <vector>
//https://zhuanlan.zhihu.com/p/43899251
namespace maki {
	using uchar = unsigned char;
	//״̬
	enum class LightState {
		kOn,
		kOff,
	};
	//��Դ����
	enum LightType {
		kAmbient		= 0x0001,//������
		kInfinite		= 0x0002,//����Զ��Դ
		kPoint			= 0x0004,//���Դ
		kSpotLight1		= 0x0008,//�۹��
		kSpotLight2		= 0x0010, //�۹��
	};
	/***********************��ɫ***************************/
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
		Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255) :r(_r), g(_g), b(_b), a(_a) {}
		Color(int _rgba) :rgba(_rgba) {}
		void SetColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255) {
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}
	};

	class Light{
	public:
		LightState state{ LightState::kOff };
		int id{ 0 };
		int attr{ 0 };  // �������ͼ���������

		Color c_ambient{0};   // ������ǿ��
		Color c_diffuse{ 0 };   // ɢ���ǿ��
		Color c_specular{ 0 };  // ���淴���ǿ��

		Point4D  pos;       // ��Դλ��
		Vector4D dir;       // ��Դ����
		float kc{ 0.0f }, kl{ 0.0f }, kq{ 0.0f };   // ˥������

		float spot_inner{ 0.0f };   // �۹����׶��
		float spot_outer{ 0.0f };   // �۹����׶��
		float pf{ 0.0f };           // �۹��ָ������

		int   iaux1{ 0 }, iaux2{ 0 }; // ��������
		float faux1{ 0.0f }, faux2{ 0.0f };
		void *ptr;

		Light() = default;
		Light(int _id, LightState _state, int _attr, Color &_c_ambient, Color &_c_diffuse, Color &_c_specular, Point4D  &_pos,
			Vector4D &_dir, float _kc , float _kl ,float _kq , float _spot_inner, float _spot_outer , float _pf ){
			id = _id;
			state = _state;
			attr = _attr;

			c_ambient = _c_ambient;
			c_diffuse = _c_diffuse;
			c_specular = _c_specular;

			pos = _pos;

			dir = _dir;
			dir.Normalize();

			kc = _kc;
			kl = _kl;
			kq = _kq;

			spot_inner = _spot_inner;
			spot_outer = _spot_outer;
			pf = _pf;
		}
	};

	class LightBuilder {
	private:
		int id{ 0 };
		LightState state{ LightState::kOff };		
		int attr{0 };  // �������ͼ���������

		Color c_ambient{ 0 };   // ������ǿ��
		Color c_diffuse{ 0 };   // ɢ���ǿ��
		Color c_specular{ 0 };  // ���淴���ǿ��

		Point4D  pos;       // ��Դλ��
		Vector4D dir;       // ��Դ����
		float kc{ 0.0f }, kl{ 0.0f }, kq{ 0.0f };   // ˥������

		float spot_inner{ 0.0f };   // �۹����׶��
		float spot_outer{ 0.0f };   // �۹����׶��
		float pf{ 0.0f };           // �۹��ָ������
	public:
		LightBuilder() {}
		LightBuilder& setID(int _id) {
			id = _id;
			return *this;
		}
		LightBuilder& setState(LightState _state) {
			state = _state;
			return *this;
		}		
		LightBuilder& setAttr(int _attr) {
			attr = _attr;
			return *this;
		}
		LightBuilder& setAmbient(int _c_ambient) {
			c_ambient = _c_ambient;
			return *this;
		}
		LightBuilder& setDiffuse(int _c_diffuse) {
			c_diffuse = _c_diffuse;
			return *this;
		}
		LightBuilder& setSpecular(int _c_specular) {
			c_specular = _c_specular;
			return *this;
		}
		LightBuilder& setBasic(int _id, LightState _state, int _attr) {
			id = _id;
			state = _state;
			attr = _attr;
			return *this;
		}
		LightBuilder& setIntensity(int _c_ambient, int _c_diffuse,int _c_specular) {
			//����ǿ��
			c_ambient = _c_ambient;
			c_diffuse = _c_diffuse;
			c_specular = _c_specular;
			return *this;
		}
		LightBuilder& setPos(Point4D &_pos) {
			pos = _pos;
			return *this;
		}
		LightBuilder& setDir(Vector4D &_dir) {
			dir = _dir;
			return *this;
		}
		LightBuilder& setAttenFactor(float _kc, float _kl, float _kq) {
			//����˥������
			kc = _kc;
			kl = _kl;
			kq = _kq;
			return *this;
		}
		LightBuilder& setSpot(float _spot_inner = 0.0f, float _spot_outer = 0.0f, float _pf = 0.0f) {
			//���þ۹�Ʋ���
			spot_inner = _spot_inner;
			spot_outer = _spot_outer;
			pf = _pf;
			return *this;
		}
		
		Light build() {
			return Light(id,state,attr,c_ambient,c_diffuse,c_specular,pos,dir,kc,kl,kq,spot_inner,spot_outer,pf);
		}
	};
}
