#pragma once
#include"VectorXD.h"
#include <vector>
//https://zhuanlan.zhihu.com/p/43899251
namespace maki {
	using uchar = unsigned char;
	//状态
	enum class LightState {
		kOn,
		kOff,
	};
	//光源类型
	enum LightType {
		kAmbient		= 0x0001,//环境光
		kInfinite		= 0x0002,//无穷远光源
		kPoint			= 0x0004,//点光源
		kSpotLight1		= 0x0008,//聚光灯
		kSpotLight2		= 0x0010, //聚光灯
	};
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
		int attr{ 0 };  // 光照类型及其他属性

		Color c_ambient{0};   // 环境光强度
		Color c_diffuse{ 0 };   // 散射光强度
		Color c_specular{ 0 };  // 镜面反射光强度

		Point4D  pos;       // 光源位置
		Vector4D dir;       // 光源方向
		float kc{ 0.0f }, kl{ 0.0f }, kq{ 0.0f };   // 衰减因子

		float spot_inner{ 0.0f };   // 聚光灯内锥角
		float spot_outer{ 0.0f };   // 聚光灯外锥角
		float pf{ 0.0f };           // 聚光灯指数因子

		int   iaux1{ 0 }, iaux2{ 0 }; // 辅助变量
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
		int attr{0 };  // 光照类型及其他属性

		Color c_ambient{ 0 };   // 环境光强度
		Color c_diffuse{ 0 };   // 散射光强度
		Color c_specular{ 0 };  // 镜面反射光强度

		Point4D  pos;       // 光源位置
		Vector4D dir;       // 光源方向
		float kc{ 0.0f }, kl{ 0.0f }, kq{ 0.0f };   // 衰减因子

		float spot_inner{ 0.0f };   // 聚光灯内锥角
		float spot_outer{ 0.0f };   // 聚光灯外锥角
		float pf{ 0.0f };           // 聚光灯指数因子
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
			//光照强度
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
			//设置衰减因子
			kc = _kc;
			kl = _kl;
			kq = _kq;
			return *this;
		}
		LightBuilder& setSpot(float _spot_inner = 0.0f, float _spot_outer = 0.0f, float _pf = 0.0f) {
			//设置聚光灯参数
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
