#pragma once
#include "VectorXD.h"
#include "Matrix.h"
#include "ParmLine.h"
#include <string>
#include "Math.h"

namespace maki{
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

	constexpr auto POLY4DV1_ATTR_SHADE_MODE_PURE = 0x0020;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_CONSTANT = 0x0020;// (alias)
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_FLAT = 0x0040;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_GOURAUD = 0x0080;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_PHONG = 0x0100;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_FASTPHONG = 0x0100;// (alias)
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_TEXTURE = 0x0200;

	// �����states
	enum  PloygonStates
	{
		kInit = 0,
		kActive = 0x0001,//��Ծ
		kClipped = 0x0002,//�ü�
		kBackface = 0x0004//����
	};
	enum class TransfromType
	{
		kLocalOnly,
		kTransOnly,
		kLocalToTrans
	};

	/**************************�����***************************/
	class Polygon
	{
	public:
		Polygon(){}
		~Polygon() {}
		PloygonStates state{ PloygonStates::kInit };//״̬
		int attr{ 0 };//��������
		int color{ 0 };//��ɫ
		Point4D *vlist{};//�����б�
		int vert[3];//�����б���Ԫ������
	};

	/***********************�԰��������******************************/
	class PolygonFull
	{
	public:
		PolygonFull() {}
		~PolygonFull() {}
		PloygonStates state{ PloygonStates::kInit };//״̬
		int attr{ 0 };
		int color{ 0 };

		Point4D vlist[3];//�����ζ���
		Point4D tvlist[3];//�任�󶥵�

		Polygon *next{};
		Polygon *prev{};
		
	};

	/*************************����************************************/
	class Object
	{
	public:
		Object() = default;
		~Object() {}
		int id{ 0 };//�����ʶ
		std::string name;//����
		int state{ 0 };//״̬
		int attr{ 0 };//����
		float avgRadius{ 0 };//����ƽ���뾶
		float maxRadius{ 0 };//���뾶
		Vector4D worldPos;//��������
		Vector4D dir;//�����ھֲ�����ϵ����ת�Ƕ�
		Vector4D ux, uy, uz;//����ֲ����꣬���ڴ洢����ĵĳ�����ת�ڼ佫���Զ�����
		int numVertices{ 0 };//���嶥����
		Point4D vlistLocal[MAX_VERTICES];//���ڴ洢����ֲ����������
		Point4D vlistTransl[MAX_VERTICES];//�洢�任��Ķ�����������
		int numPolygons{ 0 };//��������������
		Polygon plist[MAX_POLYS];//���������
		//������Ķ����б���б任	//@parm obj��Ҫ�任�����壬mat �任���� ��type ָ��Ҫ�任���б� isChange �Ƿ�Գ��������任
		void TransfromObject(Matrix<float, 4, 4> &mat,const TransfromType type, bool isChange) {
			switch (type) {
			case TransfromType::kLocalOnly:
				for (int i = 0; i < numVertices; ++i) {
					vlistLocal[i] = vlistLocal[i] * mat;
				}
				break;
			case TransfromType::kTransOnly:
				for (int i = 0; i < numVertices; ++i) {
					vlistTransl[i] = vlistTransl[i] * mat;
				}
				break;
			case TransfromType::kLocalToTrans:
				for (int i = 0; i < numVertices; ++i) {
					vlistTransl[i] = vlistLocal[i] * mat;
				}
				break;
			default:
				break;
			}

			//�Ƿ�任���峯��
			if (isChange) {
				ux = ux * mat;
				uy = uy * mat;
				uz = uz * mat;
			}
		}
		// ������Ӿֲ�����ת������������
		void TransfromToWorld(const TransfromType type = TransfromType::kLocalToTrans) {
			if (type == TransfromType::kLocalToTrans) {
				for (int i = 0; i < numVertices; ++i) {
					vlistTransl[i] = vlistLocal[i] + worldPos;
				}
			}
			else {//TransfromType::kTransOnly
				for (int i = 0; i < numVertices; ++i) {
					vlistTransl[i] = vlistTransl[i] + worldPos;
				}
			}

		}
		void TransfromToWorldMat(TransfromType type = TransfromType::kLocalToTrans,bool isChange = true) {
			Matrix<float, 4, 4> mat = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				worldPos.x,worldPos.y,worldPos.z,1 };
			TransfromObject(mat, TransfromType::kLocalToTrans, isChange);
		}
		
	};


	/************************��������б�*************************************/
	class RenderList {
	public:
		RenderList(){}
		~RenderList() {}
		void Reset() {
			numPolys = 0;
		}
		int state{ 0 };//״̬
		int attr{ 0 };//����
		//��Ⱦ�б���һ��ָ�����飬����ÿ��ָ��ָ��һ���԰����ġ�����Ⱦ�Ķ����
		//����Ҫ����zֵ����ʱ���޸ĵ�����Ⱦ�б�
		PolygonFull* polyPtrs[MAX_RENDER_POLYGON];
		//����δ�����飬Ϊ����ÿ֡��Ϊ����η���/�ͷ��ڴ�
		PolygonFull polyData[MAX_RENDER_POLYGON];
		int numPolys{ 0 };//��Ⱦ�б��а����Ķ������Ŀ
		//����Ⱦ�б�Ķ���α任	//@parm rederList ��Ⱦ�б�mat �任���� ��type ָ��Ҫ�任������
		void TransformRenderList(Matrix<float, 4, 4> &mat, TransfromType type) {
			switch (type) {
			case TransfromType::kLocalOnly:
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->vlist[vetNum];
						curPoly->vlist[vetNum] = vect * mat;
					}
				}
				break;
			case TransfromType::kTransOnly:
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->tvlist[vetNum];
						curPoly->tvlist[vetNum] = vect * mat;
					}
				}
				break;
			case TransfromType::kLocalToTrans:
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->vlist[vetNum];
						curPoly->tvlist[vetNum] = vect * mat;
					}
				}
				break;
			default:
				break;
			}
		}
		//ת������������
		void TransfromToWorld(Vector4D worldPos,TransfromType type = TransfromType::kLocalToTrans) {
			if (type == TransfromType::kLocalToTrans) {
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						curPoly->tvlist[vetNum] = curPoly->vlist[vetNum] + worldPos;
					}
				}
			}
			else {//TransfromType::kTransOnly
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						curPoly->tvlist[vetNum] = curPoly->tvlist[vetNum] + worldPos;
					}
				}
			}

		}
	};

	/**********************************���**********************************/
	class camera {
	public:
		int state{ 0 };
		int attr{ 0 };
		Point4D pos;//��������������е�λ��
		Vector4D dir;//ŷ���ǶȻ�UVN���ģ�͵�ע�ӷ���

		Vector4D u{ 1.0,0.0,0.0,1.0 };//UVN���ģ�͵ĳ���
		Vector4D v{ 0.0,1.0,0.0,1.0 };
		Vector4D n{ 0.0,0.0,1.0,1.0 };
		Point4D target;//UVNģ��Ŀ���λ��

		float viewDist{ 0.0 };//����
		float viewDistH{ 0.0 };//ˮƽ�Ӿ�/��ֱ�Ӿ�
		float viewDistV{ 0.0 };

		float fov{ 0.0 };//ˮƽ����/��ֱ�������Ұ

		//3d�ü��棬�����Ұ����90�㣬3d�ü��潫Ϊһ������ƽ�淽��
		float nearClipZ{ 0.0 };//���ü���
		float farClipZ{ 0.0 };//Զ�ü���

		Plane3D rtClipPlane;//�Ҳü���
		Plane3D ltClipPlane;//��ü���
		Plane3D tpClipPlane;//�ϲü���
		Plane3D btClipPlane;//�²ü���

		float viewPlaneWidth{ 0.0 };//��ƽ����/�߶�
		float viewPlaneHeight{ 0.0 };//���ڹ�һ��ͶӰ��Ϊ2x2�������С���ӿڻ���Ļ������ͬ

		//��Ļ���ӿ���ͬ���

		float viewPortWidth{ 0.0 };//��Ļ��С
		float viewPortHeight{ 0.0 };//��Ļ��С
		float viewPortCenterX{ 0.0 };//��Ļ����
		float viewPortCenterY{ 0.0 };

		float aspectRatio{ 0.0 };//��Ļ��߱�

		Matrix<float, 4, 4> mcam;//�������굽�������任����
		Matrix<float, 4, 4> mper;//������굽͸������任����
		Matrix<float, 4, 4> mscr;//͸�����굽��Ļ����任����
		//��ʼ����� @parame fov����Ұ����λ��
		void InitCamera(int camAttr, Point4D &camPos,Vector4D camdir, Point4D &camTarget,
			float nearClipZ, float farClipZ,float fov, 	float viewportWidth,float viewportHeight){
			attr = camAttr;  
			pos = camPos;
			dir = camdir;     

			target = camTarget;	
			nearClipZ = nearClipZ;
			farClipZ = farClipZ;
			
			viewPortWidth = viewportWidth;   // dimensions of viewport
			viewPortHeight = viewportHeight;

			viewPortCenterX = (viewPortWidth - 1) / 2; // center of viewport
			viewPortCenterY = (viewPortHeight - 1) / 2;

			aspectRatio = viewPortWidth / viewPortHeight;

			// ��λ����
			Matrix<float, 4, 4>  mat = {
				1.0,0.0,0.0,0.0,
				0.0,1.0,0.0,0.0,
				0.0,0.0,1.0,0.0,
				0.0,0.0,0.0,1.0
			};
			
			mcam = mat;
			mper = mat;
			mscr = mat;
			
			fov = fov;

			//��ƽ���СΪ 2 x (2/ar)
			viewPlaneWidth = 2.0;
			viewPlaneHeight = 2.0 / aspectRatio;
		
			// now we know fov and we know the viewplane dimensions plug into formula and
			// solve for view distance parameters
			float tan_fov_div2 = tan(DegToRad(fov / 2));

			viewDist = (0.5)*(viewPlaneWidth)*tan_fov_div2;

			if (fabs(fov - 90.0f) < EPSILON_E5)
			{
				Point3D ptOri;	

				Vector3D vn{1,0,-1}; // �淨��
				rtClipPlane = Plane3D(ptOri, vn,true);

				vn = Vector3D{ -1,0,-1 };
				ltClipPlane = Plane3D(ptOri, vn,true);

				vn = Vector3D{ 0, 1, -1 };
				tpClipPlane = Plane3D(ptOri, vn, true);

				vn = Vector3D{ 0, -1, -1 };
				btClipPlane = Plane3D(ptOri, vn, true);
			} 
			else
			{
				// now compute clipping planes yuck!
				POINT3D pt_origin; // point on the plane
				VECTOR3D_INITXYZ(&pt_origin, 0, 0, 0);

				VECTOR3D vn; // normal to plane

				// since we don't have a 90 fov, computing the normals
				// are a bit tricky, there are a number of geometric constructions
				// that solve the problem, but I'm going to solve for the
				// vectors that represent the 2D projections of the frustrum planes
				// on the x-z and y-z planes and then find perpendiculars to them

				// right clipping plane, check the math on graph paper 
				VECTOR3D_INITXYZ(&vn, cam->view_dist, 0, -cam->viewplane_width / 2.0);
				PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);

				// left clipping plane, we can simply reflect the right normal about
				// the z axis since the planes are symetric about the z axis
				// thus invert x only
				VECTOR3D_INITXYZ(&vn, -cam->view_dist, 0, -cam->viewplane_width / 2.0);
				PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);

				// top clipping plane, same construction
				VECTOR3D_INITXYZ(&vn, 0, cam->view_dist, -cam->viewplane_width / 2.0);
				PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);

				// bottom clipping plane, same inversion
				VECTOR3D_INITXYZ(&vn, 0, -cam->view_dist, -cam->viewplane_width / 2.0);
				PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
			} // end else

		} // end Init_CAM4DV1
	};

	/****************************************��ӡgameobject*****************************/
	inline std::ostream & operator <<(std::ostream &out, const Object &obj) {
		std::cout << "id:" << obj.id << std::endl;
		std::cout << "name:" << obj.name << std::endl;
		std::cout << "state:" << std::hex << obj.state << std::endl;
		std::cout << "attr:" << std::hex << obj.attr << std::endl;
		std::cout << "avgRadius:" << obj.avgRadius << std::endl;
		std::cout << "avgRadius:" << obj.maxRadius << std::endl;
		std::cout << "dir:" << obj.dir.x << " " << obj.dir.y << " " << obj.dir.z << " " << obj.dir.w << std::endl;
		std::cout << "worldPos:" << obj.worldPos.x << " " << obj.worldPos.y << " " << obj.worldPos.z << " " << obj.worldPos.w << std::endl;
		std::cout << "ux:" << obj.ux.x << " " << obj.ux.y << " " << obj.ux.z << " " << obj.ux.w << std::endl;
		std::cout << "uz:" << obj.uz.x << " " << obj.uz.y << " " << obj.uz.z << " " << obj.uz.w << std::endl;
		std::cout << "uy:" << obj.uy.x << " " << obj.uy.y << " " << obj.uy.z << " " << obj.uy.w << std::endl;
		std::cout << "***********************************************" << std::endl;
		std::cout << "numVertices:" << obj.numVertices << std::endl;
		for (int i = 0; i < obj.numVertices; ++i) {
			auto v = obj.vlistLocal[i];
			std::cout << i << " vlistLocal:" << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
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
			std::cout << i << " Polygon:" << "state|" << std::hex << v.state << " attr|" << std::hex << v.state << " color|" << v.color;
			std::cout << " index:" << v.vert[0] << " " << v.vert[1] << " " << v.vert[2] << std::endl;
		}
		std::cout << "***********************************************" << std::endl;

		return out;
	}

	inline bool operator ==(const Object&a, const Object&b) {
		if (&a == &b) {
			return true;
		}
		bool flag = true;
		flag = (a.numPolygons == b.numPolygons) &&
			(a.numVertices == b.numVertices) &&
			(a.state == b.state) &&
			(a.attr == b.attr) && 
			(fabs(a.avgRadius - b.avgRadius) < EPSILON_E5) && 
			(fabs(a.maxRadius - b.maxRadius) < EPSILON_E5) &&
			(a.ux == b.ux) &&
			(a.uy == b.uy) &&
			(a.uz == b.uz) &&
			(a.worldPos == b.worldPos) &&
			(a.dir == b.dir);
		if (!flag) {
			return flag;
		}
		else {
			for (int i = 0; i < a.numVertices; ++i) {
				if (a.vlistLocal[i] == b.vlistLocal[i] && a.vlistTransl[i] == b.vlistTransl[i]) {
					continue;
				}
				else {
					return false;
				}

			}
		}
		return flag;
	}
	

}