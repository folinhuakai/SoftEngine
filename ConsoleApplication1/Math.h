#pragma once
#include<iostream>

using FloatType = float;
const float PI = 3.141592654f;
const float PI_2 = 6.283185307f;
const float PI_DIV_2 = 1.570796327f;
const float PI_DIV_4 = 0.785398163f;
//�ǳ�С�����ĳ���
const float EPSILON_E4 = 1E-4f;
const float EPSILON_E5 = 1E-5f;
const float EPSILON_E6 = 1E-6f;
//������ֱ�߽���
const int PARM_LINE_NO_INTERSECT = 0;
const int PARM_LINE_NO_INTERSECT_IN_SEGMENT = 1;
const int PARM_LINE_INTERSECT_OUT_SEGMENT = 2;
const int PARM_LINE_EVERYWHERE = 3;

//�Ƚ���������С������
template <class T>
inline T Min(T a, T b) {
	return (a < b) ? a : b;
}
template <class T>
inline T Max(T a, T b) {
	return (a > b) ? a : b;
}
//��������
template <class T>
inline void Swap(T& a, T& b) {
	T tmp;
	tmp = a;
	a = b;
	b = tmp;
}
//ת���ǶȺͻ���
inline FloatType DegToRad(FloatType ang) {
	return ang * PI / 180.0f;
}
inline FloatType RadToDeg(FloatType rads) {
	return rads * 180.0f / PI;
}

inline int& i() {
	static int i_{0};
	return i_;
}

inline void usingI() {
	i()++;
}

inline FloatType (&SinLook())[361]{
	static FloatType sinLook_[361];
	return sinLook_;
}
inline FloatType(&CosLook())[361]{
	static FloatType cosLook_[361];
return cosLook_;
}
//���Ǻ������������

//���Ǻ������
inline void InitSinTable() {
	FloatType w = 0.0f;
	for (int i = 0; i < 361; ++i, ++w)
		SinLook()[i] = sin(DegToRad(w));
}
inline void InitCosTable() {
	FloatType w = 0.0f;
	for (int i = 0; i < 361; ++i, ++w)
		CosLook()[i] = cos(DegToRad(w));
}
inline FloatType FastSin(FloatType theta) {
	//������Բ�ֵ
	theta = fmodf(theta, 360);
	if (theta < 0) {
		theta = +360.0f;
	}
	int thetaInt = (int)theta;
	FloatType thetaF = theta - thetaInt;
	return SinLook()[thetaInt]
		+ thetaF * (SinLook()[thetaInt + 1] - SinLook()[thetaInt]);
}
inline FloatType FastCos(FloatType theta) {
	//������Բ�ֵ
	theta = fmodf(theta, 360);
	if (theta < 0) {
		theta = +360.0f;
	}
	int thetaInt = (int)theta;
	FloatType thetaF = theta - thetaInt;
	return CosLook()[thetaInt]
		+ thetaF * (CosLook()[thetaInt + 1] - CosLook()[thetaInt]);
}





//������
class Fix {//�д����Ȳ���
public:	
	static const int MAX = INT_MAX;
	static const int MIN = INT_MIN;
	static const size_t NUM_BIT = sizeof(int)*8;
	static const size_t NUM_BIT_OVER_2 = sizeof(int)*4;
	static const size_t ONE = 1<< NUM_BIT_OVER_2;
	static const size_t TEN = 10<< NUM_BIT_OVER_2;
	static const size_t HALF = 1<< (NUM_BIT_OVER_2-1);
	static const int LOW_MASK = 0X0000FFFF;
	static const int HIGH_MASK = 0XFFFF0000;
	Fix(int v) :val(v){}
	Fix(double v) {
		val =int (v * ONE);
	}
	Fix(const Fix& f):val(f.val){}
	Fix(const float v) {
		val =int (v * ONE);
	}
	Fix operator +(const Fix& y) {
		Fix sum(val + y.val);
		return sum;
	}
	Fix operator *(const Fix& y) {
		auto low = (unsigned int)(val&LOW_MASK);
		auto high = val >> NUM_BIT_OVER_2;
		auto yLow = (unsigned int)(y.val&LOW_MASK);
		auto yHigh = y.val >> NUM_BIT_OVER_2;

		auto low2 = low * yLow;
		auto lowHigh =(int) low * yHigh;
		auto highLow = high *(int) yLow;
		auto high2 = high * yHigh;

		auto lowReault = low2 >> NUM_BIT_OVER_2;
		auto highResult1 = high2 << NUM_BIT_OVER_2;

		auto sum = (int)lowReault + lowHigh + highLow + highResult1;
		Fix res(sum);
		return res;
	}
	operator float() const {
		return (float)val / ONE;
	}
	int GetValue() {
		return val;
	}
private:
	int val;
};

/*
using Point2D = Vector2D;
using Point3D = Vector3D;
class Line2D {
public:
	Point2D p0;
	Point2D p1;
	Vector2D v;
};
class Line3D {
public:
	Point3D p0;
	Point3D p1;
	Vector3D v;
};
class Plane3D {
public:
	Point3D P0;
	Vector3D n;
};*/



class PointInt {
public:
	PointInt(int x, int y);
	PointInt(PointInt& v);
	~PointInt();
	void operator =(const PointInt &v) {
		x = v.x;
		y = v.y;
	}
	inline void PointIntZero(PointInt &v) {
		v.x = 0;
		v.y = 0;
	}
	int x, y;
};