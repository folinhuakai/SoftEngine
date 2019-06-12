#include "main_switch.h"

#if UNIT_TEST_MODE

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#else

#include<iostream>
#include <fstream>
#include "Test.h"
#include <windows.h>


using namespace std;
using namespace maki;

std::string workingdir()
{
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	return std::string(buf) + '\\';
}


int main()
{
	/*TestVector2D();
	TestVector3D();
	TestVector4D();
	TestQuat();
	TestCoordinate();
	TestParmLine();
	TestLoadObject();*/
	TestDrawLine();
	return 0;
}

#endif // #if UNIT_TEST
