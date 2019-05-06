#pragma once
#include "VectorXD.h"
#include <string>
constexpr auto MAX_VERTICES = 128;
constexpr auto MAX_POLYS = 128;
class  Polygon
{
public:
	 Polygon(){}
	~ Polygon(){}

private:
	int state;//状态
	int attr;//物理属性
	int color;//颜色
	Point4D *vlist;//顶点列表
	int vert[3];//顶点列表中元素索引
};

class PolygonFull
{
public:
	PolygonFull() {}
	~PolygonFull() {}

private:
	int state;
	int attr;
	int color;

	Point4D vlist[3];//三角形顶点
	Point4D tvlist[3];//变换后顶点

	Polygon *next;
	Polygon *prev;
};

class Object
{
public:
	Object(){}
	~Object(){}

private:
	int id;//物体标识
	std::string name;//名称
	int state;//状态
	int attr;//属性
	float avgRadius;//物体平均半径
	float maxRadius;//最大半径
	Point4D worldPos;//世界坐标
	Vector4D dir;//物体在局部坐标系的旋转角度
	Vector4D ux, uy, uz;//物体局部坐标，用于存储物体的的朝向，旋转期间将被自动更新
	int numVertices;//物体顶点数
	Point4D vlistLocal[MAX_VERTICES];//用于存储顶点局部坐标的数组
	Point4D vlistTransl[MAX_VERTICES];//存储变换后的顶点坐标数组
	int numPolygons;//物体网格多边形数
	Polygon plist[MAX_POLYS];//多边形数组
};
