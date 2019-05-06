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
	int state;//״̬
	int attr;//��������
	int color;//��ɫ
	Point4D *vlist;//�����б�
	int vert[3];//�����б���Ԫ������
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

	Point4D vlist[3];//�����ζ���
	Point4D tvlist[3];//�任�󶥵�

	Polygon *next;
	Polygon *prev;
};

class Object
{
public:
	Object(){}
	~Object(){}

private:
	int id;//�����ʶ
	std::string name;//����
	int state;//״̬
	int attr;//����
	float avgRadius;//����ƽ���뾶
	float maxRadius;//���뾶
	Point4D worldPos;//��������
	Vector4D dir;//�����ھֲ�����ϵ����ת�Ƕ�
	Vector4D ux, uy, uz;//����ֲ����꣬���ڴ洢����ĵĳ�����ת�ڼ佫���Զ�����
	int numVertices;//���嶥����
	Point4D vlistLocal[MAX_VERTICES];//���ڴ洢����ֲ����������
	Point4D vlistTransl[MAX_VERTICES];//�洢�任��Ķ�����������
	int numPolygons;//��������������
	Polygon plist[MAX_POLYS];//���������
};
