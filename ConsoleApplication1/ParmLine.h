#pragma once
#include "Math.h"
#include "VectorXD.h"
namespace maki {
	//笛卡尔坐标
	using Point2D = Vector2D;
	using Point3D = Vector3D;

	/*************2D参数化直线***************/
	class ParmLine2D {
	public:
		ParmLine2D(const Point2D& p0, const Point2D& p1) :p0(p0), p1(p1), v(p1 - p0) {}
		//计算参数化直线在参数t处的值
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

	//两条直线的交点，t1,t2为交点处对应的t值，返回交点
	inline int IntersectionOfLine2D(const ParmLine2D& l1, const ParmLine2D& l2, float& t1, float& t2) {
		//判断是否平行，两个方向向量是否存在线性关系,暂不考虑重叠情况
		float det = (l1.v.x * l2.v.y - l1.v.y * l2.v.x);
		if (fabs(det) <= EPSILON_E5) {
			return PARM_LINE_NO_INTERSECT;
		}
		t1 = (l2.v.x*(l1.p0.y - l2.p0.y) - l2.v.y *(l1.p0.x - l2.p0.x)) / det;
		t2 = (l1.v.x*(l1.p0.y - l2.p0.y) - l1.v.y *(l1.p0.x - l2.p0.x)) / det;
		//判断交点是否在线段上

		if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
			return PARM_LINE_INTERSECT_IN_SEGMENT;
		}
		else
		{
			return PARM_LINE_INTERSECT_OUT_SEGMENT;
		}
	}


	/*************3D参数化直线***************/
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

	/*************3D平面******************/
	class Plane3D {
	public:
		Plane3D(const Point3D& p0, const Vector3D& n0, bool isNormolize = false) :p0(p0), n(n0) {
			if (isNormolize) {
				n.Normalize();
			}
		}
		Plane3D() = default;
		// 计算点与平面的位置关系：>0位于平面的正、<0负空间、==0位于平面上
		float ComputePointInPlane3D(const Point3D& pt) const {
			auto tmp = pt - p0;
			float result = n * tmp;
			return result;
		}
		Point3D p0;
		Vector3D n;
	};

	//线段与平面的交点
	inline int IntersectionOfLineAndPlane3D(const ParmLine3D& line, const Plane3D& plane, float& t, Point3D& pt) {
		//判断是否平行，两个方向向量是否存在线性关系,暂不考虑重叠情况
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
		//判断交点是否在线段上
		if (t >= 0.0 && t <= 1.0) {
			return PARM_LINE_INTERSECT_IN_SEGMENT;
		}
		else
		{
			return PARM_LINE_INTERSECT_OUT_SEGMENT;
		}
	}
}