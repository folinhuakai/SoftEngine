#pragma once
#include "Math.h"
#include "VectorXD.h"
using Point2D = Vector2D;
using Point3D = Vector3D;

/********2D极坐标************/
class Polar2D {
public:
	Polar2D();
	Polar2D(FloatType r, FloatType theta);
	Polar2D(Polar2D&);
	~Polar2D();
	void ConvertToPoint2D(Point2D&);
	FloatType r;
	FloatType theta;//弧度
};
/***********柱面坐标*********/
class Cylindrical {
public:
	Cylindrical();
	Cylindrical(FloatType r, FloatType theta, FloatType z);
	Cylindrical(Cylindrical&);
	~Cylindrical();
	void ConvertToPoint3D(Point3D&);
	FloatType r;
	FloatType theta;
	FloatType z;
};
/************球坐标*************/
class Spherical3D {
public:
	Spherical3D();
	Spherical3D(FloatType p, FloatType theta, FloatType phi);
	Spherical3D(Spherical3D&);
	~Spherical3D();
	void ConvertToPoint3D(Point3D&);
	FloatType p;
	FloatType theta;
	FloatType phi;
};


//极坐标
Polar2D::Polar2D() :r(0.0f), theta(0.0f) {

}
Polar2D::Polar2D(FloatType r, FloatType theta) : r(r), theta(theta) {
}
Polar2D::Polar2D(Polar2D& v) : r(v.r), theta(v.theta) {
}
void Polar2D::ConvertToPoint2D(Point2D& p) {
	p.x = r * FastCos(theta);
	p.y = r * FastCos(theta);
}
Polar2D::~Polar2D() {

}

//柱面坐标
Cylindrical::Cylindrical() :r(0.0f), theta(0.0f), z(0.0f) {

}
Cylindrical::Cylindrical(FloatType r, FloatType theta, FloatType z) : r(r), theta(theta), z(z) {
}
Cylindrical::Cylindrical(Cylindrical& v) : r(v.r), theta(v.theta), z(v.z) {
}
void Cylindrical::ConvertToPoint3D(Point3D& p) {
	p.x = r * FastCos(theta);
	p.y = r * FastCos(theta);
	p.z = z;
}
Cylindrical::~Cylindrical() {

}

//球面坐标
Spherical3D::Spherical3D() :p(0.0f), theta(0.0f), phi(0.0f) {
}
Spherical3D::Spherical3D(FloatType p, FloatType theta, FloatType phi)
	: p(p), theta(theta), phi(phi) {
}
Spherical3D::Spherical3D(Spherical3D& s)
	: p(s.p), theta(s.theta), phi(s.phi) {

}
Spherical3D::~Spherical3D() {

}
void Spherical3D::ConvertToPoint3D(Point3D& po) {
	FloatType r = p*FastSin(theta);

	po.x = r * FastCos(phi);
	po.y = r * FastSin(phi);
	po.z = r * FastCos(theta);
}

