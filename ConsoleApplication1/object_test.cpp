#include "main_switch.h"

#if UNIT_TEST_MODE

#include "catch.hpp"
#include "Object.h"
#include "LoadObject.h"
#include "Draw.h"

using namespace std;
using namespace maki;

TEST_CASE("Object Test") {
	maki::Object obj;
	maki::Object obj1;
	std::string name = "Resources\\markerg1.plg";
	Vector4D scale = { 1,1,1,1 };
	Vector4D pos = { 0,0,0,1 };
	Vector4D rot = { 0,0,0,1 };
	Load_OBJECT4DV1_PLG(obj, name, scale, pos, rot);
	SECTION("两种转换成世界坐标方法测试:") {
		Load_OBJECT4DV1_PLG(obj1, name, scale, pos, rot);
		obj.TransfromToWorld(TransfromType::kLocalToTrans);
		obj1.TransfromToWorldMat(TransfromType::kLocalToTrans, false);
		REQUIRE(obj1 == obj);
	}
	SECTION("变换测试（*单位矩阵）:") {
		Load_OBJECT4DV1_PLG(obj1, name, scale, pos, rot);
		Matrix<float, 4, 4> mat = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1};
		obj.TransfromObject(mat,TransfromType::kLocalOnly,true);
		REQUIRE(obj1 == obj);
	}
	SECTION("变换测试:平移和缩放") {
		Load_OBJECT4DV1_PLG(obj1, name, scale, pos, rot);
		Point4D p = { 2,3,5,0 };
		for (int i = 0; i < obj1.numVertices; ++i) {
			obj1.vlistLocal[i] = (obj1.vlistLocal[i] *3+ p);
		}
		obj1.ux = obj1.ux *3 + p;
		obj1.uy = obj1.uy*3 + p;
		obj1.uz = obj1.uz *3 + p;
		Matrix<float, 4, 4> mat = {
		3,0,0,0,
		0,3,0,0,
		0,0,3,0,
		2,3,5,1 };
		obj.TransfromObject(mat, TransfromType::kLocalOnly, true);
		REQUIRE(obj1 == obj);
	}
}

TEST_CASE("Camera Test") {
	Point4D camPos{ 100.0f,200.0f,300.0f,1.0f };
	Point4D camdir{ -45.0f,0.0f,0.0f,1.0f };
	Point4D camTarget{ 4.0f,4.0f,4.0f,1.0f };
	Camera ca1;
	SECTION("相机初始化测试1") {
		Vector3D v{0.0f, 0.707107f,-0.707107f};
		ca1.InitCamera(CameraType::kModeEuler, camPos, camdir, camTarget,
			50.0f, 500.0f, 90, 400.0f, 400.0f);
		REQUIRE(ca1.tpClipPlane.n == v);
	}

	SECTION("相机初始化测试2") {
		Vector3D v{ 0.0f, 0.707107f,-0.707107f };
		ca1.InitCamera(CameraType::kModeEuler, camPos, camdir, camTarget,
			50.0f, 500.0f, 90, 640.0f, 480.0f);
		REQUIRE(ca1.tpClipPlane.n == v);
	}
	SECTION("相机初始化测试2") {
		Matrix<float, 4, 4> mat = {
		1,0,0,0,
		0,0.525321f,0.850903,0,
		0,-0.850903,0.525321f,0,
		-100.0f,150.206665f,-327.777283f,1 };
		ca1.InitCamera(CameraType::kModeEuler, camPos, camdir, camTarget,
			50.0f, 500.0f, 90, 640.0f, 480.0f);
		ca1.BuildMatrixEuler(CameraRotSeq::kSeqXYZ);
		REQUIRE(ca1.mcam == mat);
	}
	SECTION("相机初始化测试3") {
		camPos = Point4D{10.0f,20.0f,30.0f,1.0f };
		camdir = Point4D{60.0f,60.0f,0.0f,1.0f };
		Matrix<float, 4, 4> mat = {
		-0.939810097f, -0.180016667f, -0.290432453f, 0.0f,
		0.0f, 0.849970043f, -0.526830912f, 0.0f,
		0.341697246f, -0.495121002f, -0.798810482f, 0.0,
		-0.852816582f, -0.345603943f, 37.4052582f, 1.0f };
		ca1.InitCamera(CameraType::kModeUvn, camPos, camdir, camTarget,
			50.0f, 500.0f, 90, 640.0f, 480.0f);
		ca1.BuildCameraMatrixUVN(CameraUvnMode::kSpherical);
		REQUIRE(ca1.mcam == mat);
	}	
}

TEST_CASE("渲染流水线 Test") {
	maki::Object obj;
	std::string name = "Resources\\markerg1.plg";
	Vector4D scale = { 1,1,1,1 };
	Vector4D pos = { 0,0,0,1 };
	Vector4D rot = { 0,0,0,1 };
	Load_OBJECT4DV1_PLG(obj, name, scale, pos, rot);

	Point4D camPos{ 100.0f,200.0f,300.0f,1.0f };
	Point4D camdir{ -45.0f,0.0f,0.0f,1.0f };
	Point4D camTarget{ 4.0f,4.0f,4.0f,1.0f };
	Camera ca1;
	ca1.InitCamera(CameraType::kModeEuler, camPos, camdir, camTarget,
		50.0f, 500.0f, 90, 640.0f, 480.0f);
	SECTION("世界坐标到相机坐标") {
		
		ca1.BuildMatrixEuler(CameraRotSeq::kSeqXYZ);
		obj.TransfromToWorld(TransfromType::kLocalToTrans);
		obj.WorldToCamera(ca1);
		Point4D p0{ -100.000000 ,150.731995 ,-326.926392,1.0 };
		Point4D p1{ -101.000000 ,151.057571 ,-328.302612,1.0 };
		Point4D p2{ -99.0000000 ,151.057571 ,-328.302612,1.0 };
		Point4D p3{ -100.000000 ,149.355759 ,-327.251953,1.0 };
		REQUIRE(obj.vlistTransl[0] == p0);
		REQUIRE(obj.vlistTransl[1] == p1);
		REQUIRE(obj.vlistTransl[2] == p2);
		REQUIRE(obj.vlistTransl[3] == p3);
	}
	SECTION("创建透视变换矩阵") {
		ca1.BuildMatrixEuler(CameraRotSeq::kSeqXYZ);
		Matrix<float, 4, 4> mt{
			ca1.viewDist, 0, 0, 0,
			0, ca1.viewDist*ca1.aspectRatio, 0, 0,
			0, 0, 1, 1,
			0, 0, 0, 0 };
		auto m_test = ca1.GetCameraToPerspectiveMtri();
		REQUIRE(m_test == mt);
	}
}

TEST_CASE("基本图元测试") {
	auto total = screenWidth * screenHeight;
	uchar *dest_buffer = new uchar[total * byteStep];
	memset(dest_buffer, 0, sizeof(char) * total * byteStep);
	auto begin = (int *)dest_buffer;
	int color = 1;
	SECTION("画线") {
		Vector4D pt1 = { 0.5,0.6,0,0 };
		Vector4D pt2 = { 21,300,0,0 };
		DrawClipLine(pt1, pt2, color, dest_buffer, screenWidth * byteStep);
		PrintTest(begin, total, color);
		delete[]dest_buffer;
	}
	SECTION("画三角形") {
		Vector4D pt1 = {10,1,0,0 };
		Vector4D pt2 = { -2,15,0,0 };
		Vector4D pt3 = { 7,15,0,0 };
		DrawBottomTri(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, color, dest_buffer, screenWidth*byteStep);
		PrintTest(begin, total, color);
		delete[]dest_buffer;
	}
}

#endif