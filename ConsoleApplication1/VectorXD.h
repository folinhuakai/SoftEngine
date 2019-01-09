#pragma once
#include "Math.h"
#include "CoordinateSystem.h"

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
	Polar2D ConvertToPolar2D()const;//转换成极坐标
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

inline FloatType CosTh(const Vector2D& a, const Vector2D& b) {
	auto tmp = a * b / (a.Length() *b.Length());
	return tmp;
}
//判断vector2D是否相等
inline bool operator ==(const Vector2D& a, const Vector2D& b) {
	if ((a.x==b.x) && (a.y == b.y)) {
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

//笛卡尔坐标转化成极坐标
inline Polar2D Vector2D::ConvertToPolar2D()const {
	Polar2D p;
	p.r = sqrtf(x*x + y * y);
	p.theta = atanf(y / x);
	return p;
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
	Cylindrical ConvertToCylindrical();
	Spherical3D ConvertToSpherical3D();
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
	if ((a.x == b.x) && (a.y == b.y) && (a.z==b.z)) {
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
//3维笛卡尔坐标转换为柱面坐标
inline Cylindrical Vector3D::ConvertToCylindrical() {
	Cylindrical c;
	c.r = sqrtf(x*x + y * y);
	c.theta = atanf(y / x);
	c.z = z;
	return c;
}
//3维笛卡尔坐标转换为球面坐标
inline Spherical3D Vector3D::ConvertToSpherical3D() {
	Spherical3D sp;
	sp.p = sqrtf(x*x + y * y + z * z);
	sp.theta = atanf(y / x);
	FloatType r = sp.p* FastSin(sp.theta);
	sp.phi = atanf(r / z);
	return sp;
}


/******************4维点或向量，用于齐次坐标***************/
class Vector4D {
public:
	Vector4D() :x(0), y(0), z(0), w(0) {}
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
	if ((a.x == b.x) && (a.y == b.y) && (a.z == b.z) &&(a.w==b.w)) {
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
		FloatType theta_div = 0.5 * theta;
		FloatType theta_sinf = sinf(theta_div);
		w = cosf(theta_div);
		x = theta_sinf * v.x;
		y = theta_sinf * v.y;
		z = theta_sinf * v.z;
	}
	Quat(const Vector4D& v, FloatType theta) {
		//方向向量v必须是单位向量，角度单位为弧度
		FloatType theta_div = 0.5 * theta;
		FloatType theta_sinf = sinf(theta_div);
		w = cosf(theta_div);
		x = theta_sinf * v.x;
		y = theta_sinf * v.y;
		z = theta_sinf * v.z;
	}
	Quat(FloatType theta_z, FloatType theta_y, FloatType theta_x) {
		//根据绕x、y、z旋转的角度，创建对应的四元数
		FloatType cos_z = 0.5 * cosf(theta_z);
		FloatType cos_y = 0.5 * cosf(theta_y);
		FloatType cos_x = 0.5 * cosf(theta_x);

		FloatType sin_z = 0.5* sinf(theta_z);
		FloatType sin_x = 0.5* sinf(theta_x);
		FloatType sin_y = 0.5* sinf(theta_y);

		w = cos_z*cos_y*cos_x + sin_z*sin_y*sin_x;
		x = cos_z*cos_y*sin_x - sin_z*sin_y*cos_x;
		y = cos_z*sin_y*cos_x + sin_z*cos_y*sin_x;
		z = sin_z*cos_y*cos_x - cos_z*sin_y*sin_x;
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
		FloatType sinf_theta_inv = 1.0 / sinf(theta);
		v.x = x * sinf_theta_inv;
		v.y = y * sinf_theta_inv;
		v.z = z * sinf_theta_inv;

		theta *= 2;
	}
	Quat Conjufate() {
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
		return Conjufate();
	}
	Quat Inverse() {
		auto inv = 1.0 / Length2();

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
inline Vector4D operator -(const Quat &a, const Quat &b) {
	Vector4D sub(a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z);
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
	FloatType ret9 = 0.5*(ret4 + ret8);

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
	if ((a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w)) {
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
