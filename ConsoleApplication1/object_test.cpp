#include "main_switch.h"

#if UNIT_TEST_MODE

#include "catch.hpp"
#include "Object.h"
#include "LoadObject.h"

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
	Point4D camPos{ 0.0f,0.0f,0.0f,1.0f };
	Point4D camdir{ 2.0f,3.0f,1.0f,1.0f };
	Point4D camTarget{ 4.0f,4.0f,4.0f,1.0f };
	Camera ca1;
	SECTION("变换测试:平移和缩放") {
		Camera ca2;
		ca1.InitCamera(CameraType::kModeEuler, camPos, camdir, camTarget,
			50.0f, 500.0f, 90, 400.0f, 400.0f);
		REQUIRE(obj1 == obj);
	}
}

#endif