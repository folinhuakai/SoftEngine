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
		LightBase ()= default;
		LightBase(const LightType _type,const Color& _color) :type(_type),color(_color) {}
		//��ȡ��������
		LightType GetType() { return type; }
		//��ȡ����ǿ��
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

	//ƽ�й�
	class DirectionLight : public LightBase
	{
	public:
		//ƽ�й⹹�캯��
		DirectionLight(const Color& _color, const Vector4D &_dir) :LightBase(LightType::kInfinite,_color), dir(_dir) {}
		//�������ǿ��
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
		//��Դ����
		Vector4D dir;//��λ����
	};

	//���Դ
	class PointLight : public LightBase
	{
	public:
		//���Դ���캯��
		PointLight(const Color& _color, const Vector4D& _pos, float _kc, float _kl, float _kq) 
			:LightBase(LightType::kPoint,_color), kc(_kc), kl(_kl), kq(_kq) {
			pos = _pos; }
		//�����Դǿ��
		Color LightCalculate(const Vector4D &_VertexPos, const Vector4D &_normal) override {
			auto light_vec = pos - _VertexPos;//���㵽��Դ����
			float dot = _normal *light_vec;
			if (dot > 0) {
				//��һ�����ӺͶ�������Ϊ0�򻯼���
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
		//����˥������
		float kc;
		//����˥������
		float kl;
		//����˥������
		float kq;
	};

	//�򵥾۹�ƣ�����Դ���ƣ�
	class SimpleSpotLight : public LightBase
	{
	public:
		//�򵥾۹�ƹ��캯��
		SimpleSpotLight(const Color& _color, const Vector4D& _pos, const Vector4D& _dir, float _kc, float _kl, float _kq) :
			LightBase(LightType::kSpotLight1,_color), dir(_dir), kc(_kc), kl(_kl), kq(_kq) {
			pos = _pos;
		}
		//�����Դǿ��
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
		//��Դ����
		Vector4D dir;
		//����˥������
		float kc;
		//����˥������
		float kl;
		//����˥������
		float kq;
	};

	//���Ӿ۹��
	class ComplexSpotLight : public LightBase
	{
	public:
		//���Ӿ۹�ƹ��캯��
		ComplexSpotLight(const Color& _color, const Vector4D& _pos, const Vector4D& _dir, float _inAng,float _outAng ,float _kc, float _kl, float _kq, float _pf) :
			LightBase(LightType::kSpotLight2,_color), dir(_dir), spotInnerAngle(_inAng), spotOutterAngle(_outAng), kc(_kc), kl(_kl), kq(_kq), pf(_pf) {
			pos = _pos;
		}
		//�����Դǿ��
		Color LightCalculate(const Vector4D &_VertexPos, const Vector4D &_normal) override
		{
			//         	     I0spotlight * Clspotlight * MAX( (l . s), 0)^pf                     
		    // I(d)spotlight = __________________________________________      
			//               		 kc + kl*d + kq*d2        
			// Where d = |p - s|, and pf = power factor
			auto light_vec = _VertexPos - pos;
			float angle = AngleBetweenVectors(dir,light_vec);
			//λ�ڰ�Ӱ��
			if (angle > spotOutterAngle) {
				return Color(0, 0, 0, 255);
			}
			//λ�ڰ�Ӱ��
			else if (angle < spotOutterAngle && angle > spotInnerAngle) {
				float dist = light_vec.LengthDW();
				float dis_atten = 1 / (kc + kl * sqrt(dist) + kq * dist);
				float angle_atten = pow(cos(DegToRad(angle)) - cos(DegToRad(spotOutterAngle)), pf) / (cos(DegToRad(spotInnerAngle)) - cos(DegToRad(spotOutterAngle)));
				auto _color = color * (dis_atten * angle_atten);
				return _color;
			}
			//λ�ڱ�Ӱ��
			else
			{
				float dist = light_vec.LengthDW();
				float atten = 1 / (kc + kl * sqrt(dist) + kq * dist);
				auto _color = color * atten;
				return _color;
			}
		}
	private:
		//��Դ����
		Vector4D dir;
		//�۹����׶��
		float spotInnerAngle;
		//�۹����׶��(��λ��)
		float spotOutterAngle;
		//����˥������
		float kc;
		//����˥������
		float kl;
		//����˥������
		float kq;
		//�۹��ָ������
		float pf;
	};

	//������
	class AmbientLight : public LightBase
	{
	public:
		//�����⹹�캯��
		AmbientLight(const Color& color) :LightBase(LightType::kAmbient,color) {}

	};
}
