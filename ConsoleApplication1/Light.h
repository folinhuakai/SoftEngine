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
	//��Դ����
	class LightBase
	{
	public:
		//���캯��
		LightBase(const Color& _color) :color(_color) {}
		//��ȡ��������
		LightType GetType() { return type; }
		//��ȡ����ǿ��
		virtual Color LightCalculate(const Vector4D &_pos, const Vector4D &_normal) {
			return color;
		}
	protected:
		LightType type;
		Color color;
	};

	//ƽ�й�
	class DirectionLight : public LightBase
	{
	public:
		//ƽ�й⹹�캯��
		DirectionLight(const Color& _color, const Vector4D &_dir) :LightBase(_color), dir(_dir) { type = LightType::kInfinite; }
		//�������ǿ��
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
		//��Դ����
		Vector4D dir;
	};

	//���Դ
	class PointLight : public LightBase
	{
	public:
		//���Դ���캯��
		PointLight(const Color& _color, const KZMath::KZVector4D<float>& _pos, float _kc, float _kl, float _kq) :LightBase(_color), pos_(pos), kc_(kc), kl_(kl), kq_(kq) { type_ = LightType::POINT; }
		//�����Դǿ��
		Color LightCalculate(const Vector4D &_pos, const Vector4D &_normal) override {

		}
		//��ȡ��Դ����
		KZMath::KZVector4D<float> GetLightVec(const KZMath::KZVector4D<float>& vertex_pos) override;
		////��ȡ��Դλ��
		//inline KZMath::KZVector4D<float> GetLightPos() { return pos_; };
		//��ȡ��Դλ��
		void GetLightPos(KZMath::KZVector4D<float>& light_pos) override { light_pos = pos_; return; };
	private:
		//��Դλ��
		KZMath::KZVector4D<float> pos_;
		//����˥������
		float kc_;
		//����˥������
		float kl_;
		//����˥������
		float kq_;
		//˥������
		float dist_;
	};

	//�򵥾۹��
	class SimpleSpotLight : public LightBase
	{
	public:
		//�򵥾۹�ƹ��캯��
		SimpleSpotLight(const Color& color, const KZMath::KZVector4D<float>& pos, const KZMath::KZVector4D<float>& dir, float kc, float kl, float kq, float pf) :
			LightBase(color), pos_(pos), dir_(dir), kc_(kc), kl_(kl), kq_(kq) {
			type_ = LightType::SSPOT;
		}
		//�����Դǿ��
		Color CalculateLightIntensity(const KZMath::KZVector4D<float>& vertex_pos = KZMath::KZVector4D<float>(), const KZMath::KZVector4D<float>& vertex_normal = KZMath::KZVector4D<float>()) override;
		//��ȡ��Դ����
		KZMath::KZVector4D<float> GetLightVec(const KZMath::KZVector4D<float>& vertex_pos) override;
		////��ȡ��Դλ��
		//inline KZMath::KZVector4D<float> GetLightPos() { return pos_; };
		//��ȡ��Դλ��
		void GetLightPos(KZMath::KZVector4D<float>& light_pos) override { light_pos = pos_; return; };
	private:
		//��Դλ��
		KZMath::KZVector4D<float> pos_;
		//��Դ����
		KZMath::KZVector4D<float> dir_;
		//����˥������
		float kc_;
		//����˥������
		float kl_;
		//����˥������
		float kq_;
		//�۹��ָ������Ĭ��Ϊ1
		//float pf_;
	};

	//���Ӿ۹��
	class ComplexSpotLight : public LightBase
	{
	public:
		//���Ӿ۹�ƹ��캯��
		ComplexSpotLight(const Color& color, const KZMath::KZVector4D<float>& pos, const KZMath::KZVector4D<float>& dir, float spot_inner_angle, float spot_outter_angle, float kc, float kl, float kq, float pf) :
			LightBase(color), pos_(pos), dir_(dir), spot_inner_angle_(spot_inner_angle), spot_outter_angle_(spot_outter_angle), kc_(kc), kl_(kl), kq_(kq), pf_(pf) {
			type_ = LightType::CSPOT;
		}
		//�����Դǿ��
		Color CalculateLightIntensity(const KZMath::KZVector4D<float>& vertex_pos = KZMath::KZVector4D<float>(), const KZMath::KZVector4D<float>& vertex_normal = KZMath::KZVector4D<float>()) override;
		//��ȡ��Դ����
		KZMath::KZVector4D<float> GetLightVec(const KZMath::KZVector4D<float>& vertex_pos) override;
		//��ȡ��Դλ��
		//inline KZMath::KZVector4D<float> GetLightPos() { return pos_; };
		//��ȡ��Դλ��
		void GetLightPos(KZMath::KZVector4D<float>& light_pos) override { light_pos = pos_; return; };
	private:
		//��Դλ��
		KZMath::KZVector4D<float> pos_;
		//��Դ����
		KZMath::KZVector4D<float> dir_;
		//�۹����׶��
		float spot_inner_angle_;
		//�۹����׶��
		float spot_outter_angle_;
		//����˥������
		float kc_;
		//����˥������
		float kl_;
		//����˥������
		float kq_;
		//�۹��ָ������
		float pf_;
	};

	//������
	class AmbientLight : public LightBase
	{
	public:
		//�����⹹�캯��
		AmbientLight(const Color& color) :LightBase(color) { type_ = LightType::AMBIENT; }
	private:

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
		Color Lightcalcul(const Vector4D &pos ,const Vector4D &normal) {
			unsigned int r_base, g_base, b_base,  // ��ʼ��ɫ
				r_sum, g_sum, b_sum,   // ȫ������
				shaded_color;          // ������ɫ

			float dp,     // ���
				dist,   // ����͹�Դ����
				i,      // ǿ��
				nl,     // ���߳���
				atten;  // ˥��������

					// is this light active
			if (state == LightState::kOff)
				return Color(0, 0, 0, 255);

			// what kind of light are we dealing with
			if (attr & LightType::kAmbient) {
				//������
				return c_ambient;
			} // end if
			else if (attr & LightType::kInfinite) {				
				dp = normal * dir;//������淨�߹�һ����dp��Ϊ����ֵ

				// only add light if dp > 0
				if (dp > 0)
				{
					//�����淨�߳���,���ڷ�������һ��
					nl = normal.Length();
					i = 128 * dp / nl;//��128.���⸡�������㣿
					r_sum += (c_diffuse.r * r_base * i) / (256 * 128);//����ɢ���
					g_sum += (c_diffuse.g * g_base * i) / (256 * 128);
					b_sum += (c_diffuse.b * b_base * i) / (256 * 128);
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
			// ȷ����ɫ�������
			if (r_sum > 255) r_sum = 255;
			if (g_sum > 255) g_sum = 255;
			if (b_sum > 255) b_sum = 255;

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
