#pragma once
#include "VectorXD.h"
#include <string>
constexpr auto MAX_VERTICES = 128; //������󶥵���
constexpr auto MAX_POLYS = 128;//�������������
constexpr auto MAX_RENDER_POLYGON = 128;//��Ⱦ�б����������
constexpr auto OBJECT_STATE_ACTIVE = 0x0001;//��������
constexpr auto OBJECT_STATE_VISIBLE = 0x0002;//����ɼ���

//����� attr
constexpr auto POLY4DV1_ATTR_2SIDED = 0x0001;
constexpr auto POLY4DV1_ATTR_TRANSPARENT = 0x0002;
constexpr auto POLY4DV1_ATTR_8BITCOLOR = 0x0004;
constexpr auto POLY4DV1_ATTR_RGB16 = 0x0008;
constexpr auto POLY4DV1_ATTR_RGB24 = 0x0010;

// �����states
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

/**************************�����***************************/
class  Polygon
{
public:
	 Polygon(){}
	~ Polygon(){}
	int state;//״̬
	int attr;//��������
	int color;//��ɫ
	Point4D *vlist;//�����б�
	int vert[3];//�����б���Ԫ������
};

/***********************�԰��������******************************/
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

/*************************����************************************/
class Object
{
public:
	Object(){}
	~Object(){}
	int id;//�����ʶ
	std::string name;//����
	int state;//״̬
	int attr;//����
	float avgRadius;//����ƽ���뾶
	float maxRadius;//���뾶
	Vector4D worldPos;//��������
	Vector4D dir;//�����ھֲ�����ϵ����ת�Ƕ�
	Vector4D ux, uy, uz;//����ֲ����꣬���ڴ洢����ĵĳ�����ת�ڼ佫���Զ�����
	int numVertices;//���嶥����
	Point4D vlistLocal[MAX_VERTICES];//���ڴ洢����ֲ����������
	Point4D vlistTransl[MAX_VERTICES];//�洢�任��Ķ�����������
	int numPolygons;//��������������
	Polygon plist[MAX_POLYS];//���������
};
/************************��������б�*************************************/
class RenderList {
public:
	RenderList() {}
	~RenderList() {}
	void Reset() {
		numPolys = 0;
	}

private:
	int state;//״̬
	int attr;//����
	//��Ⱦ�б���һ��ָ�����飬����ÿ��ָ��ָ��һ���԰����ġ�����Ⱦ�Ķ����
	//����Ҫ����zֵ����ʱ���޸ĵ�����Ⱦ�б�
	PolygonFull* polyPtrs[MAX_RENDER_POLYGON];
	//����δ�����飬Ϊ����ÿ֡��Ϊ����η���/�ͷ��ڴ�
	PolygonFull polyData[MAX_RENDER_POLYGON];
	int numPolys;//��Ⱦ�б��а����Ķ������Ŀ
};
