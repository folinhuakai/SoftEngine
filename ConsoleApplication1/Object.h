#pragma once
#include "Camera.h"
#include <string>
#include "Math.h"
#include "Light.h"
#include <algorithm>
#include<vector>
namespace maki{		
	constexpr auto MAX_VERTICES = 128; //������󶥵���
	constexpr auto MAX_POLYS = 128;//�������������
	constexpr auto MAX_RENDER_POLYGON = 128;//��Ⱦ�б����������
#define RGB_FROM32BIT(RGB, r,g,b) { *r = ( ((RGB) >> 24) & 0xFF); *g = (((RGB) >> 16) & 0xFF); *b = ( ((RGB) >> 8) & 0xFF);}
#define RGBA32BIT(r,g,b,a) ((a) + ((b) << 8) + ((g) << 16) + ((r) << 24))
	
	//����� attr
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

	//��ɫģʽ
	enum PloygonShadeMode {
		kPuer		= 0x0020,//�̶���ɫ
		kFlat		= 0x0040,//�㶨��ɫ
		kGouraud	= 0x0080,
		kPhong		= 0x0100,
		kTexture	= 0x0200,
	};

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
	
	/************************����*******************************/
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
		//ԭʼ�����б�
		std::vector<Vertex> vlistLocal;
		//�任�����б�
		std::vector<Vertex> vlistTtran;
		//�淨��
		std::vector<Vector4D> faceNormal;
		//��������(������3�����㹹��������)
		std::vector<unsigned int> index;
		//��̬or��̬
		bool isStatic = false;
	};
	/**************************�����***************************/
	class Polygon
	{
	public:
		Polygon(){}
		~Polygon() {}
		int state{ PloygonStates::kInit };//״̬
		int attr{ 0 };//��������
		int color{ 0 };//��ɫ
		int tColor{ 0 };//��ɫ
		Point4D *vlist{};//�����б�
		int vert[3];//�����б���Ԫ������
	};

	/***********************�԰��������******************************/
	class PolygonFull
	{
	public:
		PolygonFull() {}
		~PolygonFull() {}
		int state{ PloygonStates::kInit };//״̬
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
		//Point4D vlistLocal[MAX_VERTICES];//���ڴ洢����ֲ����������
		//Point4D vlistTransl[MAX_VERTICES];//�洢�任��Ķ�����������
		Mesh mesh;
		//int numPolygons{ 0 };//��������������
		//Polygon plist[MAX_POLYS];//���������
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
		// ������Ӿֲ�����ת������������(ֱ�ӷ�)
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
		// ����
		void TransfromToWorldMat(TransfromType type = TransfromType::kLocalToTrans,bool isChange = true) {
			Matrix<float, 4, 4> mat = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				worldPos.x,worldPos.y,worldPos.z,1 };
			TransfromObject(mat, TransfromType::kLocalToTrans, isChange);
		}
		//�������굽�������
		void WorldToCamera(Camera &cam)
		{//��object�е�vlistTrans�Ķ�����б任����������Ķ����ѱ任Ϊ�������꣬���������vlistTrans
			for (int vertex = 0; vertex < numVertices; ++vertex)
			{
				vlistTransl[vertex] = vlistTransl[vertex] * cam.mcam;
			}
		}
		//������굽��Ļ����任
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

		//͸�ӱ任
		void CameraToPerspective(Camera &cam) {
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				float z = vlistTransl[vertex].z;

				// transform the vertex by the view parameters in the camera
				vlistTransl[vertex].x = cam.viewDist * vlistTransl[vertex].x / z;
				vlistTransl[vertex].y = cam.viewDist * vlistTransl[vertex].y * cam.aspectRatio / z;
			} // end for vertex
		}

		//�ӿڱ任
		void PerspectiveToScreen(Camera &cam) {
			float alpha = (0.5*cam.viewPortWidth - 0.5);
			float beta = (0.5*cam.viewPortHeight - 0.5);

			for (int vertex = 0; vertex < numVertices; ++vertex) {

				// transform the vertex by the view parameters in the camera
				vlistTransl[vertex].x = alpha + alpha * vlistTransl[vertex].x;
				vlistTransl[vertex].y = beta - beta * vlistTransl[vertex].y;

			} // end for vertex
		}

		// �����޳�
		bool CullObject(Camera &cam, CullType cullflags) {//���������Ϣ�ж������Ƿ����Ӿ����ڣ�cullflagsָ������Щ����ִ���޳�
		//1.����Χ�����ı任���������
			auto spherePos = worldPos * cam.mcam;

			// 2: ����cullflags�޳�����
			if (cullflags & CullType::kCullZPlane) {//Զ���ü���
				if (((spherePos.z - maxRadius) > cam.farClipZ) ||
					((spherePos.z + maxRadius) < cam.nearClipZ)) {
					state = state | ObjectState::kCull;
					return false;
				}
			} // end if

			if (cullflags & CullType::kCullXPlane) {//���Ҳü��棨ʹ���������ƣ�

				float z_test = (0.5)*cam.viewPlaneWidth*spherePos.z / cam.viewDist;

				if (((spherePos.x - maxRadius) > z_test) || // right side
					((spherePos.x + maxRadius) < -z_test))  // left side, note sign change
				{
					state = state | ObjectState::kCull;
					return false;
				} // end if
			} // end if

			if (cullflags & CullType::kCullYPlane) {//���²ü��棨ʹ���������ƣ�
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
		// ��������
		void RemoveBackfaces(Camera cam) {
			//����vlistTrans�ж������ݼ����λ����������ı������Σ��淨�����ӵ���������ˣ�<90�㼴���棩
			if (state & ObjectState::kCull)
				return;

			// ����ÿ�������
			for (int poly = 0; poly < numPolygons; ++poly)
			{
				// acquire polygon
				auto currPoly = plist[poly];
				// �ж϶������Ч��
				if (!(currPoly.state & PloygonStates::kActive) ||
					(currPoly.state & PloygonStates::kBackface) ||
					(currPoly.attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly.state & PloygonStates::kClipped))
					continue; // move onto next poly

				 // ��ö����±꣨�����԰�����
				int vindex_0 = currPoly.vert[0];
				int vindex_1 = currPoly.vert[1];
				int vindex_2 = currPoly.vert[2];

				//���������淨�ߣ������ǰ�˳ʱ�뷽������
				auto u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
				auto v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2
				auto n = u.Cross(v);//u x v

				auto view = cam.pos - vlistTransl[vindex_0];//�ӵ�ָ����������

				// ������
				float dp = n * view;

				// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
				if (dp <= 0.0) {
					currPoly.state = currPoly.state & PloygonStates::kBackface;
				}

			} // end for poly

		} // end

		//������壨�ü��ͱ��棩״̬��ʶ
		void ResetOjbectState() {
			//��������޳�״̬
			state = state & (!ObjectState::kCull);

			// ���ö���βü��ͱ����޳����
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

		//���ռ���
		bool LightInWorld(Camera &cam,
			const std::vector<Light> lights){
			//���ݹ�Դ�б�����������ִ�й��ռ��㣬֧�̶ֹ���ɫ�ͺ㶨��ɫ
			unsigned int r_base, g_base, b_base,  // ��ʼ��ɫ
				r_sum, g_sum, b_sum,   // ȫ������
				shaded_color;          // ������ɫ

			float dp,     // ���
				dist,   // ����͹�Դ����
				i,      // ǿ��
				nl,     // ���߳���
				atten;  // ˥��������

		  // test if the object is culled
			if (!(state & ObjectState::kActiveS) ||
				(state & ObjectState::kCull) ||
				!(state & ObjectState::kVisible))
				return false;

			// process each poly in mesh
			for (int poly = 0; poly < numPolygons; poly++){
				// acquire polygon
				auto currPoly = plist[poly];

				// �ж϶������Ч��
				if (!(currPoly.state & PloygonStates::kActive) ||
					(currPoly.state & PloygonStates::kBackface) ||
					(currPoly.state & PloygonStates::kClipped))
					continue; // move onto next poly

				 // ��ö����±꣨�����԰�����
				int vindex_0 = currPoly.vert[0];
				int vindex_1 = currPoly.vert[1];
				int vindex_2 = currPoly.vert[2];

				// test the lighting mode of the polygon (use flat for flat, gouraud))
				if (currPoly.attr & PloygonShadeMode::kFlat || currPoly.attr & PloygonShadeMode::kGouraud){//�㶨��ɫ��Gouraud
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
							//������
							r_sum += ((lights[currLight].c_ambient.r * r_base) / 256);
							g_sum += ((lights[currLight].c_ambient.g * g_base) / 256);
							b_sum += ((lights[currLight].c_ambient.b * b_base) / 256);
						} // end if
						else if (lights[currLight].attr & LightType::kInfinite) {
							//����Զ��Դ,������淨�ߺ͹�Դ����
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//�����淨�߳���,���ڷ�������һ��
							nl =n.Length();

							dp = n * lights[currLight].dir;//������淨�߹�һ����dp��Ϊ����ֵ

							// only add light if dp > 0
							if (dp > 0)
							{
								i = 128 * dp / nl;//��128.���⸡�������㣿
								r_sum += (lights[currLight].c_diffuse.r * r_base * i) / (256 * 128);//����ɢ���
								g_sum += (lights[currLight].c_diffuse.g * g_base * i) / (256 * 128);
								b_sum += (lights[currLight].c_diffuse.b * b_base * i) / (256 * 128);
							} // end if

						}
						else if(lights[currLight].attr & LightType::kPoint){
							// ���Դ,����ǿ�������˥���⣬����������Զ��Դ����
							//              I0point * Clpoint
							//  I(d)point = ___________________
							//              kc +  kl*d + kq*d2        

							// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//�����淨�߳���,���ڷ�������һ��
							nl = n.Length();
							// ���㵽��Դ����
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
							// �򵥰�۹�ƣ�����Դ��ͬ
							Vector4D u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
							Vector4D v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2;
							Vector4D n = u.Cross(v);

							//�����淨�߳���,���ڷ�������һ��
							nl = n.Length();
							// ���㵽��Դ����
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
								// ���㵽��Դ����
								Vector4D s = vlistTransl[vindex_0]  - lights[currLight].pos;
								dist = s.Length();

								// ����ֵ
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

				// ȷ����ɫ�������
					if (r_sum > 255) r_sum = 255;
					if (g_sum > 255) g_sum = 255;
					if (b_sum > 255) b_sum = 255;

					// write the color
					shaded_color = RGBA32BIT(r_sum, g_sum, b_sum,255);
					currPoly.tColor = shaded_color;

				} // end if
				else {
				// �̶���ɫ
				currPoly.tColor = currPoly.color;
				} // end if

			} // end for poly

		// return success
			return true;

		} // end 

		//ͨ����ѧ��ʽ����(��֧��ͬ�ȱ�������)
		void ScaleMath(const float scale) {
			for (int i = 0; i < mesh.vlistLocal.size(); ++i) {
				mesh.vlistLocal[i].pos = mesh.vlistLocal[i].pos * scale;
			}
			return;
		}
		//��������ƽ���뾶�����뾶
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


	/************************��������б�*************************************/
	// ����z��Сֵ����
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
	// ����z���ֵ����
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
	// ����zƽ��ֵ����
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
		int state{ 0 };//״̬
		int attr{ 0 };//����
		//��Ⱦ�б���һ��ָ�����飬����ÿ��ָ��ָ��һ���԰����ġ�����Ⱦ�Ķ����
		//����Ҫ����zֵ����ʱ���޸ĵ�����Ⱦ�б�
		std::vector<PolygonFull *> polyPtrs;
		//����δ�����飬Ϊ����ÿ֡��Ϊ����η���/�ͷ��ڴ�
		std::vector<PolygonFull> polyData;	
		
		//����Ⱦ�б�Ķ���α任	//@parm rederList ��Ⱦ�б�mat �任���� ��type ָ��Ҫ�任������
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
		
		//ת������������
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
	    
		//��������-���������
		void WorldToCamera(Camera &cam)
		{//��������任������Ⱦ�б��еĶ���λ�Ϊ�������
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
		
		//͸�ӱ任(�Ǿ��󷨣����������Ⱦ�б�Ķ�����ѱ��任Ϊ�������)
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

		//�ӿڱ任
		void PerspectiveToScreen(Camera &cam) {
			//��Ⱦ�б������͸�ӱ任����һ����͸������-����Ļ����
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
				{//�������ţ���תy��
					currPoly->tvlist[vertex].x = alpha + alpha * currPoly->tvlist[vertex].x;
					currPoly->tvlist[vertex].y = beta - beta * currPoly->tvlist[vertex].y;
				} // end for vertex

			} // end for poly	
		}
		
		  //������굽��Ļ����ת����ֱ�ӷ���
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

					//�������ţ���תy��
					currPoly->tvlist[vertex].x = alpha + alpha * currPoly->tvlist[vertex].x;
					currPoly->tvlist[vertex].y = beta - beta * currPoly->tvlist[vertex].y;

				} // end for vertex

			} // end for poly

		} // end 
		
		//��������
		void RemoveBackfaces(Camera cam) {
			// ���ö���α���״̬
			for (int poly = 0; poly < polyPtrs.size(); ++poly)
			{
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				//���������淨�ߣ������ǰ�˳ʱ�뷽������
				auto u = currPoly->tvlist[1] - currPoly->tvlist[0];//p0->p1
				auto v = currPoly->tvlist[2] - currPoly->tvlist[0];//p0->p2
				auto n = u.Cross(v);//u x v

				auto view = cam.pos - currPoly->tvlist[0];//�ӵ�ָ����������

				// ������
				float dp = n * view;

				// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
				if (dp <= 0.0) {
					currPoly->state = currPoly->state & PloygonStates::kBackface;
				}

			} // end for poly

		} // end RemoveBackfacesRenderlist
		
		//���������תΪ���������
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

		//����zֵ�Զ��������
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