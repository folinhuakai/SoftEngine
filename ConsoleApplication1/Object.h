#pragma once
#include "VectorXD.h"
#include "Matrix.h"
#include "ParmLine.h"
#include <string>
#include "Math.h"

namespace maki{
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

	constexpr auto POLY4DV1_ATTR_SHADE_MODE_PURE = 0x0020;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_CONSTANT = 0x0020;// (alias)
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_FLAT = 0x0040;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_GOURAUD = 0x0080;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_PHONG = 0x0100;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_FASTPHONG = 0x0100;// (alias)
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_TEXTURE = 0x0200;

	// 多边形states
	enum  PloygonStates
	{
		kInit = 0,
		kActive = 0x0001,//活跃
		kClipped = 0x0002,//裁剪
		kBackface = 0x0004//背面
	};
	enum class TransfromType
	{
		kLocalOnly,
		kTransOnly,
		kLocalToTrans
	};

	/**************************多边形***************************/
	class Polygon
	{
	public:
		Polygon(){}
		~Polygon() {}
		PloygonStates state{ PloygonStates::kInit };//状态
		int attr{ 0 };//物理属性
		int color{ 0 };//颜色
		Point4D *vlist{};//顶点列表
		int vert[3];//顶点列表中元素索引
	};

	/***********************自包含多边形******************************/
	class PolygonFull
	{
	public:
		PolygonFull() {}
		~PolygonFull() {}
		PloygonStates state{ PloygonStates::kInit };//状态
		int attr{ 0 };
		int color{ 0 };

		Point4D vlist[3];//三角形顶点
		Point4D tvlist[3];//变换后顶点

		Polygon *next{};
		Polygon *prev{};
		
	};

	/*************************物体************************************/
	class Object
	{
	public:
		Object() = default;
		~Object() {}
		int id{ 0 };//物体标识
		std::string name;//名称
		int state{ 0 };//状态
		int attr{ 0 };//属性
		float avgRadius{ 0 };//物体平均半径
		float maxRadius{ 0 };//最大半径
		Vector4D worldPos;//世界坐标
		Vector4D dir;//物体在局部坐标系的旋转角度
		Vector4D ux, uy, uz;//物体局部坐标，用于存储物体的的朝向，旋转期间将被自动更新
		int numVertices{ 0 };//物体顶点数
		Point4D vlistLocal[MAX_VERTICES];//用于存储顶点局部坐标的数组
		Point4D vlistTransl[MAX_VERTICES];//存储变换后的顶点坐标数组
		int numPolygons{ 0 };//物体网格多边形数
		Polygon plist[MAX_POLYS];//多边形数组
		//对物体的顶点列表进行变换	//@parm obj需要变换的物体，mat 变换矩阵 ，type 指定要变换的列表 isChange 是否对朝向向量变换
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

			//是否变换物体朝向
			if (isChange) {
				ux = ux * mat;
				uy = uy * mat;
				uz = uz * mat;
			}
		}
		// 将物体从局部坐标转换成世界坐标
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


	/************************主多边形列表*************************************/
	class RenderList {
	public:
		RenderList(){}
		~RenderList() {}
		void Reset() {
			numPolys = 0;
		}
		int state{ 0 };//状态
		int attr{ 0 };//属性
		//渲染列表是一个指针数组，其中每个指针指向一个自包含的、可渲染的多边形
		//如需要根据z值排序时，修改的是渲染列表
		PolygonFull* polyPtrs[MAX_RENDER_POLYGON];
		//多边形存放数组，为避免每帧都为多边形分配/释放内存
		PolygonFull polyData[MAX_RENDER_POLYGON];
		int numPolys{ 0 };//渲染列表中包含的多边形数目
		//对渲染列表的多边形变换	//@parm rederList 渲染列表，mat 变换矩阵 ，type 指定要变换的坐标
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
		//转换成世界坐标
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

	/**********************************相机**********************************/
	class camera {
	public:
		int state{ 0 };
		int attr{ 0 };
		Point4D pos;//相机在世界坐标中的位置
		Vector4D dir;//欧拉角度或UVN相机模型的注视方向

		Vector4D u{ 1.0,0.0,0.0,1.0 };//UVN相机模型的朝向
		Vector4D v{ 0.0,1.0,0.0,1.0 };
		Vector4D n{ 0.0,0.0,1.0,1.0 };
		Point4D target;//UVN模型目标的位置

		float viewDist{ 0.0 };//焦距
		float viewDistH{ 0.0 };//水平视距/垂直视距
		float viewDistV{ 0.0 };

		float fov{ 0.0 };//水平方向/垂直方向的视野

		//3d裁剪面，如果视野不是90°，3d裁剪面将为一般性明平面方程
		float nearClipZ{ 0.0 };//近裁剪面
		float farClipZ{ 0.0 };//远裁剪面

		Plane3D rtClipPlane;//右裁剪面
		Plane3D ltClipPlane;//左裁剪面
		Plane3D tpClipPlane;//上裁剪面
		Plane3D btClipPlane;//下裁剪面

		float viewPlaneWidth{ 0.0 };//视平面宽度/高度
		float viewPlaneHeight{ 0.0 };//对于归一化投影，为2x2，否则大小与视口或屏幕窗口相同

		//屏幕和视口是同义词

		float viewPortWidth{ 0.0 };//屏幕大小
		float viewPortHeight{ 0.0 };//屏幕大小
		float viewPortCenterX{ 0.0 };//屏幕中心
		float viewPortCenterY{ 0.0 };

		float aspectRatio{ 0.0 };//屏幕宽高比

		Matrix<float, 4, 4> mcam;//世界坐标到相机坐标变换矩阵
		Matrix<float, 4, 4> mper;//相机坐标到透视坐标变换矩阵
		Matrix<float, 4, 4> mscr;//透视坐标到屏幕坐标变换矩阵
		//初始化相机 @parame fov：视野，单位°
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

			// 单位矩阵
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

			//视平面大小为 2 x (2/ar)
			viewPlaneWidth = 2.0;
			viewPlaneHeight = 2.0 / aspectRatio;
		
			// now we know fov and we know the viewplane dimensions plug into formula and
			// solve for view distance parameters
			float tan_fov_div2 = tan(DegToRad(fov / 2));

			viewDist = (0.5)*(viewPlaneWidth)*tan_fov_div2;

			if (fabs(fov - 90.0f) < EPSILON_E5)
			{
				Point3D ptOri;	

				Vector3D vn{1,0,-1}; // 面法线
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

	/****************************************打印gameobject*****************************/
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