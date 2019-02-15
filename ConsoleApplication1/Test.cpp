#include "Test.h"
void TestVector2D() {
	Vector2D a(3.0f, 4.0f);
	//计算长度
	FloatType sourcef = a.Length() - 5.0f;
	FloatType desf = EPSILON_E5;
	assert(fabs(sourcef) <= EPSILON_E5);
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
	assert(fabs(sourcef) <= EPSILON_E5);
	//缩放
	source = 2 * a * 2;
	des = Vector2D(12.0f, 16.0f);
	assert(source == des);
	//余弦值
	tmp = Vector2D(2.0f, 2.0f);
	sourcef = CosTh(a, tmp) - 0.989949f;
	assert(fabs(sourcef) <= EPSILON_E5);
	//转换成极坐标
	Polar2D s = ConvertToPolar2D(a);
	Polar2D d(5.0f, 0.92729f);
	assert(s == d);
	//归一化
	Vector2D nor(0.6f, 0.8f);
	a.Normalize();
	assert(a == nor);
}

void TestVector3D() {
	Vector3D a(3.0f, 4.0f,5.0f);
	Vector3D tmp(1.0f, 2.0f, 3.0f);
	//计算长度
	FloatType sourcef = a.Length() - 7.0710678f;
	FloatType desf = EPSILON_E5;
	assert(fabs(sourcef) <= EPSILON_E5);
	//赋值
	Vector3D source(1.6f, 1.8f,1.9f);
	Vector3D des(3.0f, 4.0f, 5.0f);
	source = a;
	assert(source == des);
	//加法	
	source = a + tmp;
	des = Vector3D(4.0f, 6.0f,8.0f);
	assert(source == des);
	//减法
	source = a - tmp;
	des = Vector3D(2.0f, 2.0f,2.0f);
	assert(source == des);
	//乘法
	sourcef = a * tmp - 26.0f;
	assert(fabs(sourcef) <= EPSILON_E5);
	//叉乘
	source = a.Cross(tmp);
	des = Vector3D(2.0f, -4.0f, 2.0f);
	assert(source == des);
	//缩放
	source = 2 * a * 2;
	des = Vector3D(12.0f, 16.0f,20.f);
	assert(source == des);
	//余弦值
	tmp = Vector3D(2.0f, 2.0f,2.0f);
	sourcef = CosTh(a, tmp) - 0.9797958f;
	assert(fabs(sourcef) <= EPSILON_E5);
	//转换成柱面坐标
	auto s = ConvertToCylindrical(a);
	Cylindrical d(5.0f, 0.92729f,5.0f);
	assert(s == d);
	//转换成球面坐标
	auto s1 = ConvertToSpherical3D(a);
	Spherical3D d1(7.0710678f, 0.92729f, 0.78539f);
	assert(s1 == d1);
	//归一化
	Vector3D nor(0.424264f, 0.565685f, 0.707106f);
	a.Normalize();
	assert(a == nor);
}

void TestVector4D() {
	Vector4D a(3.0f, 4.0f, 5.0f,1.0f);
	Vector4D tmp(1.0f, 2.0f, 3.0f,1.0f);
	//计算长度
	FloatType sourcef = a.Length() - 7.0710678f;
	FloatType desf = EPSILON_E5;
	assert(fabs(sourcef) <= EPSILON_E5);
	//赋值
	Vector4D source(1.6f, 1.8f, 1.9f,1.0f);
	Vector4D des(3.0f, 4.0f, 5.0f,1.0f);
	source = a;
	assert(source == des);
	//加法	
	source = a + tmp;
	des = Vector4D(4.0f, 6.0f, 8.0f,1.0f);
	assert(source == des);
	//减法
	source = a - tmp;
	des = Vector4D(2.0f, 2.0f, 2.0f,1.0f);
	assert(source == des);
	//乘法
	sourcef = a * tmp - 26.0f;
	assert(fabs(sourcef) <= EPSILON_E5);
	//叉乘
	source = a.Cross(tmp);
	des = Vector4D(2.0f, -4.0f, 2.0f,1.0f);
	assert(source == des);
	//缩放
	source = 2 * a * 2;
	des = Vector4D(12.0f, 16.0f, 20.f,1.0f);
	assert(source == des);
	//余弦值
	tmp = Vector4D(2.0f, 2.0f, 2.0f,1.0f);
	sourcef = CosTh(a, tmp) - 0.9797958f;
	assert(fabs(sourcef) <= EPSILON_E5);
	//归一化
	Vector4D nor(0.424264f, 0.565685f, 0.707106f,1.0f);
	a.Normalize();
	assert(a == nor);
}

void TestQuat() {
	Quat a(0.707106f, 0.0f, 0.707106f, 0.0f);
	Quat tmp(1.0f, 2.0f, 3.0f, 1.0f);
	//创建旋转单位四元数
	Vector3D v(0.0f,1.0f, 0.0f);
	Quat source(v, PI_DIV_2);
	Quat des(0.707106f, 0.0f, 0.707106f, 0.0f);	
	assert(source == des);
	//单位四元数转换成v和角度
	FloatType theta = 0.0f;
	Vector3D vs(0.0f, 1.0f, 0.0f);
	des.ConvToVector3DAndTheta(v, theta);
	assert(vs == v);
	assert(fabs(theta- PI_DIV_2) <= EPSILON_E5);
	//计算长度
	FloatType sourcef = a.Length() -1.0f;
	FloatType desf = EPSILON_E5;
	assert(fabs(sourcef) <= EPSILON_E5);
	//赋值
	source = Quat(1.6f, 1.8f, 1.9f, 1.0f);
	des = Quat(0.707106f, 0.0f, 0.707106f, 0.0f);
	source = a;
	assert(source == des);
	//加法	
	source = a + tmp;
	des = Quat(1.707106f, 2.0f, 3.707106f, 1.0f);
	assert(source == des);
	//减法
	a = Quat(1.0f, 2.0f, 3.0f, 4.0f);
	tmp = Quat(5.0f, 6.0f, 7.0f, 8.0f);
	source = a - tmp;
	des = Quat(-4.0f, -4.0f, -4.0f, -4.0f);
	assert(source == des);
	//乘法	
	source = a * tmp;
	des = Quat(-60.0f, 12.0f, 30.0f, 24.0f);
	assert(source == des);
	//缩放
	source = 2 * a * 2;
	des = Quat(4.0f, 8.0f, 12.0f, 16.0f);
	assert(source == des);
	//归一化
	Quat nor(0.18257f, 0.36514f, 0.54772f, 0.73029f);
	a.Normalize();
	assert(a == nor);
}

void TestCoordinate() {
	Polar2D a(2.0f, PI/6.0f);
	Vector2D s = ConvertToPoint2D(a);
	Vector2D d(1.73205078f, 1.0f);
	assert(s == d); 
}