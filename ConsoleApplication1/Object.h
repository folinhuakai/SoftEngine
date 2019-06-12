#pragma once
#include "Camera.h"
#include <string>
#include "Math.h"

namespace maki{
	constexpr auto MAX_VERTICES = 128; //物体最大顶点数
	constexpr auto MAX_POLYS = 128;//物体最大多边形数
	constexpr auto MAX_RENDER_POLYGON = 128;//渲染列表最大多边形数
	

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
	enum  ObjectState
	{
		kActiveS	= 0x0001,
		kVisible	= 0x0002,
		kCull		= 0x0004,	
	};


	enum CullType {
		kCullXPlane = 0x0001,
		kCullYPlane = 0x0002,
		kCullZPlane = 0x0004,
		kCullXYZPlane = 0x0007,
	};
	/**************************多边形***************************/
	class Polygon
	{
	public:
		Polygon(){}
		~Polygon() {}
		int state{ PloygonStates::kInit };//状态
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
		int state{ PloygonStates::kInit };//状态
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
		// 将物体从局部坐标转换成世界坐标(直接法)
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
		// 矩阵法
		void TransfromToWorldMat(TransfromType type = TransfromType::kLocalToTrans,bool isChange = true) {
			Matrix<float, 4, 4> mat = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				worldPos.x,worldPos.y,worldPos.z,1 };
			TransfromObject(mat, TransfromType::kLocalToTrans, isChange);
		}
		//世界坐标到相机坐标
		void WorldToCamera(Camera &cam)
		{//对object中的vlistTrans的顶点进行变换，假设物体的顶点已变换为世界坐标，并结果存在vlistTrans
			for (int vertex = 0; vertex < numVertices; ++vertex)
			{
				vlistTransl[vertex] = vlistTransl[vertex] * cam.mcam;
			}
		}
		//相机坐标到屏幕坐标变换
		void CameraToScreen(Camera &cam) {

			float alpha = (0.5*cam.viewPortWidth - 0.5);
			float beta = (0.5*cam.viewPortHeight - 0.5);

			for (int vertex = 0; vertex < numVertices; ++vertex) {
				float z = vlistTransl[vertex].z;

				// transform the vertex by the view parameters in the camera
				vlistTransl[vertex].x = cam.viewDist * vlistTransl[vertex].x / z;
				vlistTransl[vertex].y = cam.viewDist * vlistTransl[vertex].y*cam.aspectRatio / z;
				// z = z, so no change

				vlistTransl[vertex].x = alpha + alpha * vlistTransl[vertex].x;
				vlistTransl[vertex].y = beta - beta * vlistTransl[vertex].y;
			} // end for vertex

		} // end Camera_To_P

		//透视变换
		void CameraToPerspective(Camera &cam) {
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				float z = vlistTransl[vertex].z;

				// transform the vertex by the view parameters in the camera
				vlistTransl[vertex].x = cam.viewDist * vlistTransl[vertex].x / z;
				vlistTransl[vertex].y = cam.viewDist * vlistTransl[vertex].y * cam.aspectRatio / z;
			} // end for vertex
		}

		//视口变换
		void PerspectiveToScreen(Camera &cam) {
			float alpha = (0.5*cam.viewPortWidth - 0.5);
			float beta = (0.5*cam.viewPortHeight - 0.5);

			for (int vertex = 0; vertex < numVertices; ++vertex) {

				// transform the vertex by the view parameters in the camera
				vlistTransl[vertex].x = alpha + alpha * vlistTransl[vertex].x;
				vlistTransl[vertex].y = beta - beta * vlistTransl[vertex].y;

			} // end for vertex
		}

		// 物体剔除
		bool CullObject(Camera &cam, CullType cullflags) {//根据相机信息判断物体是否在视景体内，cullflags指定在哪些轴上执行剔除
		//1.将包围球球心变换到相机坐标
			auto spherePos = worldPos * cam.mcam;

			// 2: 根据cullflags剔除物体
			if (cullflags & CullType::kCullZPlane) {//远近裁剪面
				if (((spherePos.z - maxRadius) > cam.farClipZ) ||
					((spherePos.z + maxRadius) < cam.nearClipZ)) {
					state = state | ObjectState::kCull;
					return false;
				}
			} // end if

			if (cullflags & CullType::kCullXPlane) {//左右裁剪面（使用三角相似）

				float z_test = (0.5)*cam.viewPlaneWidth*spherePos.z / cam.viewDist;

				if (((spherePos.x - maxRadius) > z_test) || // right side
					((spherePos.x + maxRadius) < -z_test))  // left side, note sign change
				{
					state = state | ObjectState::kCull;
					return false;
				} // end if
			} // end if

			if (cullflags & CullType::kCullYPlane) {//上下裁剪面（使用三角相似）
				float z_test = (0.5)*cam.viewPlaneHeight *spherePos.z / cam.viewDist;
				if (((spherePos.y - maxRadius) > z_test) || // top side
					((spherePos.y + maxRadius) < -z_test))  // bottom side, note sign change
				{
					state = state | ObjectState::kCull;
					return false;
				} // end if

			} // end if

			return true;
		}
		// 背面消除
		void RemoveBackfaces(Camera cam) {
			//根据vlistTrans中顶点数据及相机位置消除物体的背面多边形（面法线与视点间的向量点乘，<90°即背面）
			if (state & ObjectState::kCull)
				return;

			// 处理每个多边形
			for (int poly = 0; poly < numPolygons; ++poly)
			{
				// acquire polygon
				auto currPoly = plist[poly];
				// 判断多边形有效性
				if (!(currPoly.state & PloygonStates::kActive) ||
					(currPoly.state & PloygonStates::kBackface) ||
					(currPoly.attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly.state & PloygonStates::kClipped))
					continue; // move onto next poly

				 // 获得顶点下标（不是自包含）
				int vindex_0 = currPoly.vert[0];
				int vindex_1 = currPoly.vert[1];
				int vindex_2 = currPoly.vert[2];

				//计算多边形面法线，顶点是按顺时针方向排列
				auto u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
				auto v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2
				auto n = u.Cross(v);//u x v

				auto view = cam.pos - vlistTransl[vindex_0];//视点指向多边形向量

				// 计算点积
				float dp = n * view;

				// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
				if (dp <= 0.0) {
					currPoly.state = currPoly.state & PloygonStates::kBackface;
				}

			} // end for poly

		} // end

		//清除物体（裁剪和背面）状态标识
		void ResetOjbectState() {
			//清除物体剔除状态
			state = state & (!ObjectState::kCull);

			// 重置多边形裁剪和背面剔除标记
			for (int poly = 0; poly < numPolygons; poly++)
			{
				auto curPoly = plist[poly];
				if (!(curPoly.state &PloygonStates::kActive)) {
					continue;
				}
				curPoly.state = curPoly.state & (!PloygonStates::kClipped);
				curPoly.state = curPoly.state & (!PloygonStates::kBackface);

			}
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
	    
		//世界坐标-》相机坐标
		void WorldToCamera(Camera &cam)
		{//根据相机变换矩阵将渲染列表中的多边形换为相机坐标
			for (int i = 0; i < numPolys; ++i) {
				auto curPoly = polyPtrs[i];
				if (curPoly == nullptr ||
					!(curPoly->state &PloygonStates::kActive) ||
					curPoly->state &PloygonStates::kClipped ||
					curPoly->state &PloygonStates::kBackface) {
					continue;
				}
				for (int vetNum = 0; vetNum < 3; ++vetNum) {
					curPoly->tvlist[vetNum] = curPoly->vlist[vetNum] * cam.mcam;
				}
			}
		}
		
		//透视变换(非矩阵法，这里假设渲染列表的多边形已被变换为相机坐标)
		void CameraToPerspective(Camera &cam) {
			for (int poly = 0; poly < numPolys; poly++)
			{
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

			 // all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++)
				{
					float z = currPoly->tvlist[vertex].z;

					// transform the vertex by the view parameters in the camera
					currPoly->tvlist[vertex].x = cam.viewDist*currPoly->tvlist[vertex].x / z;
					currPoly->tvlist[vertex].y = cam.viewDist*currPoly->tvlist[vertex].y*cam.aspectRatio / z;

				} // end for vertex

			} // end for poly

		} // end CameraToPerspective

		//视口变换
		void PerspectiveToScreen(Camera &cam) {
			//渲染列表已完成透视变换并归一化，透视坐标-》屏幕坐标
			for (int poly = 0; poly < numPolys; poly++){
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				float alpha = (0.5*cam.viewPortWidth - 0.5);
				float beta = (0.5*cam.viewPortHeight - 0.5);

				// all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++)
				{//坐标缩放，反转y轴
					currPoly->tvlist[vertex].x = alpha + alpha * currPoly->tvlist[vertex].x;
					currPoly->tvlist[vertex].y = beta - beta * currPoly->tvlist[vertex].y;
				} // end for vertex

			} // end for poly	
		}
		
		  //相机坐标到屏幕坐标转换（直接法）
		void CameraToScreen(Camera &cam) {
			for (int poly = 0; poly < numPolys; poly++){
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				float alpha = (0.5*cam.viewPortWidth - 0.5);
				float beta = (0.5*cam.viewPortHeight - 0.5);

				// all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++){
					float z = currPoly->tvlist[vertex].z;

					// transform the vertex by the view parameters in the camera
					currPoly->tvlist[vertex].x = cam.viewDist*currPoly->tvlist[vertex].x / z;
					currPoly->tvlist[vertex].y = cam.viewDist*currPoly->tvlist[vertex].y*cam.aspectRatio / z;

					//坐标缩放，反转y轴
					currPoly->tvlist[vertex].x = alpha + alpha * currPoly->tvlist[vertex].x;
					currPoly->tvlist[vertex].y = beta - beta * currPoly->tvlist[vertex].y;

				} // end for vertex

			} // end for poly

		} // end 
		
		//背面消除
		void RemoveBackfaces(Camera cam) {
			// 设置多边形背面状态
			for (int poly = 0; poly < numPolys; ++poly)
			{
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				//计算多边形面法线，顶点是按顺时针方向排列
				auto u = currPoly->tvlist[1] - currPoly->tvlist[0];//p0->p1
				auto v = currPoly->tvlist[2] - currPoly->tvlist[0];//p0->p2
				auto n = u.Cross(v);//u x v

				auto view = cam.pos - currPoly->tvlist[0];//视点指向多边形向量

				// 计算点积
				float dp = n * view;

				// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
				if (dp <= 0.0) {
					currPoly->state = currPoly->state & PloygonStates::kBackface;
				}

			} // end for poly

		} // end RemoveBackfacesRenderlist
		
		//从齐次坐标转为非齐次坐标
		void ConvertFromHomo4D() {
			for (int poly = 0; poly < numPolys; poly++)
			{
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

			 // all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++)
				{
					currPoly->tvlist[vertex].DivByW();
				} // end for vertex

			} // end for poly		

		}



	};
	
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