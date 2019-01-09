#pragma once
#include "Math.h"
#include "VectorXD.h"
//笛卡尔坐标
using Point2D = Vector2D;
using Point3D = Vector3D;

/*************2D参数化直线***************/
class ParmLine2D {
	ParmLine2D(const Point2D& p0,const Point2D& p1) :p0(p0), p1(p1) ,v(p1-p0){}
	//计算参数化直线在参数t处的值
	Point2D ComputeParmLine2D(FloatType t) {
		return (p0 + v * t);
	}
	
	Point2D p0;
	Point2D p1;
	Vector2D v;
};

//两条直线的交点，t1,t2为交点处对应的t值，返回交点
inline Point2D IntersectionOfLine2D(ParmLine2D& l1, ParmLine2D& l2, FloatType& t1, FloatType& t2) {

}


/*************3D参数化直线***************/
class ParmLine3D {
	Point3D p0;
	Point3D p1;
	Vector3D v;
};