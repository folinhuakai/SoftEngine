#pragma once
#include "Math.h"
#include "VectorXD.h"
namespace maki {
	//�ѿ�������
	using Point2D = Vector2D;
	using Point3D = Vector3D;

	/*************2D������ֱ��***************/
	class ParmLine2D {
	public:
		ParmLine2D(const Point2D& p0, const Point2D& p1) :p0(p0), p1(p1), v(p1 - p0) {}
		//���������ֱ���ڲ���t����ֵ
		Point2D ComputeParmLine2D(float t) {
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
	inline int IntersectionOfLine2D(const ParmLine2D& l1, const ParmLine2D& l2, float& t1, float& t2) {
		//�ж��Ƿ�ƽ�У��������������Ƿ�������Թ�ϵ,�ݲ������ص����
		float det = (l1.v.x * l2.v.y - l1.v.y * l2.v.x);
		if (fabs(det) <= EPSILON_E5) {
			return PARM_LINE_NO_INTERSECT;
		}
		t1 = (l2.v.x*(l1.p0.y - l2.p0.y) - l2.v.y *(l1.p0.x - l2.p0.x)) / det;
		t2 = (l1.v.x*(l1.p0.y - l2.p0.y) - l1.v.y *(l1.p0.x - l2.p0.x)) / det;
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
		ParmLine3D(const Point3D& p0, const Point3D& p1) :p0(p0), p1(p1), v(p1 - p0) {}
		Point3D ComputeParmLine3D(float t) {
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
		Plane3D() = default;
		// �������ƽ���λ�ù�ϵ��>0λ��ƽ�������<0���ռ䡢==0λ��ƽ����
		float ComputePointInPlane3D(const Point3D& pt) const {
			auto tmp = pt - p0;
			float result = n * tmp;
			return result;
		}
		Point3D p0;
		Vector3D n;
	};

	//�߶���ƽ��Ľ���
	inline int IntersectionOfLineAndPlane3D(const ParmLine3D& line, const Plane3D& plane, float& t, Point3D& pt) {
		//�ж��Ƿ�ƽ�У��������������Ƿ�������Թ�ϵ,�ݲ������ص����
		float dot = (plane.n * line.v);
		if (fabs(dot) <= EPSILON_E5) {
			if (fabs(plane.ComputePointInPlane3D(line.p0)) <= EPSILON_E5)
				return PARM_LINE_EVERYWHERE;
			else
				return PARM_LINE_NO_INTERSECT;
		}
		t = -(plane.n.x*line.p0.x +
			plane.n.y*line.p0.y +
			plane.n.z*line.p0.z -
			plane.n.x*plane.p0.x -
			plane.n.y*plane.p0.y -
			plane.n.z*plane.p0.z) / dot;

		pt.x = line.p0.x + line.v.x*t;
		pt.y = line.p0.y + line.v.y*t;
		pt.z = line.p0.z + line.v.z*t;
		//�жϽ����Ƿ����߶���
		if (t >= 0.0 && t <= 1.0) {
			return PARM_LINE_INTERSECT_IN_SEGMENT;
		}
		else
		{
			return PARM_LINE_INTERSECT_OUT_SEGMENT;
		}
	}
}