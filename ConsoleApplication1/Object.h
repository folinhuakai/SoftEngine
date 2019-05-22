#pragma once
#include "VectorXD.h"
#include <string>
constexpr auto MAX_VERTICES = 128; //物体最大顶点数
constexpr auto MAX_POLYS = 128;//物体最大多边形数
constexpr auto MAX_RENDER_POLYGON = 128;//渲染列表最大多边形数
constexpr auto OBJECT_STATE_ACTIVE = 0x0001;//物体活动属性
constexpr auto OBJECT_STATE_VISIBLE = 0x0002;//物体可见性

//多边形 attr
constexpr auto POLY4DV1_ATTR_2SIDED = 0x0001;
constexpr auto POLY4DV1_ATTR_TRANSPARENT = 0x0002;
constexpr auto POLY4DV1_ATTR_8BITCOLOR = 0x0004;
constexpr auto POLY4DV1_ATTR_RGB16 = 0x0008;
constexpr auto POLY4DV1_ATTR_RGB24 = 0x0010;

// 多边形states
constexpr auto POLY4DV1_STATE_ACTIVE = 0x0001;
constexpr auto POLY4DV1_STATE_CLIPPED = 0x0002;
constexpr auto POLY4DV1_STATE_BACKFACE = 0x0004;

constexpr auto POLY4DV1_ATTR_SHADE_MODE_PURE = 0x0020;
constexpr auto POLY4DV1_ATTR_SHADE_MODE_CONSTANT = 0x0020;// (alias)
constexpr auto POLY4DV1_ATTR_SHADE_MODE_FLAT = 0x0040;
constexpr auto POLY4DV1_ATTR_SHADE_MODE_GOURAUD = 0x0080;
constexpr auto POLY4DV1_ATTR_SHADE_MODE_PHONG = 0x0100;
constexpr auto POLY4DV1_ATTR_SHADE_MODE_FASTPHONG = 0x0100;// (alias)
constexpr auto POLY4DV1_ATTR_SHADE_MODE_TEXTURE = 0x0200;

/**************************多边形***************************/
class  Polygon
{
public:
	 Polygon(){}
	~ Polygon(){}
	int state;//状态
	int attr;//物理属性
	int color;//颜色
	Point4D *vlist;//顶点列表
	int vert[3];//顶点列表中元素索引
};

/***********************自包含多边形******************************/
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

/*************************物体************************************/
class Object
{
public:
	Object(){}
	~Object(){}
	int id;//物体标识
	std::string name;//名称
	int state;//状态
	int attr;//属性
	float avgRadius;//物体平均半径
	float maxRadius;//最大半径
	Vector4D worldPos;//世界坐标
	Vector4D dir;//物体在局部坐标系的旋转角度
	Vector4D ux, uy, uz;//物体局部坐标，用于存储物体的的朝向，旋转期间将被自动更新
	int numVertices;//物体顶点数
	Point4D vlistLocal[MAX_VERTICES];//用于存储顶点局部坐标的数组
	Point4D vlistTransl[MAX_VERTICES];//存储变换后的顶点坐标数组
	int numPolygons;//物体网格多边形数
	Polygon plist[MAX_POLYS];//多边形数组
};
/************************主多边形列表*************************************/
class RenderList {
public:
	RenderList() {}
	~RenderList() {}
	void Reset() {
		numPolys = 0;
	}

private:
	int state;//状态
	int attr;//属性
	//渲染列表是一个指针数组，其中每个指针指向一个自包含的、可渲染的多边形
	//如需要根据z值排序时，修改的是渲染列表
	PolygonFull* polyPtrs[MAX_RENDER_POLYGON];
	//多边形存放数组，为避免每帧都为多边形分配/释放内存
	PolygonFull polyData[MAX_RENDER_POLYGON];
	int numPolys;//渲染列表中包含的多边形数目
};

//打印gameobject
inline std::ostream & operator <<(std::ostream &out, const Object &obj) {
	std::cout << "id:" << obj.id << std::endl;
	std::cout << "name:" << obj.name << std::endl;
	std::cout << "state:"<< std::hex << obj.state << std::endl;
	std::cout << "attr:"<< std::hex << obj.attr << std::endl;
	std::cout << "avgRadius:"<<obj.avgRadius << std::endl;
	std::cout << "avgRadius:"<<obj.maxRadius << std::endl;
	std::cout << "dir:"<< obj.dir.x << " " << obj.dir.y << " " << obj.dir.z << " " << obj.dir.w << std::endl;
	std::cout << "worldPos:"<< obj.worldPos.x << " " << obj.worldPos.y << " " << obj.worldPos.z << " " << obj.worldPos.w << std::endl;
	std::cout << "ux:"<< obj.ux.x<<" "<< obj.ux.y << " " << obj.ux.z << " " << obj.ux.w << std::endl;
	std::cout << "uz:"<< obj.uz.x<<" "<< obj.uz.y << " " << obj.uz.z << " " << obj.uz.w << std::endl;
	std::cout << "uy:"<< obj.uy.x<<" "<< obj.uy.y << " " << obj.uy.z << " " << obj.uy.w << std::endl;
	std::cout << "***********************************************" << std::endl;
	std::cout << "numVertices:"<< obj.numVertices<< std::endl;
	for (int i = 0; i < obj.numVertices; ++i) {
		auto v = obj.vlistLocal[i];
		std::cout << i<<" vlistLocal:" << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
	}
	std::cout << "***********************************************" << std::endl;
	for (int i = 0; i < obj.numVertices; ++i) {
		auto v = obj.vlistTransl[i];
		std::cout << i << " vlistTransl:" << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
	}
	
	std::cout << "***********************************************" << std::endl;
	std::cout << "numPolygons:" << obj.numPolygons << std::endl;
	for (int i = 0; i < obj.numPolygons; ++i) {
		auto v = obj.plist[i];
		std::cout << i << " Polygon:" << "state|" << std::hex << v.state << " attr|" << std::hex << v.state <<" color|" << v.color;
		std::cout << " index:" << v.vert[0] << " " << v.vert[1] << " " << v.vert[2] << std::endl;
	}
	std::cout << "***********************************************" << std::endl;

	return out;
}