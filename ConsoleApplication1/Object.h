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
	enum  class CameraType
	{
		kModeEuler,
		kModeUvn,
	};

	enum class CameraRotSeq
	{//旋转顺序
		kSeqXYZ,
		kSeqYXZ,
		kSeqXZY,
		kSeqYZX,
		kSeqZYX,
		kSeqZXY,
	};
	enum class CameraUvnMode
	{//Uvn 相机模型
		kSimple,//简单模型，使用目标位置和观察参考点
		kSpherical//球面坐标模式，分量x/y作为观察向量的方位角和仰角
	};
	class Camera {
	public:
		int state{ 0 };
		CameraType attr{ CameraType::kModeUvn };
		Point4D pos;//相机在世界坐标中的位置
		Vector4D dir;//欧拉角度或UVN相机模型的注视方向

		Vector4D u{ 1.0,0.0,0.0,1.0 };//UVN相机模型的朝向
		Vector4D v{ 0.0,1.0,0.0,1.0 };
		Vector4D n{ 0.0,0.0,1.0,1.0 };
		Point4D target;//UVN模型目标的位置

		float viewDist{ 0.0 };//视距
		float viewDistH{ 0.0 };//水平视距/垂直视距（透视变换中使用）
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
		void InitCamera(CameraType camAttr, const Point4D &camPos, const Vector4D &camdir, const Point4D &camTarget,
			float nearClipZ, float farClipZ, float fov, float viewportWidth, float viewportHeight) {
			attr = camAttr;
			pos = camPos;
			dir = camdir;

			target = camTarget;
			nearClipZ = nearClipZ;
			farClipZ = farClipZ;

			viewPortWidth = viewportWidth;   // 视口大小
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
			float tan_fov_div2 = 1 / tan(DegToRad(fov / 2));

			viewDist = (0.5)*(viewPlaneWidth)*tan_fov_div2;

			if (fabs(fov - 90.0f) < EPSILON_E5)
			{
				Point3D ptOri;

				Vector3D vn{ 1.f,0.f,-1.f }; // 面法线
				rtClipPlane = Plane3D(ptOri, vn, true);

				vn = Vector3D{ -1.f,0.f,-1.f };
				ltClipPlane = Plane3D(ptOri, vn, true);

				vn = Vector3D{ 0.f, 1.f, -1.f };
				tpClipPlane = Plane3D(ptOri, vn, true);

				vn = Vector3D{ 0.f, -1.f, -1.f };
				btClipPlane = Plane3D(ptOri, vn, true);
			}
			else
			{
				Point3D ptOrigin;//平面上的一个点
				Vector3D vn; // 面法线

				//右裁剪面
				vn = Vector3D{ viewDist,0.0f,-viewPlaneWidth / 2.0f };
				rtClipPlane = Plane3D{ ptOrigin ,vn,true };

				// 左裁剪面，与右裁剪面关于z轴对称
				vn = Vector3D{ -viewDist,0.0,-viewPlaneWidth / 2.0f };
				ltClipPlane = Plane3D{ ptOrigin ,vn,true };

				// 上裁剪面
				vn = Vector3D{ 0.0f,viewDist,-viewPlaneWidth / 2.0f };
				tpClipPlane = Plane3D{ ptOrigin ,vn,true };

				//下裁剪面
				vn = Vector3D{ 0.0f,-viewDist,-viewPlaneWidth / 2.0f };
				btClipPlane = Plane3D{ ptOrigin ,vn,true };
			}

		} // end Init_CAM4DV1

		//根据欧拉角度计算相机变换矩阵，seq确认旋转顺序（世界坐标到相机坐标变换矩阵）
		void BuildMatrixEuler(CameraRotSeq seq)
		{
			//相机平移矩阵逆矩阵
			Matrix<float, 4, 4> mtInv{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				-pos.x, -pos.y, -pos.z, 1 };

			Matrix<float, 4, 4>	mtmp;    // 所有逆旋转矩阵积

			//提取欧拉角度
			float theta_x = dir.x;
			float theta_y = dir.y;
			float theta_z = dir.z;

			// compute the sine and cosine of the angle x
			float cos_theta = cosf(theta_x);  // no change since cos(-x) = cos(x)
			float sin_theta = -sinf(theta_x); // sin(-x) = -sin(x)
			// 绕x轴旋转逆矩阵 
			Matrix<float, 4, 4>	mxInv{
				1, 0, 0, 0,
				0, cos_theta, sin_theta, 0,
				0, -sin_theta, cos_theta, 0,
				0, 0, 0, 1 };

			// compute the sine and cosine of the angle y
			cos_theta = cosf(theta_y);  // no change since cos(-x) = cos(x)
			sin_theta = -sinf(theta_y); // sin(-x) = -sin(x)
			//绕y轴旋转逆矩阵
			Matrix<float, 4, 4> myInv{
				cos_theta, 0, -sin_theta, 0,
				0, 1, 0, 0,
				sin_theta, 0, cos_theta, 0,
				0, 0, 0, 1 };

			// compute the sine and cosine of the angle z
			cos_theta = cosf(theta_z);  // no change since cos(-x) = cos(x)
			sin_theta = -sinf(theta_z); // sin(-x) = -sin(x)
			// 绕z轴旋转逆矩阵
			Matrix<float, 4, 4> mzInv{ cos_theta, sin_theta, 0, 0,
				-sin_theta, cos_theta, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1 };

			// now compute inverse camera rotation sequence
			switch (seq)
			{
			case CameraRotSeq::kSeqXYZ:
				mtmp = mxInv * myInv * mzInv;
				break;
			case CameraRotSeq::kSeqXZY:
				mtmp = mxInv * mzInv * myInv;
				break;
			case CameraRotSeq::kSeqYXZ:
				mtmp = myInv * mxInv * mzInv;
				break;
			case CameraRotSeq::kSeqYZX:
				mtmp = myInv * mzInv * mxInv;
				break;

			case CameraRotSeq::kSeqZXY:
				mtmp = mzInv * mxInv * myInv;
				break;

			case CameraRotSeq::kSeqZYX:
				mtmp = mzInv * myInv * mxInv;
				break;

			default: break;
			}
			// 将平移矩阵乘以旋转矩阵，得到最终变换矩阵
			mcam = mtInv * mtmp;
		}

		//根据注视向量n,v,u创建相机变换矩阵
		void BuildCameraMatrixUVN(CameraUvnMode mode) {
			// step 1:平移逆矩阵
			Matrix<float, 4, 4> mtInv{
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					-pos.x, -pos.y, -pos.z, 1 };

			// step 2: 球面模型，计算目标点
			if (mode == CameraUvnMode::kSpherical)
			{
				float phi = DegToRad(dir.x); // elevation
				float theta = DegToRad( dir.y); // heading

				// compute trig functions once
				float sin_phi = sinf(phi);
				float cos_phi = cosf(phi);

				float sin_theta = sinf(theta);
				float cos_theta = cosf(theta);

				// now compute the target point on a unit sphere x,y,z
				target.x = -1 * sin_phi*sin_theta;
				target.y = 1 * cos_phi;
				target.z = 1 * sin_phi*cos_theta;
			} // end else

		 // 计算 u,v,n
		 // Step 1: n = <target position - view reference point>
			n = target - pos;

			// Step 2: Let v = <0,1,0>
			v = Vector4D{ 0.f, 1.f, 0.f ,1.f };

			// Step 3: u = (v x n)
			u = v.Cross(n);

			// Step 4: v = (n x u)
			v = n.Cross(u);

			// Step 5: 归一化
			u.Normalize();
			v.Normalize();
			n.Normalize();

			Matrix<float, 4, 4> mtUvn = {
				u.x, v.x, n.x, 0,
				u.y, v.y, n.y, 0,
				u.z, v.z,n.z, 0,
				0, 0, 0, 1 };
			// 相机变换矩阵
			mcam = mtInv * mtUvn;
		}
	};
	std::ostream & operator <<(std::ostream &out, const Object &obj);
	bool operator ==(const Object&a, const Object&b);
	void WorldToCamera(Object &obj, Camera &cam);
	void WorldToCamera(RenderList &rdList, Camera &cam);
	bool CullObject(Object &obj, Camera &cam, CullType cullflags);
	void ResetOjbectState(Object &obj);
}