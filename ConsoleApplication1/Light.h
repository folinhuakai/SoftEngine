#pragma once
#include"VectorXD.h"
#include "Color.h"
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
	//光源基类
	class LightBase
	{
	public:
		//构造函数
		LightBase ()= default;
		LightBase(const LightType _type,const Color& _color) :type(_type),color(_color) {}
		//获取光照类型
		LightType GetType() { return type; }
		//获取光照强度
		virtual Color LightCalculate(const Vector4D &_pos, const Vector4D &_normal) {
			return color;
		}
		Vector4D GetPos() {
			return pos;
		}
		LightState state{ LightState::kOff };
	protected:
		LightType type;
		Color color;
		Vector4D pos;
	};

	//平行光
	class DirectionLight : public LightBase
	{
	public:
		//平行光构造函数
		DirectionLight(const Color& _color, const Vector4D &_dir) :LightBase(LightType::kInfinite,_color), dir(_dir) {}
		//计算光照强度
		Color LightCalculate(const Vector4D &_pos, const Vector4D &_normal)  override {
			float dot = _normal * (-dir);
			if (dot > 0) {
				return color;
			}
			else
			{
				return Color(0, 0, 0, 255);
			}
		}
	private:
		//光源方向
		Vector4D dir;//单位向量
	};

	//点光源
	class PointLight : public LightBase
	{
	public:
		//点光源构造函数
		PointLight(const Color& _color, const Vector4D& _pos, float _kc, float _kl, float _kq) 
			:LightBase(LightType::kPoint,_color), kc(_kc), kl(_kl), kq(_kq) {
			pos = _pos; }
		//计算光源强度
		Color LightCalculate(const Vector4D &_VertexPos, const Vector4D &_normal) override {
			auto light_vec = pos - _VertexPos;//顶点到光源向量
			float dot = _normal *light_vec;
			if (dot > 0) {
				//当一次因子和二次因子为0简化计算
				if (kl == 0 && kq == 0)
				{
					float atten = 1 / kc;
					auto _color = color * atten;
					return _color;
				}
				else
				{
					float dist = light_vec.LengthDW();
					float atten = 1 / (kc + kl * sqrt(dist) + kq * dist);
					auto _color = color * atten;
					return _color;
				}
			}
			else
			{
				return Color(0, 0, 0, 255);
			}
		}
	private:
		//常数衰减因子
		float kc;
		//线性衰减因子
		float kl;
		//二次衰减因子
		float kq;
	};

	//简单聚光灯（与点光源类似）
	class SimpleSpotLight : public LightBase
	{
	public:
		//简单聚光灯构造函数
		SimpleSpotLight(const Color& _color, const Vector4D& _pos, const Vector4D& _dir, float _kc, float _kl, float _kq) :
			LightBase(LightType::kSpotLight1,_color), dir(_dir), kc(_kc), kl(_kl), kq(_kq) {
			pos = _pos;
		}
		//计算光源强度
		Color LightCalculate(const Vector4D &_VertexPos, const Vector4D &_normal) override
		{
			auto light_vec = pos - _VertexPos;
			float dot = _normal * light_vec;
			if (dot > 0) {
				float dist = light_vec.LengthDW();
				light_vec.Normalize();

				float atten = 1 / (kc + kl * sqrt(dist) + kq * dist);
				float cos_theta = (-light_vec)*(dir);
				auto _color = color * (cos_theta * atten);
				return _color;
			}
			else
			{
				return Color(0, 0, 0, 255);
			}
		}
	private:
		//光源方向
		Vector4D dir;
		//常数衰减因子
		float kc;
		//线性衰减因子
		float kl;
		//二次衰减因子
		float kq;
	};

	//复杂聚光灯
	class ComplexSpotLight : public LightBase
	{
	public:
		//复杂聚光灯构造函数
		ComplexSpotLight(const Color& _color, const Vector4D& _pos, const Vector4D& _dir, float _inAng,float _outAng ,float _kc, float _kl, float _kq, float _pf) :
			LightBase(LightType::kSpotLight2,_color), dir(_dir), spotInnerAngle(_inAng), spotOutterAngle(_outAng), kc(_kc), kl(_kl), kq(_kq), pf(_pf) {
			pos = _pos;
		}
		//计算光源强度
		Color LightCalculate(const Vector4D &_VertexPos, const Vector4D &_normal) override
		{
			//         	     I0spotlight * Clspotlight * MAX( (l . s), 0)^pf                     
		    // I(d)spotlight = __________________________________________      
			//               		 kc + kl*d + kq*d2        
			// Where d = |p - s|, and pf = power factor
			auto light_vec = _VertexPos - pos;
			float angle = AngleBetweenVectors(dir,light_vec);
			//位于半影外
			if (angle > spotOutterAngle) {
				return Color(0, 0, 0, 255);
			}
			//位于半影内
			else if (angle < spotOutterAngle && angle > spotInnerAngle) {
				float dist = light_vec.LengthDW();
				float dis_atten = 1 / (kc + kl * sqrt(dist) + kq * dist);
				float angle_atten = pow(cos(DegToRad(angle)) - cos(DegToRad(spotOutterAngle)), pf) / (cos(DegToRad(spotInnerAngle)) - cos(DegToRad(spotOutterAngle)));
				auto _color = color * (dis_atten * angle_atten);
				return _color;
			}
			//位于本影内
			else
			{
				float dist = light_vec.LengthDW();
				float atten = 1 / (kc + kl * sqrt(dist) + kq * dist);
				auto _color = color * atten;
				return _color;
			}
		}
	private:
		//光源方向
		Vector4D dir;
		//聚光灯内锥角
		float spotInnerAngle;
		//聚光灯外锥角(单位度)
		float spotOutterAngle;
		//常数衰减因子
		float kc;
		//线性衰减因子
		float kl;
		//二次衰减因子
		float kq;
		//聚光灯指数因子
		float pf;
	};

	//环境光
	class AmbientLight : public LightBase
	{
	public:
		//环境光构造函数
		AmbientLight(const Color& color) :LightBase(LightType::kAmbient,color) {}

	};
}
