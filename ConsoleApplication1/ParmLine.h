#pragma once
#include "Math.h"
#include "VectorXD.h"
//笛卡尔坐标
using Point2D = Vector2D;
using Point3D = Vector3D;

/*************2D参数化直线***************/
class ParmLine2D {
public:
	ParmLine2D(const Point2D& p0,const Point2D& p1) :p0(p0), p1(p1) ,v(p1-p0){}
	//计算参数化直线在参数t处的值
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

//两条直线的交点，t1,t2为交点处对应的t值，返回交点
inline FloatType IntersectionOfLine2D(ParmLine2D& l1, ParmLine2D& l2, FloatType& t1, FloatType& t2) {
	//判断是否平行，两个方向向量是否存在线性关系,暂不考虑重叠情况
	FloatType det = (l1.v.x * l2.v.y - l1.v.y * l2.v.x);
	if (det <= EPSILON_E5) {
		return PARM_LINE_NO_INTERSECT;
	}
	t1 = (l2.v.x*(l1.p0.y - l2.p0.y) - l2.v.y *(l1.p0.x - l2.p0.x));
	t2 = (l1.v.x*(l1.p0.y - l2.p0.y) - l1.v.y *(l1.p0.x - l2.p0.x));
	//判断交点是否在线段上

	if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
		return PARM_LINE_NO_INTERSECT_IN_SEGMENT;
	}
	else
	{
		return PARM_LINE_INTERSECT_OUT_SEGMENT;
	}
}


/*************3D参数化直线***************/
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

/*************3D平面******************/
class Plane3D {
	Point3D p0;
	Vector2D n;
};