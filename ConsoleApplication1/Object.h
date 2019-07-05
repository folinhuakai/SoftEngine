#pragma once
#include "Camera.h"
#include <string>
#include "Math.h"
#include "Light.h"
#include <algorithm>
#include<vector>
namespace maki{		
	constexpr auto MAX_VERTICES = 128; //物体最大顶点数
	constexpr auto MAX_POLYS = 128;//物体最大多边形数
	constexpr auto MAX_RENDER_POLYGON = 128;//渲染列表最大多边形数
#define RGB_FROM32BIT(RGB, r,g,b) { *r = ( ((RGB) >> 24) & 0xFF); *g = (((RGB) >> 16) & 0xFF); *b = ( ((RGB) >> 8) & 0xFF);}
#define RGBA32BIT(r,g,b,a) ((a) + ((b) << 8) + ((g) << 16) + ((r) << 24))
	
	//多边形 attr
	constexpr auto POLY4DV1_ATTR_2SIDED = 0x0001;
	constexpr auto POLY4DV1_ATTR_TRANSPARENT = 0x0002;
	constexpr auto POLY4DV1_ATTR_8BITCOLOR = 0x0004;
	constexpr auto POLY4DV1_ATTR_RGB16 = 0x0008;
	constexpr auto POLY4DV1_ATTR_RGB24 = 0x0010;

	enum class SortType {
		kAvgZ,
		kMinZ,
		kMaxZ,
	};

	//着色模式
	enum PloygonShadeMode {
		kPuer		= 0x0020,//固定着色
		kFlat		= 0x0040,//恒定着色
		kGouraud	= 0x0080,
		kPhong		= 0x0100,
		kTexture	= 0x0200,
	};

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
	
	/************************顶点*******************************/
	class Vertex {
	public:
		Vertex(){}
		Vertex(float x = 0.0f, float y = 0.0f, float z = 0.0f, unsigned char r = 255, unsigned char g = 255, unsigned char b = 255)
		{
			pos = Vector4D(x, y, z,1);
			color = Color(r, g, b);
		}
		Vector4D pos;
		Vector4D normal;
		Point2D uv;
		Color color;
	};

	/************************************************************/
	class Mesh{
	public:
		//原始顶点列表
		std::vector<Vertex> vlistLocal;
		//变换顶点列表
		std::vector<Vertex> vlistTtran;
		//面法线
		std::vector<Vector4D> faceNormal;
		//索引数组(连续的3个顶点构成三角面)
		std::vector<unsigned int> index;
		//静态or动态
		bool isStatic = false;
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
		int tColor{ 0 };//颜色
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
		//Point4D vlistLocal[MAX_VERTICES];//用于存储顶点局部坐标的数组
		//Point4D vlistTransl[MAX_VERTICES];//存储变换后的顶点坐标数组
		Mesh mesh;
		//int numPolygons{ 0 };//物体网格多边形数
		//Polygon plist[MAX_POLYS];//多边形数组
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

		//光照计算
		bool LightInWorld(Camera &cam,
			const std::vector<Light> lights){
			//根据光源列表和相机对物体执行光照计算，支持固定着色和恒定着色
			unsigned int r_base, g_base, b_base,  // 初始颜色
				r_sum, g_sum, b_sum,   // 全部光照
				shaded_color;          // 最终颜色

			float dp,     // 点积
				dist,   // 表面和光源距离
				i,      // 强度
				nl,     // 法线长度
				atten;  // 衰减计算结果

		  // test if the object is culled
			if (!(state & ObjectState::kActiveS) ||
				(state & ObjectState::kCull) ||
				!(state & ObjectState::kVisible))
				return false;

			// process each poly in mesh
			for (int poly = 0; poly < numPolygons; poly++){
				// acquire polygon
				auto currPoly = plist[poly];

				// 判断多边形有效性
				if (!(currPoly.state & PloygonStates::kActive) ||
					(currPoly.state & PloygonStates::kBackface) ||
					(currPoly.state & PloygonStates::kClipped))
					continue; // move onto next poly

				 // 获得顶点下标（不是自包含）
				int vindex_0 = currPoly.vert[0];
				int vindex_1 = currPoly.vert[1];
				int vindex_2 = currPoly.vert[2];

				// test the lighting mode of the polygon (use flat for flat, gouraud))
				if (currPoly.attr & PloygonShadeMode::kFlat || currPoly.attr & PloygonShadeMode::kGouraud){//恒定着色、Gouraud
					RGB_FROM32BIT(currPoly.color, &r_base, &g_base, &b_base);
					// initialize color sum
					r_sum = 0;
					g_sum = 0;
					b_sum = 0;				

					// loop thru lights
					for (int currLight = 0; currLight < lights.size(); ++currLight)
					{
						// is this light active
						if (lights[currLight].state == LightState::kOff)
							continue;

						// what kind of light are we dealing with
						if (lights[currLight].attr & LightType::kAmbient){
							//环境光
							r_sum += ((lights[currLight].c_ambient.r * r_base) / 256);
							g_sum += ((lights[currLight].c_ambient.g * g_base) / 256);
							b_sum += ((lights[currLight].c_ambient.b * b_base) / 256);
						} // end if
						else if (lights[currLight].attr & LightType::kInfinite) {
							//无穷远光源,需计算面法线和光源方向
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//计算面法线长度,用于法向量归一化
							nl =n.Length();

							dp = n * lights[currLight].dir;//如果将面法线归一化后，dp即为余弦值

							// only add light if dp > 0
							if (dp > 0)
							{
								i = 128 * dp / nl;//乘128.避免浮点数计算？
								r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);//加上散射光
								g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
								b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);
							} // end if

						}
						else if(lights[currLight].attr & LightType::kPoint){
							// 点光源,除光强度随距离衰减外，其他与无穷远光源相似
							//              I0point * Clpoint
							//  I(d)point = ___________________
							//              kc +  kl*d + kq*d2        

							// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//计算面法线长度,用于法向量归一化
							nl = n.Length();
							// 顶点到光源向量
							Vector4D l = lights[currLight].pos - vlistTransl[vindex_0];
							dist = l.Length();

							// and for the diffuse model
							dp = n * l;

							// only add light if dp > 0
							if (dp > 0)
							{
								atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);

								i = 128 * dp / (nl * dist * atten);

								r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);
								g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
								b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);
							} // end if

						} // end if point
						else if (lights[currLight].attr & LightType::kSpotLight1){
							// 简单版聚光灯，与点光源相同
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//计算面法线长度,用于法向量归一化
							nl = n.Length();
							// 顶点到光源向量
							Vector4D l = lights[currLight].pos - vlistTransl[vindex_0];
							dist = l.Length();

							// and for the diffuse model
							dp = n * l;

							// only add light if dp > 0
							if (dp > 0)
							{
								atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);

								i = 128 * dp / (nl * dist * atten);

								r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);
								g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
								b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);
							} // end if
						} // end if spotlight1
						else if (lights[currLight].attr & LightType::kSpotLight2){
							//         	     I0spotlight * Clspotlight * MAX( (l . s), 0)^pf                     
							// I(d)spotlight = __________________________________________      
							//               		 kc + kl*d + kq*d2        
							// Where d = |p - s|, and pf = power factor
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							nl = n.Length();
							dp = n * lights[currLight].dir;

							// only add light if dp > 0
							if (dp > 0){
								// 顶点到光源向量
								Vector4D s = vlistTransl[vindex_0]  - lights[currLight].pos;
								dist = s.Length();

								// 余弦值
								float dpsl = s * lights[currLight].dir / dist;

								// proceed only if term is positive
								if (dpsl > 0)
								{
									// compute attenuation
									atten = (lights[currLight].kc + lights[currLight].kl*dist + lights[currLight].kq*dist*dist);
									float dpsl_exp = dpsl;

									// exponentiate for positive integral powers
									for (int e_index = 1; e_index < (int)lights[currLight].pf; e_index++)
										dpsl_exp *= dpsl;

									// now dpsl_exp holds (dpsl)^pf power which is of course (s . l)^pf 

									i = 128 * dp * dpsl_exp / (nl * atten);

									r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);
									g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
									b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);

								} 

							} 

						}									

					} // end for light

				// 确保颜色不会溢出
					if (r_sum > 255) r_sum = 255;
					if (g_sum > 255) g_sum = 255;
					if (b_sum > 255) b_sum = 255;

					// write the color
					shaded_color = RGBA32BIT(r_sum, g_sum, b_sum,255);
					currPoly.tColor = shaded_color;

				} // end if
				else {
				// 固定着色
				currPoly.tColor = currPoly.color;
				} // end if

			} // end for poly

		// return success
			return true;

		} // end 

		//通过数学公式缩放(仅支持同等比例缩放)
		void ScaleMath(const float scale) {
			for (int i = 0; i < mesh.vlistLocal.size(); ++i) {
				mesh.vlistLocal[i].pos = mesh.vlistLocal[i].pos * scale;
			}
			return;
		}
		//计算物体平均半径和最大半径
		float ComputeObjectRadius()
		{
			// this function computes the average and maximum radius for 
			// sent object and opdates the object data

			// reset incase there's any residue
			avgRadius = 0;
			maxRadius = 0;

			// loop thru and compute radius
			for (int vertex = 0; vertex < numVertices; vertex++)
			{
				// update the average and maximum radius
				float dist_to_vertex =
					sqrt(vlistLocal[vertex].x*vlistLocal[vertex].x +
						vlistLocal[vertex].y*vlistLocal[vertex].y +
						vlistLocal[vertex].z*vlistLocal[vertex].z);

				// accumulate total radius
				avgRadius += dist_to_vertex;

				// update maximum radius   
				if (dist_to_vertex > maxRadius)
					maxRadius = dist_to_vertex;

			} // end for vertex

			// finallize average radius computation
			avgRadius /= numVertices;

		}
	};


	/************************主多边形列表*************************************/
	// 根据z最小值排序
	inline bool CompareNearZ(const PolygonFull *arg1, const PolygonFull * arg2) {
		float z1, z2;
		// dereference the poly pointers
		auto poly_1 = *arg1;
		auto poly_2 = *arg2;

		// compute the near z of each polygon
		z1 = std::fmin(poly_1.tvlist[0].z, poly_1.tvlist[1].z);
		z1 = std::fmin(z1, poly_1.tvlist[2].z);

		z2 = std::fmin(poly_2.tvlist[0].z, poly_2.tvlist[1].z);
		z2 = std::fmin(z2, poly_2.tvlist[2].z);

		// compare z1 and z2, such that polys' will be sorted in descending Z order
		return z1 > z2;
	}
	// 根据z最大值排序
	inline bool CompareFarZ(const PolygonFull *arg1, const PolygonFull * arg2) {
		float z1, z2;
		// dereference the poly pointers
		auto poly_1 = *arg1;
		auto poly_2 = *arg2;

		// compute the near z of each polygon
		z1 = std::fmax(poly_1.tvlist[0].z, poly_1.tvlist[1].z);
		z1 = std::fmax(z1, poly_1.tvlist[2].z);

		z2 = std::fmax(poly_2.tvlist[0].z, poly_2.tvlist[1].z);
		z2 = std::fmax(z2, poly_2.tvlist[2].z);

		// compare z1 and z2, such that polys' will be sorted in descending Z order
		return z1 > z2;
	}
	// 根据z平均值排序
	inline bool CompareAvgZ(const PolygonFull *arg1, const PolygonFull * arg2) {
		float z1, z2;
		// dereference the poly pointers
		auto poly_1 = *arg1;
		auto poly_2 = *arg2;

		// compute the near z of each polygon
		z1 = (poly_1.tvlist[0].z + poly_1.tvlist[1].z + poly_1.tvlist[2].z) / 3;
		z2 = (poly_2.tvlist[0].z + poly_2.tvlist[1].z + poly_2.tvlist[2].z) / 3;

		// compare z1 and z2, such that polys' will be sorted in descending Z order
		return z1 > z2;
	}

	class RenderList {
	public:
		RenderList(){}
		~RenderList() {}
		void Reset() {
			polyPtrs.clear();
		}
		int state{ 0 };//状态
		int attr{ 0 };//属性
		//渲染列表是一个指针数组，其中每个指针指向一个自包含的、可渲染的多边形
		//如需要根据z值排序时，修改的是渲染列表
		std::vector<PolygonFull *> polyPtrs;
		//多边形存放数组，为避免每帧都为多边形分配/释放内存
		std::vector<PolygonFull> polyData;	
		
		//对渲染列表的多边形变换	//@parm rederList 渲染列表，mat 变换矩阵 ，type 指定要变换的坐标
		void TransformRenderList(Matrix<float, 4, 4> &mat, TransfromType type) {
			switch (type) {
			case TransfromType::kLocalOnly:
				for (int i = 0; i < polyPtrs.size(); ++i) {
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
				for (int i = 0; i < polyPtrs.size(); ++i) {
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
				for (int i = 0; i < polyPtrs.size(); ++i) {
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
				for (int i = 0; i < polyPtrs.size(); ++i) {
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
				for (int i = 0; i < polyPtrs.size(); ++i) {
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
			for (int i = 0; i < polyPtrs.size(); ++i) {
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
			for (int poly = 0; poly < polyPtrs.size(); poly++)
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
			for (int poly = 0; poly < polyPtrs.size(); poly++){
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
			for (int poly = 0; poly < polyPtrs.size(); poly++){
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
			for (int poly = 0; poly < polyPtrs.size(); ++poly)
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
			for (int poly = 0; poly < polyPtrs.size(); poly++)
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

		//根据z值对多边形排序
		void SortByZ(SortType type){
			switch (type){
			case SortType::kAvgZ:  //  - sorts on average of all vertices
			{
				std::sort(polyPtrs.begin(), polyPtrs.end(), CompareAvgZ);
			} break;

			case SortType::kMinZ: // - sorts on closest z vertex of each poly
			{
				std::sort(polyPtrs.begin(), polyPtrs.end(), CompareNearZ);
			} break;

			case SortType::kMaxZ:  //  - sorts on farthest z vertex of each poly
			{
				std::sort(polyPtrs.begin(), polyPtrs.end(), CompareFarZ);
			} break;

			default: break;
			} // end switch

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