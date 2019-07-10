#pragma once
#include"VectorXD.h"
#include "Color.h"
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

		//���ռ���
		bool LightInWorld(Camera &cam,
			const std::vector<Light> lights) {
			//���ݹ�Դ�б�����������ִ�й��ռ��㣬֧�̶ֹ���ɫ�ͺ㶨��ɫ
			unsigned int r_base, g_base, b_base,  // ��ʼ��ɫ
				r_sum, g_sum, b_sum,   // ȫ������
				shaded_color;          // ������ɫ

			float dp,     // ���
				dist,   // ����͹�Դ����
				i,      // ǿ��
				nl,     // ���߳���
				atten;  // ˥��������

		  // test if the object is culled
			if (!(state & ObjectState::kActiveS) ||
				(state & ObjectState::kCull) ||
				!(state & ObjectState::kVisible))
				return false;

			// process each poly in mesh
			for (int poly = 0; poly < numPolygons; poly++) {
				// acquire polygon
				auto currPoly = plist[poly];

				// �ж϶������Ч��
				if (!(currPoly.state & PloygonStates::kActive) ||
					(currPoly.state & PloygonStates::kBackface) ||
					(currPoly.state & PloygonStates::kClipped))
					continue; // move onto next poly

				 // ��ö����±꣨�����԰�����
				int vindex_0 = currPoly.vert[0];
				int vindex_1 = currPoly.vert[1];
				int vindex_2 = currPoly.vert[2];

				// test the lighting mode of the polygon (use flat for flat, gouraud))
				if (currPoly.attr & PloygonShadeMode::kFlat || currPoly.attr & PloygonShadeMode::kGouraud) {//�㶨��ɫ��Gouraud
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
							//������
							r_sum += ((lights[currLight].c_ambient.r * r_base) / 256);
							g_sum += ((lights[currLight].c_ambient.g * g_base) / 256);
							b_sum += ((lights[currLight].c_ambient.b * b_base) / 256);
						} // end if
						else if (lights[currLight].attr & LightType::kInfinite) {
							//����Զ��Դ,������淨�ߺ͹�Դ����
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//�����淨�߳���,���ڷ�������һ��
							nl = n.Length();

							dp = n * lights[currLight].dir;//������淨�߹�һ����dp��Ϊ����ֵ

							// only add light if dp > 0
							if (dp > 0)
							{
								i = 128 * dp / nl;//��128.���⸡�������㣿
								r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);//����ɢ���
								g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
								b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);
							} // end if

						}
						else if (lights[currLight].attr & LightType::kPoint) {
							// ���Դ,����ǿ�������˥���⣬����������Զ��Դ����
							//              I0point * Clpoint
							//  I(d)point = ___________________
							//              kc +  kl*d + kq*d2        

							// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//�����淨�߳���,���ڷ�������һ��
							nl = n.Length();
							// ���㵽��Դ����
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
							// �򵥰�۹�ƣ�����Դ��ͬ
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//�����淨�߳���,���ڷ�������һ��
							nl = n.Length();
							// ���㵽��Դ����
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
								// ���㵽��Դ����
								Vector4D s = vlistTransl[vindex_0] - lights[currLight].pos;
								dist = s.Length();

								// ����ֵ
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

				// ȷ����ɫ�������
					if (r_sum > 255) r_sum = 255;
					if (g_sum > 255) g_sum = 255;
					if (b_sum > 255) b_sum = 255;

					// write the color
					shaded_color = RGBA32BIT(r_sum, g_sum, b_sum, 255);
					currPoly.tColor = shaded_color;

				} // end if
				else {
					// �̶���ɫ
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
