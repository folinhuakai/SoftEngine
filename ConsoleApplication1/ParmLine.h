#pragma once
#include "Math.h"
#include "VectorXD.h"
//�ѿ�������
using Point2D = Vector2D;
using Point3D = Vector3D;

/*************2D������ֱ��***************/
class ParmLine2D {
	ParmLine2D(const Point2D& p0,const Point2D& p1) :p0(p0), p1(p1) ,v(p1-p0){}
	//���������ֱ���ڲ���t����ֵ
	Point2D ComputeParmLine2D(FloatType t) {
		return (p0 + v * t);
	}
	
	Point2D p0;
	Point2D p1;
	Vector2D v;
};

//����ֱ�ߵĽ��㣬t1,t2Ϊ���㴦��Ӧ��tֵ�����ؽ���
inline Point2D IntersectionOfLine2D(ParmLine2D& l1, ParmLine2D& l2, FloatType& t1, FloatType& t2) {

}


/*************3D������ֱ��***************/
class ParmLine3D {
	Point3D p0;
	Point3D p1;
	Vector3D v;
};