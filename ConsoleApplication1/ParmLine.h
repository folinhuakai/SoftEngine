#pragma once
#include "Math.h"
#include "VectorXD.h"
//�ѿ�������
using Point2D = Vector2D;
using Point3D = Vector3D;

/*************2D������ֱ��***************/
class ParmLine2D {
public:
	ParmLine2D(const Point2D& p0,const Point2D& p1) :p0(p0), p1(p1) ,v(p1-p0){}
	//���������ֱ���ڲ���t����ֵ
	Point2D ComputeParmLine2D(FloatType t) {
		try {
			if (t > 1 || i < 0) {
				throw "i out of range!";
			}
			return (p0 + v * t);
		}
		catch (const char* msg) {
			std::cout << msg << std::endl;
		}
		
	}
	
	Point2D p0;
	Point2D p1;
	Vector2D v;
};

//����ֱ�ߵĽ��㣬t1,t2Ϊ���㴦��Ӧ��tֵ�����ؽ���
inline int IntersectionOfLine2D(ParmLine2D& l1, ParmLine2D& l2, FloatType& t1, FloatType& t2) {
	//�ж��Ƿ�ƽ�У��������������Ƿ�������Թ�ϵ,�ݲ������ص����
	FloatType det = (l1.v.x * l2.v.y - l1.v.y * l2.v.x);
	if (fabs(det) <= EPSILON_E5) {
		return PARM_LINE_NO_INTERSECT;
	}
	t1 = (l2.v.x*(l1.p0.y - l2.p0.y) - l2.v.y *(l1.p0.x - l2.p0.x));
	t2 = (l1.v.x*(l1.p0.y - l2.p0.y) - l1.v.y *(l1.p0.x - l2.p0.x));
	//�жϽ����Ƿ����߶���

	if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
		return PARM_LINE_INTERSECT_IN_SEGMENT;
	}
	else
	{
		return PARM_LINE_INTERSECT_OUT_SEGMENT;
	}
}


/*************3D������ֱ��***************/
class ParmLine3D {
public:
	ParmLine3D(const Point3D& p0,const Point3D& p1) :p0(p0), p1(p1), v(p1 - p0) {}
	Point3D ComputeParmLine3D(FloatType t) {
		try {
			if (t > 1 || i < 0) {
				throw "i out of range!";
			}
			return (p0 + v * t);
		}
		catch (const char* msg) {
			std::cout << msg << std::endl;
		}

	}
	Point3D p0;
	Point3D p1;
	Vector3D v;
};

/*************3Dƽ��******************/
class Plane3D {
public:
	Plane3D(const Point3D& p0, const Vector3D& n0, bool isNormolize = false) :p0(p0), n(n0) {
		if (isNormolize) {
			n.Normalize();
		}
	}
	// �������ƽ���λ�ù�ϵ��>0λ��ƽ�������<0���ռ䡢==0λ��ƽ����
	FloatType ComputePointInPlane3D(const Point3D& pt) {
		auto tmp = pt - p0;
		FloatType result = n*tmp;
		return result;
	}
	Point3D p0;
	Vector3D n;
};