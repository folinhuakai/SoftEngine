// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include <fstream>
#include "Test.h"
#include "Object.h"
#include<vector>
#include <windows.h>
using namespace std;

std::string workingdir()
{
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	return std::string(buf) + '\\';
}

int Load_OBJECT4DV1_PLG(Object& obj, //物体
	const string &nane,     //文件名
	Vector4D &scale,	 //初始缩放
	Vector4D &pos,       //世界坐标位置
	Vector4D &rot);       // 初始旋转
std::vector<std::string> Split(const std::string &s, const std::string &seperator);
int main()
{
	TestVector2D();
	TestVector3D();
	TestVector4D();
	TestQuat();
	TestCoordinate();
	TestParmLine();
	Object obj;
	string name = "Resources\\markerg1.plg";
	Vector4D scale = { 1,1,1,1 };
	Vector4D pos = { 0,0,0,1 };
	Vector4D rot = { 0,0,0,1 };
	Load_OBJECT4DV1_PLG(obj,name, scale,pos,rot);  
	cout << obj << endl;
	return 0;

}

