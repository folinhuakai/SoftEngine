// ConsoleApplication1.cpp: �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include<iostream>
#include <fstream>
#include "Test.h"
#include "Object.h"
using namespace std;


int Load_OBJECT4DV1_PLG(Object& obj, //����
	const char * filename,     //�ļ���
	Vector4D &scale,	 //��ʼ����
	Vector4D &pos,       //��������λ��
	Vector4D &rot);       // ��ʼ��ת
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
	Load_OBJECT4DV1_PLG(obj,
		name,     //�ļ���
		scale,	 //��ʼ����
		pos,       //��������λ��
		rot);      // ��ʼ��ת
	return 0;
}

