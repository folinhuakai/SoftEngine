#pragma once
#include "Object.h"
#include<vector>
#include<string>
namespace maki {

	int Load_OBJECT4DV1_PLG(Object& obj,const std::string &nane,	Vector4D &scale, Vector4D &pos, Vector4D &rot,bool isPrint = false);     
	std::vector<std::string> Split(const std::string &s, const std::string &seperator);
}