#include "Test.h"
using namespace std;
namespace maki {
	void TestVector2D() {
		Vector2D a(3.0f, 4.0f);
		//计算长度
		float sourcef = a.Length() - 5.0f;
		float desf = EPSILON_E5;
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
		Vector3D a(3.0f, 4.0f, 5.0f);
		Vector3D tmp(1.0f, 2.0f, 3.0f);
		//计算长度
		float sourcef = a.Length() - 7.0710678f;
		float desf = EPSILON_E5;
		assert(fabs(sourcef) <= EPSILON_E5);
		//赋值
		Vector3D source(1.6f, 1.8f, 1.9f);
		Vector3D des(3.0f, 4.0f, 5.0f);
		source = a;
		assert(source == des);
		//加法	
		source = a + tmp;
		des = Vector3D(4.0f, 6.0f, 8.0f);
		assert(source == des);
		//减法
		source = a - tmp;
		des = Vector3D(2.0f, 2.0f, 2.0f);
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
		des = Vector3D(12.0f, 16.0f, 20.f);
		assert(source == des);
		//余弦值
		tmp = Vector3D(2.0f, 2.0f, 2.0f);
		sourcef = CosTh(a, tmp) - 0.9797958f;
		assert(fabs(sourcef) <= EPSILON_E5);
		//转换成柱面坐标
		auto s = ConvertToCylindrical(a);
		Cylindrical d(5.0f, 0.92729f, 5.0f);
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
		Vector4D a(3.0f, 4.0f, 5.0f, 1.0f);
		Vector4D tmp(1.0f, 2.0f, 3.0f, 1.0f);
		//计算长度
		float sourcef = a.Length() - 7.0710678f;
		float desf = EPSILON_E5;
		assert(fabs(sourcef) <= EPSILON_E5);
		//赋值
		Vector4D source(1.6f, 1.8f, 1.9f, 1.0f);
		Vector4D des(3.0f, 4.0f, 5.0f, 1.0f);
		source = a;
		assert(source == des);
		//加法	
		source = a + tmp;
		des = Vector4D(4.0f, 6.0f, 8.0f, 1.0f);
		assert(source == des);
		//减法
		source = a - tmp;
		des = Vector4D(2.0f, 2.0f, 2.0f, 1.0f);
		assert(source == des);
		//乘法
		sourcef = a * tmp - 26.0f;
		assert(fabs(sourcef) <= EPSILON_E5);
		//叉乘
		source = a.Cross(tmp);
		des = Vector4D(2.0f, -4.0f, 2.0f, 1.0f);
		assert(source == des);
		//缩放
		source = 2 * a * 2;
		des = Vector4D(12.0f, 16.0f, 20.f, 1.0f);
		assert(source == des);
		//余弦值
		tmp = Vector4D(2.0f, 2.0f, 2.0f, 1.0f);
		sourcef = CosTh(a, tmp) - 0.9797958f;
		assert(fabs(sourcef) <= EPSILON_E5);
		//归一化
		Vector4D nor(0.424264f, 0.565685f, 0.707106f, 1.0f);
		a.Normalize();
		assert(a == nor);
	}

	void TestQuat() {
		Quat a(0.707106f, 0.0f, 0.707106f, 0.0f);
		Quat tmp(1.0f, 2.0f, 3.0f, 1.0f);
		//创建旋转单位四元数
		Vector3D v(0.0f, 1.0f, 0.0f);
		Quat source(v, PI_DIV_2);
		Quat des(0.707106f, 0.0f, 0.707106f, 0.0f);
		assert(source == des);
		//单位四元数转换成v和角度
		float theta = 0.0f;
		Vector3D vs(0.0f, 1.0f, 0.0f);
		des.ConvToVector3DAndTheta(v, theta);
		assert(vs == v);
		assert(fabs(theta - PI_DIV_2) <= EPSILON_E5);
		//计算长度
		float sourcef = a.Length() - 1.0f;
		float desf = EPSILON_E5;
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
		Polar2D a(2.0f, PI / 6.0f);
		Vector2D s = ConvertToPoint2D(a);
		Vector2D d(1.73205078f, 1.0f);
		assert(s == d);
	}

	void TestMatrix() {
		Matrix<float, 2, 3> a = { 1,4,2,2,0,0 };
		Matrix<float, 2, 3> b = { 0,0,5,7,5,0 };
		//+
		auto source = a + b;
		Matrix<float, 2, 3> d = { 1,4,7,9,5,0 };
		assert(source == d);
		//-
		source = a - b;
		d = { 1,4,-3,-5,-5,0 };
		assert(source == d);
		//*
		source = 2 * a * 2;
		d = { 4,16,8,8,0,0 };
		assert(source == d);
		//转置
		auto s = a.Transpose();
		Matrix<float, 3, 2> d1 = { 1,2,4,0,2,0 };
		assert(s == d1);
		//赋值	
		Matrix<float, 2, 3> tmp = { 1,3,4,6,2,8 };
		tmp = a;
		assert(tmp == a);
		//乘法
		a = Matrix<float, 2, 3>({ 1,0,2,-1,3,1 });
		Matrix<float, 3, 2> c({ 3,1,2,1,1,0 });
		auto s2 = a * c;
		Matrix<float, 2, 2> d2 = { 5,1,4,2 };
		assert(s2 == d2);
		//2*2矩阵求逆
		Matrix<float, 2, 2> ret({ 2,1,-1,0 });
		InverseMatrix2X2(ret, s2);
		d2 = Matrix<float, 2, 2>({ 0,-1,1,2 });
		assert(s2 == d2);
		//3*3矩阵求逆
		Matrix<float, 3, 3> ret3({ 1,2,3,2,2,1,3,4,3 });
		Matrix<float, 3, 3> s3;
		Matrix<float, 3, 3> d3({ 1,3,-2,-3 / 2,-3,5 / 2,1,1,-1 });
		InverseMatrix3X3(ret3, s3);
		assert(s3 == d3);
		//方程组求解
		Matrix<float, 3, 3> d33({ 1.0,1.0,1.0,0,2,5 ,2,5,-1 });
		Matrix<float, 3, 1> b1({ 6,-4,27 });
		Matrix<float, 3, 1> ret4;
		Matrix<float, 3, 1> res33({ 5,3,-2 });
		auto res = SolveSystem3_3(d33, ret4, b1);
		assert(res == 1);
		assert(ret4 == res33);
	}

	void TestParmLine() {
		Point2D p0(1, 1);
		Point2D p1(8, 5);
		ParmLine2D line1(p0, p1);
		Point2D p2(3, 6);
		Point2D p3(8, 3);
		ParmLine2D line2(p2, p3);
		float t1 = 0;
		float t2 = 0;
		int type = IntersectionOfLine2D(line1, line2, t1, t2);
		assert(type == PARM_LINE_INTERSECT_IN_SEGMENT);
		assert(t1 - 0.756 < 0.01);
		assert(t2 - 0.658 < 0.01);

		auto pt = line1.ComputeParmLine2D(0.5);
		Point2D des(4.5, 3);
		assert(pt == des);

		Point3D q0(1, 1, 1);
		Point3D q1(8, 5, 5);
		ParmLine3D line3(q0, q1);
		auto pt1 = line3.ComputeParmLine3D(0.5);
		Point3D des1(4.5, 3, 3);
		assert(pt1 == des1);

		q0 = Point3D(0, 0, 0);
		Vector3D n(1, 1, 1);
		Plane3D pl(q0, n, true);
		Point3D test(50, 50, 50);
		float type1 = pl.ComputePointInPlane3D(test);
		assert(type1 > 0);

		Point3D pt11(5, 5, -5);
		Point3D pt2(5, 5, 5);
		ParmLine3D line(pt11, pt2);
		n = Vector3D(0, 0, 1);
		q0 = Point3D(0, 0, 0);
		pl = Plane3D(q0, n, true);
		Point3D pt3(0.0, 0.0, 0.0);
		Point3D des3(5.0, 5.0, 0.0);
		type = IntersectionOfLineAndPlane3D(line, pl, t1, pt3);
		assert(type == PARM_LINE_INTERSECT_IN_SEGMENT);
		assert(fabs(t1 - 0.5) <= 0);
		assert(pt3 == des3);
	}

	void TestLoadObject() {
		maki::Object obj;
		maki::Object obj1;
		std::string name = "Resources\\markerg1.plg";
		Vector4D scale = { 1,1,1,1 };
		Vector4D pos = { 3,2,1,1  };
		Vector4D pos1 = { 0,0,0,1 };
		Vector4D rot = { 0,0,0,1 };
		Load_OBJECT4DV1_PLG(obj, name, scale, pos, rot);
		Load_OBJECT4DV1_PLG(obj1, name, scale, pos, rot);
		obj.TransfromToWorld(TransfromType::kLocalToTrans);
		obj1.TransfromToWorldMat(TransfromType::kLocalToTrans,false);
		assert((obj == obj1));
	}

	void TestDrawLine() {
		Vector4D pt1 = { 0.5,0.6,0,0 };
		Vector4D pt2 = { 21,300,0,0 };
		auto total = screenWidth * screenHeight;
		uchar *dest_buffer = new uchar[total * 4];
		memset(dest_buffer, 0, sizeof(char) * total *4);
		auto begin =(int *) dest_buffer;
		DrawClipLine(pt1, pt2, 1, dest_buffer, screenWidth * 4);
		for (int i = 0; i < total; ++i) {
			auto c = * begin;
			if (c == 1) {
				cout << "X";
			}
			else {
				cout << "o";
			}
			if ((i+1)%screenWidth == 0 and i!= 0) {
				cout << endl;
			}
			begin += 1;
		}
		delete[]dest_buffer;
	}
}