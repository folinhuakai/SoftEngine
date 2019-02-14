#include "Test.h"
void TestVector2D() {
	Vector2D a(3.0f, 4.0f);
	//计算长度
	FloatType sourcef = a.Length() - 5.0f;
	FloatType desf = EPSILON_E5;
	assert(sourcef <= EPSILON_E5);
	//赋值
	Vector2D source(1.6f, 1.8f);
	Vector2D des(3.0f, 4.0f);
	source = a;
	assert(source == des);
	//加法
	Vector2D tmp(1.0f, 2.0f);
	source = a + tmp;
	des = Vector2D(4.0f, 6.0f);
	assert(source == des);
	//减法
	source = a - tmp;
	des = Vector2D(2.0f, 2.0f);
	assert(source == des);
	//乘法
	sourcef = a * tmp - 11.0f;
	assert(sourcef <= EPSILON_E5);
	//缩放
	source = 2 * a * 2;
	des = Vector2D(12.0f, 16.0f);
	assert(source == des);
	//余弦值
	tmp = Vector2D(2.0f, 2.0f);
	sourcef = CosTh(a, tmp) - 1.4f;
	assert(sourcef <= EPSILON_E5);
	//转换成极坐标
	Polar2D s = ConvertToPolar2D(a);
	Polar2D d(5.0f, 0.92729f);
	assert(s == d);
	//归一化
	Vector2D nor(0.6f, 0.8f);
	a.Normalize();
	assert(a == nor);
}