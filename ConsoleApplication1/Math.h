#pragma once
#include<iostream>
namespace maki {
	const float PI = 3.141592654f;
	const float PI_2 = 6.283185307f;
	const float PI_DIV_2 = 1.570796327f;
	const float PI_DIV_4 = 0.785398163f;
	//非常小的数的常量
	const float EPSILON_E4 = 1E-4f;
	const float EPSILON_E5 = 1E-5f;
	const float EPSILON_E6 = 1E-6f;
	//参数化直线交点
	const int PARM_LINE_NO_INTERSECT = 0;
	const int PARM_LINE_INTERSECT_IN_SEGMENT = 1;
	const int PARM_LINE_INTERSECT_OUT_SEGMENT = 2;
	const int PARM_LINE_EVERYWHERE = 3;

	//比较两个数大小并返回
	template <class T>
	inline T Min(T a, T b) {
		return (a < b) ? a : b;
	}
	template <class T>
	inline T Max(T a, T b) {
		return (a > b) ? a : b;
	}
	//交换数据
	template <class T>
	inline void Swap(T& a, T& b) {
		T tmp;
		tmp = a;
		a = b;
		b = tmp;
	}
	//转换角度和弧度
	inline float DegToRad(float ang) {
		return ang * PI / 180.0f;
	}
	inline float RadToDeg(float rads) {
		return rads * 180.0f / PI;
	}

	inline int& i() {
		static int i_{ 0 };
		return i_;
	}

	inline void usingI() {
		i()++;
	}

	inline float(&SinLook())[361]{
		static float sinLook_[361];
		return sinLook_;
	}
		inline float(&CosLook())[361]{
			static float cosLook_[361];
		return cosLook_;
	}
		//三角函数函数，查表法

		//三角函数相关
		inline void InitSinTable() {
		float w = 0.0f;
		for (int i = 0; i < 361; ++i, ++w)
			SinLook()[i] = sin(DegToRad(w));
	}
	inline void InitCosTable() {
		float w = 0.0f;
		for (int i = 0; i < 361; ++i, ++w)
			CosLook()[i] = cos(DegToRad(w));
	}
	inline float FastSin(float theta) {
		//查表，线性插值
		theta = fmodf(theta, 360);
		if (theta < 0) {
			theta = +360.0f;
		}
		int thetaInt = (int)theta;
		float thetaF = theta - thetaInt;
		return SinLook()[thetaInt]
			+ thetaF * (SinLook()[thetaInt + 1] - SinLook()[thetaInt]);
	}
	inline float FastCos(float theta) {
		//查表，线性插值
		theta = fmodf(theta, 360);
		if (theta < 0) {
			theta = +360.0f;
		}
		int thetaInt = (int)theta;
		float thetaF = theta - thetaInt;
		return CosLook()[thetaInt]
			+ thetaF * (CosLook()[thetaInt + 1] - CosLook()[thetaInt]);
	}

	inline bool Equalf(float a, float b) {
		if (fabs(a - b) <= EPSILON_E5) {
			return true;
		}
		else
		{
			return false;
		}
	}


	//定点数
	class Fix {//有错误，先不管
	public:
		static const int MAX = INT_MAX;
		static const int MIN = INT_MIN;
		static const size_t NUM_BIT = sizeof(int) * 8;
		static const size_t NUM_BIT_OVER_2 = sizeof(int) * 4;
		static const size_t ONE = 1 << NUM_BIT_OVER_2;
		static const size_t TEN = 10 << NUM_BIT_OVER_2;
		static const size_t HALF = 1 << (NUM_BIT_OVER_2 - 1);
		static const int LOW_MASK = 0X0000FFFF;
		static const int HIGH_MASK = 0XFFFF0000;
		Fix(int v) :val(v) {}
		Fix(double v) {
			val = int(v * ONE);
		}
		Fix(const Fix& f) :val(f.val) {}
		Fix(const float v) {
			val = int(v * ONE);
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
			auto lowHigh = (int)low * yHigh;
			auto highLow = high * (int)yLow;
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
}