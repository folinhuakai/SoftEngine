#pragma once
#include "Math.h"

/******************2维点或向量***************/
class Vector2D {
public:
	//构造函数
	Vector2D() :x(0.0f), y(0.0f) {}
	Vector2D(FloatType x, FloatType y) : x(x), y(y) {}
	Vector2D(const Vector2D& v) : x(v.x), y(v.y) {}
	~Vector2D() {};

	FloatType Length()const {//向量长度
		return sqrtf(x*x + y * y);
	}
	bool Normalize() {//归一化
		auto length = Length();

		if (length < EPSILON_E5)
			return false;
		auto inv = 1 / length;
		x *= inv;
		y *= inv;
		return true;
	}
	void Vector2DZero() {
		x = 0.0f;
		y = 0.0f;
	}
	Vector2D& operator =(const Vector2D &v) {
		if (this != &v) {
			x = v.x;
			y = v.y;
		}
		return *this;
	}
	FloatType x, y;
};

inline const Vector2D operator +(const Vector2D &a, const Vector2D &b) {
	Vector2D sum(a.x + b.x, a.y + b.y);
	return sum;
}
inline const Vector2D operator -(const Vector2D &a, const Vector2D &b) {
	Vector2D sub(a.x - b.x, a.y - b.y);
	return sub;
}
inline FloatType operator *(const Vector2D &a, const Vector2D &b) {
	return(b.x*a.x + b.y*a.y);
}
inline Vector2D operator *(const FloatType k, const Vector2D &v) {
	Vector2D scale(v.x*k, v.y *k);
	return scale;
}
inline Vector2D operator *(const Vector2D &v, const FloatType k) {
	Vector2D scale(v.x*k, v.y *k);
	return scale;
}
//a,b向量夹角余弦值
inline FloatType CosTh(const Vector2D& a, const Vector2D& b) {
	auto tmp = a * b / (a.Length() *b.Length());
	return tmp;
}
//判断vector2D是否相等
inline bool operator ==(const Vector2D& a, const Vector2D& b) {
	if ((fabs(a.x - b.x) < EPSILON_E5) && (fabs(a.y - b.y) < EPSILON_E5)) {
		return true;
	}
	else {
		return false;
	}
}

inline std::ostream & operator <<(std::ostream &out, const Vector2D &vec) {
	out << "x = " << vec.x << " y = " << vec.y << std::endl;
	return out;
}

/******************3维点或向量***************/
class Vector3D {
public:
	Vector3D() :x(0), y(0), z(0) {}
	Vector3D(FloatType x, FloatType y, FloatType z) :x(x), y(y), z(z) {}
	Vector3D(const Vector3D& v) : x(v.x), y(v.y), z(v.z) {}
	void Vector3DZero() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	Vector3D& operator =(const Vector3D &v) {
		if (this != &v) {
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}
	~Vector3D() {}
	Vector3D Cross(const Vector3D &b)const {//叉乘
		Vector3D cross(y*b.z - b.y*z, b.x*z - x * b.z, x*b.y - b.x*y);
		return cross;
	}
	FloatType Length()const {//向量长度
		return sqrtf(x*x + y * y + z * z);
	}
	void Normalize() {//归一化
		auto length = Length();

		if (length < EPSILON_E5)
			return;
		auto inv = 1 / length;
		x *= inv;
		y *= inv;
		z *= inv;
	}
	FloatType x, y, z;
};


inline Vector3D operator +(const Vector3D &a, const Vector3D &b) {
	Vector3D sum(a.x + b.x, a.y + b.y, a.z + b.z);
	return sum;
}
inline Vector3D operator -(const Vector3D &a, const Vector3D &b) {
	Vector3D sub(a.x - b.x, a.y - b.y, a.z - b.z);
	return sub;
}

inline FloatType operator *(const Vector3D &a, const Vector3D &b) {
	return(b.x*a.x + b.y*a.y + b.z*a.z);
}
inline Vector3D operator *(const FloatType k, const Vector3D &a) {
	Vector3D scale(a.x*k, a.y *k, a.z*k);
	return scale;
}
inline Vector3D operator *(const Vector3D &a, const FloatType k) {
	return (k*a);
}
//判断vector3D是否相等
inline bool operator ==(const Vector3D& a, const Vector3D& b) {
	if ((fabs(a.x - b.x) < EPSILON_E5) && (fabs(a.y - b.y) < EPSILON_E5) && (fabs(a.z - b.z) < EPSILON_E5)) {
		return true;
	}
	else {
		return false;
	}
}

inline FloatType CosTh(const Vector3D& a, const Vector3D& b) {
	auto tmp = a * b / (a.Length() *b.Length());
	return tmp;
}
inline std::ostream & operator <<(std::ostream &out, const Vector3D &vec) {
	out << "x = " << vec.x << " y = " << vec.y << " z = " << vec.z << std::endl;
	return out;
}


/******************4维点或向量，用于齐次坐标***************/
class Vector4D {
public:
	Vector4D() :x(0), y(0), z(0), w(1) {}
	Vector4D(FloatType x, FloatType y, FloatType z, FloatType w) :x(x), y(y), z(z), w(w) {}
	Vector4D(const Vector4D& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	void Vector4DZero() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}
	Vector4D& operator =(const Vector4D &v) {
		if (this != &v) {
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}
		return *this;
	}
	~Vector4D() {}
	Vector4D Cross(const Vector4D &b)const {//叉乘
		Vector4D cross(y*b.z - b.y*z, b.x*z - x * b.z, x*b.y - b.x*y, 1);
		return cross;
	}
	FloatType Length()const {//向量长度
		return sqrtf(x*x + y * y + z * z);
	}
	void Normalize() {//归一化
		auto length = Length();

		if (length < EPSILON_E5)
			return;
		auto inv = 1 / length;
		x *= inv;
		y *= inv;
		z *= inv;
		w = 1;
	}
	FloatType x, y, z, w;
};

inline Vector4D operator +(const Vector4D &a, const Vector4D &b) {
	Vector4D sum(a.x + b.x, a.y + b.y, a.z + b.z, 1);
	return sum;
}
inline Vector4D operator -(const Vector4D &a, const Vector4D &b) {
	Vector4D sub(a.x - b.x, a.y - b.y, a.z - b.z, 1);
	return sub;
}

inline FloatType operator *(const Vector4D &a, const Vector4D &b) {
	return(b.x*a.x + b.y*a.y + b.z*a.z);
}
inline Vector4D operator *(const FloatType k, const Vector4D &a) {
	Vector4D scale(a.x*k, a.y *k, a.z*k, 1);
	return scale;
}
inline Vector4D operator *(const Vector4D &a, const FloatType k) {
	return (k*a);
}
//判断vector4D是否相等
inline bool operator ==(const Vector4D& a, const Vector4D& b) {
	if ((fabs(a.x - b.x) < EPSILON_E5) && (fabs(a.y - b.y) < EPSILON_E5) 
		&& (fabs(a.z - b.z) < EPSILON_E5) && (fabs(a.w - b.w) < EPSILON_E5)) {
		return true;
	}
	else {
		return false;
	}
}

inline FloatType CosTh(const Vector4D& a, const Vector4D& b) {
	auto tmp = a * b / (a.Length() *b.Length());
	return tmp;
}
inline std::ostream & operator <<(std::ostream &out, const Vector4D &vec) {
	out << "x = " << vec.x << " y = " << vec.y << " z = " << vec.z << " w = " << vec.w << std::endl;
	return out;
}

/******************四元数***************/
class Quat {
public:
	Quat() :w(0), x(0), y(0), z(0) {}
	Quat(FloatType w, FloatType x, FloatType y, FloatType z) :w(w), x(x), y(y), z(z) {}
	Quat(const Quat& v) :w(v.w), x(v.x), y(v.y), z(v.z) {}
	Quat(const Vector3D& v, FloatType theta) {
		//方向向量v必须是单位向量，角度单位为弧度
		FloatType theta_div = 0.5f * theta;
		FloatType theta_sinf = sinf(theta_div);
		w = cosf(theta_div);
		x = theta_sinf * v.x;
		y = theta_sinf * v.y;
		z = theta_sinf * v.z;
	}
	Quat(const Vector4D& v, FloatType theta) {
		//方向向量v必须是单位向量，角度单位为弧度
		FloatType theta_div = 0.5f * theta;
		FloatType theta_sinf = sinf(theta_div);
		w = cosf(theta_div);
		x = theta_sinf * v.x;
		y = theta_sinf * v.y;
		z = theta_sinf * v.z;
	}
	Quat(FloatType theta_z, FloatType theta_y, FloatType theta_x) {
		//根据绕x、y、z旋转的角度，创建对应的四元数
		FloatType cos_z = cosf(0.5f *theta_z);
		FloatType cos_y = cosf(0.5f * theta_y);
		FloatType cos_x = cosf(0.5f * theta_x);

		FloatType sin_z = sinf(0.5f * theta_z);
		FloatType sin_x = sinf(0.5f * theta_x);
		FloatType sin_y = sinf(0.5f * theta_y);

		w = cos_z * cos_y*cos_x + sin_z * sin_y*sin_x;
		x = cos_z * cos_y*sin_x - sin_z * sin_y*cos_x;
		y = cos_z * sin_y*cos_x + sin_z * cos_y*sin_x;
		z = sin_z * cos_y*cos_x - cos_z * sin_y*sin_x;
	}
	~Quat() {}
	Quat& operator =(const Quat &v) {
		if (this != &v) {
			w = v.w;
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}
	void QuatZero() {
		w = 0.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	void ConvToVector3DAndTheta(Vector3D &v, FloatType &theta) {
		//将单位四元数转换成单位向量和角度
		theta = acosf(w);
		FloatType sinf_theta_inv = 1.0f / sinf(theta);
		v.x = x * sinf_theta_inv;
		v.y = y * sinf_theta_inv;
		v.z = z * sinf_theta_inv;

		theta *= 2;
	}
	Quat Conjufate() {
		//共轭
		Quat q;
		q.w = w;
		q.x = -x;
		q.y = -y;
		q.z = -z;
		return q;
	}
	FloatType Length() {
		return sqrt(w*w + x * x + y * y + z * z);
	}
	FloatType Length2() {
		return (w*w + x * x + y * y + z * z);
	}
	void Normalize() {//归一化
		auto length = Length();

		if (length < EPSILON_E5)
			return;
		auto inv = 1 / length;
		w *= inv;
		x *= inv;
		y *= inv;
		z *= inv;
	}
	Quat UnitInverse() {
		//单位四元数的逆与共轭相同
		return Conjufate();
	}
	Quat Inverse() {
		//求逆
		auto inv = 1.0f / Length2();

		Quat q;
		q.w = w * inv;
		q.x = -x * inv;
		q.y = -y * inv;
		q.z = -z * inv;
		return q;
	}
	FloatType w, x, y, z;
};

inline Quat operator +(const Quat &a, const Quat &b) {
	Quat sum(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);
	return sum;
}
inline Quat operator -(const Quat &a, const Quat &b) {
	Quat sub(a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z);
	return sub;
}

inline Quat operator *(const Quat &a, const Quat &b) {//a*b!=b*a 除非a,b为乘法单位数
	FloatType ret0 = (a.z - a.y) *(b.y - b.z);
	FloatType ret1 = (a.w + a.x) *(b.w + b.x);
	FloatType ret2 = (a.w - a.x) *(b.y + b.z);
	FloatType ret3 = (a.y + a.z) *(b.w - b.x);
	FloatType ret4 = (a.z - a.x) *(b.x - b.y);
	FloatType ret5 = (a.z + a.x) *(b.x + b.y);
	FloatType ret6 = (a.w + a.y) *(b.w - b.z);
	FloatType ret7 = (a.w - a.y) *(b.w + b.z);
	FloatType ret8 = ret5 + ret6 + ret7;
	FloatType ret9 = 0.5f*(ret4 + ret8);

	Quat mul(ret0 + ret9 - ret5, ret1 + ret9 - ret8, ret2 + ret9 - ret7, ret3 + ret9 - ret6);
	return mul;

}
inline Quat operator *(const FloatType k, const Quat &a) {
	Quat scale(a.w*k, a.x*k, a.y *k, a.z*k);
	return scale;
}
inline Quat operator *(const Quat &a, const FloatType k) {
	return (k*a);
}
//判断Quat是否相等
inline bool operator ==(const Quat& a, const Quat& b) {
	if ((fabs(a.x - b.x) < EPSILON_E5) && (fabs(a.y - b.y) < EPSILON_E5)
		&& (fabs(a.z - b.z) < EPSILON_E5) && (fabs(a.w - b.w) < EPSILON_E5)) {
		return true;
	}
	else {
		return false;
	}
}
inline std::ostream & operator <<(std::ostream &out, const Quat &vec) {
	out << " w = " << vec.w << " x = " << vec.x << " y = " << vec.y << " z = " << vec.z << std::endl;
	return out;
}


using Point2D = Vector2D;
using Point3D = Vector3D;
/****************极坐标******************/
class Polar2D {
public:
	Polar2D() :r(0.0f), theta(0.0f) {}
	Polar2D(FloatType r, FloatType theta) :r(r), theta(theta) {}
	Polar2D(const Polar2D& p) :r(p.r), theta(p.theta) {}
	~Polar2D() {}
	Polar2D& operator=(const Polar2D& p) {
		if (this != &p) {
			r = p.r;
			theta = p.theta;
		}
		return *this;
	}

	FloatType r;
	FloatType theta;//弧度
};
inline bool operator ==(const Polar2D& a, const Polar2D& b) {
	if ((fabs(a.r - b.r) < EPSILON_E5) && (fabs(a.theta - b.theta) < EPSILON_E5)) {
		return true;
	}
	else {
		return false;
	}
}

/***********柱面坐标*********/
class Cylindrical {
public:
	Cylindrical() :r(0.0f), theta(0.0f), z(0.0f) {}
	Cylindrical(FloatType r, FloatType theta, FloatType z) :r(r), theta(theta), z(z) {}
	Cylindrical(const Cylindrical& c) :r(c.r), theta(c.theta), z(c.z) {}
	~Cylindrical() {}
	Cylindrical& operator=(const Cylindrical& p) {
		if (this != &p) {
			r = p.r;
			theta = p.theta;
			z = p.z;
		}
		return *this;
	}

	FloatType r;
	FloatType theta;
	FloatType z;
};

inline bool operator ==(const Cylindrical& a, const Cylindrical& b) {
	if ((fabs(a.r - b.r) < EPSILON_E5) && (fabs(a.theta - b.theta) < EPSILON_E5) && (fabs(a.z - b.z) < EPSILON_E5)) {
		return true;
	}
	else {
		return false;
	}
}

/************球坐标*************/
class Spherical3D {
public:
	Spherical3D() :p(0.0f), theta(0.0f), phi(0.0f) {}
	Spherical3D(FloatType p, FloatType theta, FloatType phi) :p(p), theta(theta), phi(phi) {}
	Spherical3D(const Spherical3D& s) :p(s.p), theta(s.theta), phi(s.phi) {}
	~Spherical3D() {}
	Spherical3D& operator=(const Spherical3D& s) {
		if (this != &s) {
			p = s.p;
			theta = s.theta;
			phi = s.phi;
		}
		return *this;
	}
	FloatType p;
	FloatType theta;
	FloatType phi;
};

inline bool operator ==(const Spherical3D& a, const Spherical3D& b) {
	if ((fabs(a.p - b.p) < EPSILON_E5) && (fabs(a.theta - b.theta) < EPSILON_E5) && (fabs(a.phi - b.phi) < EPSILON_E5)) {
		return true;
	}
	else {
		return false;
	}
}


//笛卡尔坐标转化成极坐标
inline Polar2D ConvertToPolar2D(const Point2D &v) {
	Polar2D p;
	p.r = sqrtf(v.x*v.x + v.y * v.y);
	p.theta = atanf(v.y / v.x);
	return p;
}
//极坐标转化成笛卡尔坐标
inline Point2D ConvertToPoint2D(const Polar2D &q) {
	Point2D v;
	v.x = q.r * cosf(q.theta);
	v.y = q.r * sinf(q.theta);
	return v;
}

//3维笛卡尔坐标转换为柱面坐标
inline Cylindrical ConvertToCylindrical(const Point3D &v) {
	Cylindrical c;
	c.r = sqrtf(v.x*v.x + v.y * v.y);
	c.theta = atanf(v.y / v.x);
	c.z = v.z;
	return c;
}
//柱面坐标转换为3维笛卡尔坐标
inline Point3D ConvertToPoint3D(const Cylindrical & c) {
	Point3D p;
	p.x = c.r * cosf(c.theta);
	p.y = c.r * sinf(c.theta);
	p.z = c.z;
	return p;
}


//3维笛卡尔坐标转换为球面坐标
inline Spherical3D ConvertToSpherical3D(const Point3D &v) {
	Spherical3D sp;
	sp.p = sqrtf(v.x*v.x + v.y * v.y + v.z * v.z);
	sp.theta = atanf(v.y / v.x);
	FloatType r = sqrtf(v.x*v.x + v.y * v.y);
	sp.phi = atanf(r / v.z);
	return sp;
}

//球面坐标转换为3维笛卡尔坐标
inline Point3D ConvertToPoint3D(const Spherical3D& s) {
	Point3D pt;
	auto r = s.p * sinf(s.phi);
	pt.x = r * cosf(s.theta);
	pt.y = r * sinf(s.theta);
	pt.z = s.p * cosf(s.phi);
}