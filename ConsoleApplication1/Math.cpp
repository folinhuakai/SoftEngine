#include "Math.h"
#include<math.h>
FloatType sinLook[361];
FloatType cosLook[361];

//Vector2D
Polar2D Vector2D::ConvertToPolar2D()const {
	Polar2D p;
	p.r = sqrtf(x*x + y * y);
	p.theta = atanf(y / x);
	return p;
}

//Vector3D
Cylindrical Vector3D::ConvertToCylindrical() {
	Cylindrical c;
	c.r = sqrtf(x*x + y * y);
	c.theta = atanf(y / x);
	c.z = z;
	return c;
}
Spherical3D Vector3D::ConvertToSpherical3D() {
	Spherical3D sp;
	sp.p = sqrtf(x*x + y * y + z * z);
	sp.theta = atanf(y / x);
	FloatType r = sp.p* FastSin(sp.theta);
	sp.phi = atanf(r / z);
	return sp;
}

//Vector4D

//Quat

//PointInt构造函数
PointInt::PointInt(int x, int y) :x(x), y(y) {
}
PointInt::PointInt(PointInt& v) : x(v.x), y(v.y) {
}
PointInt::~PointInt() {

}


//极坐标
Polar2D::Polar2D() :r(0.0f), theta(0.0f) {

}
Polar2D::Polar2D(FloatType r, FloatType theta) :r(r), theta(theta) {
}
Polar2D::Polar2D(Polar2D& v):r(v.r), theta(v.theta) {
}
void Polar2D::ConvertToPoint2D(Point2D& p){
	p.x = r * FastCos(theta);
	p.y = r * FastCos(theta);
}
Polar2D::~Polar2D() {

}

//柱面坐标
Cylindrical::Cylindrical() :r(0.0f), theta(0.0f),z(0.0f) {

}
Cylindrical::Cylindrical(FloatType r, FloatType theta,FloatType z) : r(r), theta(theta),z(z) {
}
Cylindrical::Cylindrical(Cylindrical& v) : r(v.r), theta(v.theta),z(v.z) {
}
void Cylindrical::ConvertToPoint3D(Point3D& p) {
	p.x = r * FastCos(theta);
	p.y = r * FastCos(theta);
	p.z = z;
}
Cylindrical::~Cylindrical() {

}

//球面坐标
Spherical3D::Spherical3D():p(0.0f),theta(0.0f),phi(0.0f){
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

/*
三角函数相关
*/
 void InitSinTable(FloatType table[361]) {
	 FloatType w = 0.0f;
	for(int i = 0;i < 361; ++i, ++w)
		table[i] = sin(DegToRad(w));
}
void InitCosTable(FloatType table[361]) {
	FloatType w = 0.0f;
	for (int i = 0;i < 361; ++i, ++w)
		table[i] = cos(DegToRad(w));
}
FloatType FastSin(FloatType theta) {
	//查表，线性插值
	theta = fmodf(theta, 360);
	if (theta < 0) {
		theta = +360.0f;
	}
	int thetaInt = (int)theta;
	FloatType thetaF = theta - thetaInt;
	return sinLook[thetaInt]
		+ thetaF * (sinLook[thetaInt + 1] - sinLook[thetaInt]);
}
FloatType FastCos(FloatType theta) {
	//查表，线性插值
	theta = fmodf(theta, 360);
	if (theta < 0) {
		theta = +360.0f;
	}
	int thetaInt = (int)theta;
	FloatType thetaF = theta - thetaInt;
	return cosLook[thetaInt]
		+ thetaF * (cosLook[thetaInt + 1] - cosLook[thetaInt]);
}
