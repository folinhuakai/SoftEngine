// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Math.h"
#include<iostream>
#include <fstream>

using namespace std;
int main()
{
	Matrix<FloatType, 4, 4> a = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	Vector3D v = { 1,1,1 };
	cout << a;
	cout << (a * v);
	return 0;
}

