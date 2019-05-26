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
	Vector4D pos = { 3,2,1,1 };
	Vector4D rot = { 0,0,0,1 };
	Load_OBJECT4DV1_PLG(obj, name, scale, pos, rot);
	SECTION("两种转换成世界坐标方法测试:") {
		Load_OBJECT4DV1_PLG(obj1, name, scale, pos, rot);
		obj.TransfromToWorld(TransfromType::kLocalToTrans);
		obj1.TransfromToWorldMat(TransfromType::kLocalToTrans, false);
		REQUIRE(obj1 == obj);
	}
	SECTION("两种转换成世界坐标方法测试:") {
		Vector4D pos1 = { 0,0,0,1 };
		Load_OBJECT4DV1_PLG(obj1, name, scale, pos1, rot);
		obj.TransfromToWorld(TransfromType::kLocalToTrans);
		obj1.TransfromToWorldMat(TransfromType::kLocalToTrans, false);
		REQUIRE(obj1 == obj);
	}
}

#endif