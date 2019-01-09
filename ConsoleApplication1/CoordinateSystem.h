#pragma once
#include "Math.h"
#include "VectorXD.h"
//笛卡尔坐标
using Point2D = Vector2D;
using Point3D = Vector3D;

/********2D极坐标************/
class Polar2D {
public:
	Polar2D():r(0.0f),theta(0.0f){}
	Polar2D(FloatType r, FloatType theta) :r(r), theta(theta) {}
	Polar2D(Polar2D& p) :r(p.r), theta(p.theta) {}
	~Polar2D() {}
	Polar2D& operator=(const Polar2D& p) {
		if (this != &p) {
			r = p.r;
			theta = p.theta;
		}
		return *this;
	}
	void ConvertToPoint2D(Point2D& p) {
		p.x = r * cosf(theta);
		p.y = r * sinf(theta);
	}

	FloatType r;
	FloatType theta;//弧度
};
/***********柱面坐标*********/
class Cylindrical {
public:
	Cylindrical() :r(0.0f), theta(0.0f),z(0.0f){}
	Cylindrical(FloatType r, FloatType theta, FloatType z) :r(r), theta(theta), z(z) {}
	Cylindrical(Cylindrical& c) :r(c.r), theta(c.theta), z(c.z){}
	~Cylindrical(){}
	Cylindrical& operator=(const Cylindrical& p) {
		if (this != &p) {
			r = p.r;
			theta = p.theta;
			z = p.z;
		}
		return *this;
	}
	void ConvertToPoint3D(Point3D& p) {
		p.x = r * cosf(theta);
		p.y = r * sinf(theta);
		p.z = z;
	}
	FloatType r;
	FloatType theta;
	FloatType z;
};
/************球坐标*************/
class Spherical3D {
public:
	Spherical3D():p(0.0f),theta(0.0f),phi(0.0f){}
	Spherical3D(FloatType p, FloatType theta, FloatType phi) :p(p), theta(theta), phi(phi){}
	Spherical3D(Spherical3D& s) :p(s.p), theta(s.theta), phi(s.phi) {}
	~Spherical3D(){}
	Spherical3D& operator=(const Spherical3D& s) {
		if (this != &s) {
			p = s.p;
			theta = s.theta;
			phi =s.phi;
		}
		return *this;
	}
	void ConvertToPoint3D(Point3D& pt) {
		auto r = p * sinf(phi);
		pt.x = r * cosf(theta);
		pt.y = r * sinf(theta);
		pt.z = p * cosf(phi);
	}
	FloatType p;
	FloatType theta;
	FloatType phi;
};