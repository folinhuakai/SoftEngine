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

		//光照计算
		bool LightInWorld(Camera &cam,
			const std::vector<Light> lights) {
			//根据光源列表和相机对物体执行光照计算，支持固定着色和恒定着色
			unsigned int r_base, g_base, b_base,  // 初始颜色
				r_sum, g_sum, b_sum,   // 全部光照
				shaded_color;          // 最终颜色

			float dp,     // 点积
				dist,   // 表面和光源距离
				i,      // 强度
				nl,     // 法线长度
				atten;  // 衰减计算结果

		  // test if the object is culled
			if (!(state & ObjectState::kActiveS) ||
				(state & ObjectState::kCull) ||
				!(state & ObjectState::kVisible))
				return false;

			// process each poly in mesh
			for (int poly = 0; poly < numPolygons; poly++) {
				// acquire polygon
				auto currPoly = plist[poly];

				// 判断多边形有效性
				if (!(currPoly.state & PloygonStates::kActive) ||
					(currPoly.state & PloygonStates::kBackface) ||
					(currPoly.state & PloygonStates::kClipped))
					continue; // move onto next poly

				 // 获得顶点下标（不是自包含）
				int vindex_0 = currPoly.vert[0];
				int vindex_1 = currPoly.vert[1];
				int vindex_2 = currPoly.vert[2];

				// test the lighting mode of the polygon (use flat for flat, gouraud))
				if (currPoly.attr & PloygonShadeMode::kFlat || currPoly.attr & PloygonShadeMode::kGouraud) {//恒定着色、Gouraud
					RGB_FROM32BIT(currPoly.color, &r_base, &g_base, &b_base);
					// initialize color sum
					r_sum = 0;
					g_sum = 0;
					b_sum = 0;

					// loop thru lights
					for (int currLight = 0; currLight < lights.size(); ++currLight)
					{
						// is this light active
						if (lights[currLight].state == LightState::kOff)
							continue;

						// what kind of light are we dealing with
						if (lights[currLight].attr & LightType::kAmbient) {
							//环境光
							r_sum += ((lights[currLight].c_ambient.r * r_base) / 256);
							g_sum += ((lights[currLight].c_ambient.g * g_base) / 256);
							b_sum += ((lights[currLight].c_ambient.b * b_base) / 256);
						} // end if
						else if (lights[currLight].attr & LightType::kInfinite) {
							//无穷远光源,需计算面法线和光源方向
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//计算面法线长度,用于法向量归一化
							nl = n.Length();

							dp = n * lights[currLight].dir;//如果将面法线归一化后，dp即为余弦值

							// only add light if dp > 0
							if (dp > 0)
							{
								i = 128 * dp / nl;//乘128.避免浮点数计算？
								r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);//加上散射光
								g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
								b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);
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

					} // end for light

				// 确保颜色不会溢出
					if (r_sum > 255) r_sum = 255;
					if (g_sum > 255) g_sum = 255;
					if (b_sum > 255) b_sum = 255;

					// write the color
					shaded_color = RGBA32BIT(r_sum, g_sum, b_sum, 255);
					currPoly.tColor = shaded_color;

				} // end if
				else {
					// 固定着色
					currPoly.tColor = currPoly.color;
				} // end if

			} // end for poly

		// return success
			return true;

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
