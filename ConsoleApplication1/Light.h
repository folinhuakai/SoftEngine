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
		LightBase(const Color& _color) :color(_color) {}
		//获取光照类型
		LightType GetType() { return type; }
		//获取光照强度
		virtual Color LightCalculate(const Vector4D &_pos, const Vector4D &_normal) {
			return color;
		}
	protected:
		LightType type;
		Color color;
	};

	//平行光
	class DirectionLight : public LightBase
	{
	public:
		//平行光构造函数
		DirectionLight(const Color& _color, const Vector4D &_dir) :LightBase(_color), dir(_dir) { type = LightType::kInfinite; }
		//计算光照强度
		Color LightCalculate(const Vector4D &_pos, const Vector4D &_normal)  override {
			float dot = _normal * dir;
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
		Vector4D dir;
	};

	//点光源
	class PointLight : public LightBase
	{
	public:
		//点光源构造函数
		PointLight(const Color& _color, const KZMath::KZVector4D<float>& _pos, float _kc, float _kl, float _kq) :LightBase(_color), pos_(pos), kc_(kc), kl_(kl), kq_(kq) { type_ = LightType::POINT; }
		//计算光源强度
		Color LightCalculate(const Vector4D &_pos, const Vector4D &_normal) override {

		}
		//获取光源向量
		KZMath::KZVector4D<float> GetLightVec(const KZMath::KZVector4D<float>& vertex_pos) override;
		////获取光源位置
		//inline KZMath::KZVector4D<float> GetLightPos() { return pos_; };
		//获取光源位置
		void GetLightPos(KZMath::KZVector4D<float>& light_pos) override { light_pos = pos_; return; };
	private:
		//光源位置
		KZMath::KZVector4D<float> pos_;
		//常数衰减因子
		float kc_;
		//线性衰减因子
		float kl_;
		//二次衰减因子
		float kq_;
		//衰减距离
		float dist_;
	};

	//简单聚光灯
	class SimpleSpotLight : public LightBase
	{
	public:
		//简单聚光灯构造函数
		SimpleSpotLight(const Color& color, const KZMath::KZVector4D<float>& pos, const KZMath::KZVector4D<float>& dir, float kc, float kl, float kq, float pf) :
			LightBase(color), pos_(pos), dir_(dir), kc_(kc), kl_(kl), kq_(kq) {
			type_ = LightType::SSPOT;
		}
		//计算光源强度
		Color CalculateLightIntensity(const KZMath::KZVector4D<float>& vertex_pos = KZMath::KZVector4D<float>(), const KZMath::KZVector4D<float>& vertex_normal = KZMath::KZVector4D<float>()) override;
		//获取光源向量
		KZMath::KZVector4D<float> GetLightVec(const KZMath::KZVector4D<float>& vertex_pos) override;
		////获取光源位置
		//inline KZMath::KZVector4D<float> GetLightPos() { return pos_; };
		//获取光源位置
		void GetLightPos(KZMath::KZVector4D<float>& light_pos) override { light_pos = pos_; return; };
	private:
		//光源位置
		KZMath::KZVector4D<float> pos_;
		//光源方向
		KZMath::KZVector4D<float> dir_;
		//常数衰减因子
		float kc_;
		//线性衰减因子
		float kl_;
		//二次衰减因子
		float kq_;
		//聚光灯指数因子默认为1
		//float pf_;
	};

	//复杂聚光灯
	class ComplexSpotLight : public LightBase
	{
	public:
		//复杂聚光灯构造函数
		ComplexSpotLight(const Color& color, const KZMath::KZVector4D<float>& pos, const KZMath::KZVector4D<float>& dir, float spot_inner_angle, float spot_outter_angle, float kc, float kl, float kq, float pf) :
			LightBase(color), pos_(pos), dir_(dir), spot_inner_angle_(spot_inner_angle), spot_outter_angle_(spot_outter_angle), kc_(kc), kl_(kl), kq_(kq), pf_(pf) {
			type_ = LightType::CSPOT;
		}
		//计算光源强度
		Color CalculateLightIntensity(const KZMath::KZVector4D<float>& vertex_pos = KZMath::KZVector4D<float>(), const KZMath::KZVector4D<float>& vertex_normal = KZMath::KZVector4D<float>()) override;
		//获取光源向量
		KZMath::KZVector4D<float> GetLightVec(const KZMath::KZVector4D<float>& vertex_pos) override;
		//获取光源位置
		//inline KZMath::KZVector4D<float> GetLightPos() { return pos_; };
		//获取光源位置
		void GetLightPos(KZMath::KZVector4D<float>& light_pos) override { light_pos = pos_; return; };
	private:
		//光源位置
		KZMath::KZVector4D<float> pos_;
		//光源方向
		KZMath::KZVector4D<float> dir_;
		//聚光灯内锥角
		float spot_inner_angle_;
		//聚光灯外锥角
		float spot_outter_angle_;
		//常数衰减因子
		float kc_;
		//线性衰减因子
		float kl_;
		//二次衰减因子
		float kq_;
		//聚光灯指数因子
		float pf_;
	};

	//环境光
	class AmbientLight : public LightBase
	{
	public:
		//环境光构造函数
		AmbientLight(const Color& color) :LightBase(color) { type_ = LightType::AMBIENT; }
	private:

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

		//光照计算
		Color Lightcalcul(const Vector4D &pos ,const Vector4D &normal) {
			unsigned int r_base, g_base, b_base,  // 初始颜色
				r_sum, g_sum, b_sum,   // 全部光照
				shaded_color;          // 最终颜色

			float dp,     // 点积
				dist,   // 表面和光源距离
				i,      // 强度
				nl,     // 法线长度
				atten;  // 衰减计算结果

					// is this light active
			if (state == LightState::kOff)
				return Color(0, 0, 0, 255);

			// what kind of light are we dealing with
			if (attr & LightType::kAmbient) {
				//环境光
				return c_ambient;
			} // end if
			else if (attr & LightType::kInfinite) {				
				dp = normal * dir;//如果将面法线归一化后，dp即为余弦值

				// only add light if dp > 0
				if (dp > 0)
				{
					//计算面法线长度,用于法向量归一化
					nl = normal.Length();
					i = 128 * dp / nl;//乘128.避免浮点数计算？
					r_sum += (c_diffuse.r * r_base * i) / (256 * 128);//加上散射光
					g_sum += (c_diffuse.g * g_base * i) / (256 * 128);
					b_sum += (c_diffuse.b * b_base * i) / (256 * 128);
				} // end if

			}
			else if (lights[currLight].attr & LightType::kPoint) {
				// 点光源,除光强度随距离衰减外，其他与无穷远光源相似
				//              I0point * Clpoint
				//  I(d)point = ___________________
				//              kc +  kl*d + kq*d2        

				// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
				Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
				Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
				Vector4D n = u.Cross(v);

				//计算面法线长度,用于法向量归一化
				nl = n.Length();
				// 顶点到光源向量
				Vector4D l = lights[currLight].pos - vlistTransl[vindex_0];
				dist = l.Length();

				// and for the diffuse model
				dp = n * l;

				// only add light if dp > 0
				if (dp > 0)
				{
					atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);

					i = 128 * dp / (nl * dist * atten);

					r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);
					g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
					b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);
				} // end if

			} // end if point
			else if (lights[currLight].attr & LightType::kSpotLight1) {
				// 简单版聚光灯，与点光源相同
				Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
				Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
				Vector4D n = u.Cross(v);

				//计算面法线长度,用于法向量归一化
				nl = n.Length();
				// 顶点到光源向量
				Vector4D l = lights[currLight].pos - vlistTransl[vindex_0];
				dist = l.Length();

				// and for the diffuse model
				dp = n * l;

				// only add light if dp > 0
				if (dp > 0)
				{
					atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);

					i = 128 * dp / (nl * dist * atten);

					r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);
					g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
					b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);
				} // end if
			} // end if spotlight1
			else if (lights[currLight].attr & LightType::kSpotLight2) {
				//         	     I0spotlight * Clspotlight * MAX( (l . s), 0)^pf                     
				// I(d)spotlight = __________________________________________      
				//               		 kc + kl*d + kq*d2        
				// Where d = |p - s|, and pf = power factor
				Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
				Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
				Vector4D n = u.Cross(v);

				nl = n.Length();
				dp = n * lights[currLight].dir;

				// only add light if dp > 0
				if (dp > 0) {
					// 顶点到光源向量
					Vector4D s = vlistTransl[vindex_0] - lights[currLight].pos;
					dist = s.Length();

					// 余弦值
					float dpsl = s * lights[currLight].dir / dist;

					// proceed only if term is positive
					if (dpsl > 0)
					{
						// compute attenuation
						atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);
						float dpsl_exp = dpsl;

						// exponentiate for positive integral powers
						for (int e_index = 1; e_index < (int)lights[currLight].pf; e_index++)
							dpsl_exp *= dpsl;

						// now dpsl_exp holds (dpsl)^pf power which is of course (s . l)^pf 

						i = 128 * dp * dpsl_exp / (nl * atten);

						r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);

					}

				}

			}
			// 确保颜色不会溢出
			if (r_sum > 255) r_sum = 255;
			if (g_sum > 255) g_sum = 255;
			if (b_sum > 255) b_sum = 255;

		} // end 

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
