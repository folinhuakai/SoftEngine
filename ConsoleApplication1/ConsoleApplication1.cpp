// ConsoleApplication1.cpp: �������̨Ӧ�ó������ڵ㡣
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

int Load_OBJECT4DV1_PLG(Object& obj, //����
	const char * filename,     //�ļ���
	Vector4D &scale,	 //��ʼ����
	Vector4D &pos,       //��������λ��
	Vector4D &rot);       // ��ʼ��ת
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
	char name[]= "markerg1.plg";
	Vector4D scale = { 1,1,1,1 };
	Vector4D pos = { 0,0,0,1 };
	Vector4D rot = { 0,0,0,1 };
	/*Load_OBJECT4DV1_PLG(obj,
		name,     //�ļ���
		scale,	 //��ʼ����
		pos,       //��������λ��
		rot);      // ��ʼ��ת
	return 0;*/
	string line;
	ifstream myfile("Resources\\markerg1.plg");
	
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			cout << line << '\n';
		}
		myfile.close();
	}

}

