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
	union Rgba
	{
		int rgba;                    // compressed format
		uchar rgba_M[4];             // array format
		struct { uchar a, b, g, r; }; // explict name format
	}; // end union

	class Light{
	public:
		LightState state{ LightState::kOff };
		int id{ 0 };
		int attr{ 0 };  // �������ͼ���������

		Rgba c_ambient{0};   // ������ǿ��
		Rgba c_diffuse{ 0 };   // ɢ���ǿ��
		Rgba c_specular{ 0 };  // ���淴���ǿ��

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
		Light(int id, LightState state, int attr, int c_ambient, int c_diffuse, int c_specular, Point4D  &pos,
			Vector4D &dir, float kc , float kl ,float kq , float spot_inner, float spot_outer , float pf ){
			id = id;
			state = state;
			attr = attr;

			c_ambient = c_ambient;
			c_diffuse = c_diffuse;
			c_specular = c_specular;

			pos = pos;

			dir = dir;
			dir.Normalize();

			kc = kc;
			kl = kl;
			kq = kq;

			spot_inner = spot_inner;
			spot_outer = spot_outer;
			pf = pf;
		}
	};

	class LightBuilder {
	private:
		int id{ 0 };
		LightState state{ LightState::kOff };		
		int attr{0 };  // �������ͼ���������

		Rgba c_ambient{ 0 };   // ������ǿ��
		Rgba c_diffuse{ 0 };   // ɢ���ǿ��
		Rgba c_specular{ 0 };  // ���淴���ǿ��

		Point4D  pos;       // ��Դλ��
		Vector4D dir;       // ��Դ����
		float kc{ 0.0f }, kl{ 0.0f }, kq{ 0.0f };   // ˥������

		float spot_inner{ 0.0f };   // �۹����׶��
		float spot_outer{ 0.0f };   // �۹����׶��
		float pf{ 0.0f };           // �۹��ָ������
	public:
		LightBuilder() {}
		LightBuilder& setID(int id) {
			id = id;
			return *this;
		}
		LightBuilder& setState(LightState state) {
			state = state;
			return *this;
		}		
		LightBuilder& setAttr(int attr) {
			attr = attr;
			return *this;
		}
		LightBuilder& setAmbient(int c_ambient) {
			c_ambient = c_ambient;
			return *this;
		}
		LightBuilder& setDiffuse(int c_diffuse) {
			c_diffuse = c_diffuse;
			return *this;
		}
		LightBuilder& setSpecular(int c_specular) {
			c_specular = c_specular;
			return *this;
		}
		LightBuilder& setBasic(int id, LightState state, int attr) {
			id = id;
			state = state;
			attr = attr;
			return *this;
		}
		LightBuilder& setIntensity(int c_ambient, int c_diffuse,int c_specular) {
			//����ǿ��
			c_ambient = c_ambient;
			c_diffuse = c_diffuse;
			c_specular = c_specular;
			return *this;
		}
		LightBuilder& setPos(Point4D &pos) {
			pos = pos;
			return *this;
		}
		LightBuilder& setDir(Vector4D &dir) {
			dir = dir;
			return *this;
		}
		LightBuilder& setAttenFactor(float kc, float kl, float kq) {
			//����˥������
			kc = kc;
			kl = kl;
			kq = kq;
			return *this;
		}
		LightBuilder& setSpot(float spot_inner = 0.0f, float spot_outer = 0.0f, float pf = 0.0f) {
			//���þ۹�Ʋ���
			spot_inner = spot_inner;
			spot_outer = spot_outer;
			pf = pf;
			return *this;
		}
		
		Light build() {
			//return Light(id,state,attr,c_ambient,c_diffuse,c_specular,pos,dir,kc,kl,kq,spot_inner,spot_outer,pf);
			return Light();
		}
	};
}
