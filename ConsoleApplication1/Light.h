#pragma once
#include"VectorXD.h"
namespace maki {
	class Light{
	public:
		int state{ 0 };
		int id{ 0 };
		int attr{ 0 };  // 光照类型及其他属性

		int c_ambient{0};   // 环境光强度
		int c_diffuse{ 0 };   // 散射光强度
		int c_specular{ 0 };  // 镜面反射光强度

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
		Light( int id, int state, int attr, int c_ambient, int c_diffuse, int c_specular, Point4D  &pos,
			Vector4D &dir, float kc = 0.0f, float kl = 0.0f,float kq = 0.0f, float spot_inner = 0.0f, float spot_outer = 0.0f, float pf = 0.0f){
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
		int state{ 0 };
		int id{ 0 };
		int attr{ 0 };  // 光照类型及其他属性

		int c_ambient{ 0 };   // 环境光强度
		int c_diffuse{ 0 };   // 散射光强度
		int c_specular{ 0 };  // 镜面反射光强度

		Point4D  pos;       // 光源位置
		Vector4D dir;       // 光源方向
		float kc{ 0.0f }, kl{ 0.0f }, kq{ 0.0f };   // 衰减因子

		float spot_inner{ 0.0f };   // 聚光灯内锥角
		float spot_outer{ 0.0f };   // 聚光灯外锥角
		float pf{ 0.0f };           // 聚光灯指数因子
	public:
		LightBuilder() {}
		LightBuilder& setID(int id) {
			id = id;
			return *this;
		}
		LightBuilder& setState(int state) {
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
		LightBuilder& setBasic(int id, int state, int attr) {
			id = id;
			state = state;
			attr = attr;
			return *this;
		}
		LightBuilder& setIntensity(int c_ambient, int c_diffuse,int c_specular) {
			//光照强度
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
			//设置衰减因子
			kc = kc;
			kl = kl;
			kq = kq;
			return *this;
		}
		LightBuilder& setSpot(float spot_inner = 0.0f, float spot_outer = 0.0f, float pf = 0.0f) {
			//设置聚光灯参数
			spot_inner = spot_inner;
			spot_outer = spot_outer;
			pf = pf;
			return *this;
		}
		
		Light build() {
			return Light(id, state, attr, c_ambient, c_diffuse, c_specular,  pos,
				dir, kc, kl, kq,  spot_inner,  spot_outer , pf );
		}
	};
}
