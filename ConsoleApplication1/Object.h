#pragma once
#include "Camera.h"
#include <string>
#include "Math.h"
#include "Light.h"
#include "Color.h"
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
		Vertex() = default;
		Vertex(const Vertex &tx):pos(tx.pos), normal(tx.normal), uv(tx.uv), color(tx.color){}
		Vertex(float x, float y, float z, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0)
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

	/***********************�����(Ŀǰֻ֧��������)******************************/
	class Polygon
	{
	public:
		Polygon(const Vertex &v1, const Vertex &v2, const Vertex &v3, const Vector4D &fn, int mID = -1)
			:faceNormal(fn), materialID(mID){
			vlist[0] = Vertex(v1);
			vlist[1] = Vertex(v2);
			vlist[2] = Vertex(v3);
		}
		int state{ PloygonStates ::kActive};
		int attr;
		Vertex vlist[3];//�����ζ���
		Vector4D faceNormal;//�淨��
		int materialID = -1;//����id	
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
		Mesh mesh;
		std::vector<int> matIdList;
		//������Ķ����б���б任	//@parm obj��Ҫ�任�����壬mat �任���� ��type ָ��Ҫ�任���б� isChange �Ƿ�Գ��������任
		void TransfromObject(Matrix<float, 4, 4> &mat,const TransfromType type, bool isChange) {
			switch (type) {
			case TransfromType::kLocalOnly:
				for (int i = 0; i < mesh.vlistLocal.size(); ++i) {
					mesh.vlistLocal[i].pos = mesh.vlistLocal[i].pos * mat;
				}
				break;
			case TransfromType::kTransOnly:
				for (int i = 0; i < mesh.vlistTtran.size(); ++i) {
					mesh.vlistTtran[i].pos = mesh.vlistTtran[i].pos * mat;
				}
				break;
			case TransfromType::kLocalToTrans:
				for (int i = 0; i < mesh.vlistLocal.size(); ++i) {
					mesh.vlistTtran[i].pos = mesh.vlistLocal[i].pos * mat;
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
				for (int i = 0; i < mesh.vlistLocal.size(); ++i) {
					mesh.vlistTtran[i].pos = mesh.vlistLocal[i].pos + worldPos;
				}
			}
			else {//TransfromType::kTransOnly
				for (int i = 0; i < mesh.vlistTtran.size(); ++i) {
					mesh.vlistTtran[i].pos = mesh.vlistTtran[i].pos + worldPos;
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
			for (int vertex = 0; vertex < mesh.vlistTtran.size(); ++vertex)
			{
				mesh.vlistTtran[vertex].pos = mesh.vlistTtran[vertex].pos * cam.mcam;
			}
		}
		//������굽��Ļ����任
		void CameraToScreen(Camera &cam) {

			float alpha = (0.5*cam.viewPortWidth - 0.5);
			float beta = (0.5*cam.viewPortHeight - 0.5);

			for (int vertex = 0; vertex < mesh.vlistTtran.size(); ++vertex) {
				float z = mesh.vlistTtran[vertex].pos.z;

				// transform the vertex by the view parameters in the camera
				mesh.vlistTtran[vertex].pos.x = cam.viewDist * mesh.vlistTtran[vertex].pos.x / z;
				mesh.vlistTtran[vertex].pos.y = cam.viewDist * mesh.vlistTtran[vertex].pos.y*cam.aspectRatio / z;
				// z = z, so no change

				mesh.vlistTtran[vertex].pos.x = alpha + alpha * mesh.vlistTtran[vertex].pos.x;
				mesh.vlistTtran[vertex].pos.y = beta - beta * mesh.vlistTtran[vertex].pos.y;
			} // end for vertex

		} // end Camera_To_P

		//͸�ӱ任
		void CameraToPerspective(Camera &cam) {
			for (int vertex = 0; vertex < mesh.vlistTtran.size(); ++vertex) {
				float z = mesh.vlistTtran[vertex].pos.z;

				// transform the vertex by the view parameters in the camera
				mesh.vlistTtran[vertex].pos.x = cam.viewDist * mesh.vlistTtran[vertex].pos.x / z;
				mesh.vlistTtran[vertex].pos.y = cam.viewDist * mesh.vlistTtran[vertex].pos.y * cam.aspectRatio / z;
			} // end for vertex
		}

		//�ӿڱ任
		void PerspectiveToScreen(Camera &cam) {
			float alpha = (0.5*cam.viewPortWidth - 0.5);
			float beta = (0.5*cam.viewPortHeight - 0.5);

			for (int vertex = 0; vertex < mesh.vlistTtran.size(); ++vertex) {

				// transform the vertex by the view parameters in the camera
				mesh.vlistTtran[vertex].pos.x = alpha + alpha * mesh.vlistTtran[vertex].pos.x;
				mesh.vlistTtran[vertex].pos.y = beta - beta * mesh.vlistTtran[vertex].pos.y;

			} // end for vertex
		}

		// �����޳�,���������Ϣ�ж������Ƿ����Ӿ����ڣ�cullflagsָ������Щ����ִ���޳�
		bool CullObject(Camera &cam, CullType cullflags) {
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
			for (int vertex = 0; vertex < mesh.vlistLocal.size(); ++vertex)
			{
				auto _pos = mesh.vlistLocal[vertex].pos;
				// update the average and maximum radius
				float dist_to_vertex = sqrt(_pos.x*_pos.x +_pos.y*_pos.y +_pos.z*_pos.z);

				// accumulate total radius
				avgRadius += dist_to_vertex;

				// update maximum radius   
				if (dist_to_vertex > maxRadius)
					maxRadius = dist_to_vertex;

			} // end for vertex

			// finallize average radius computation
			avgRadius /= mesh.vlistLocal.size();

		}

		//Ԥ�ȼ����淨�ߺͶ��㷨��
		void CalculateNormal(bool need_vertex) {
			std::vector<int> tmp(mesh.vlistLocal.size(),0);
			int face_index = 0;
			for (int i = 0; i < mesh.index.size(); ++face_index, i += 3) {
				auto vec1 = mesh.vlistLocal[mesh.index[i + 1]].pos - mesh.vlistLocal[mesh.index[i]].pos;
				auto vec2 = mesh.vlistLocal[mesh.index[i + 2]].pos - mesh.vlistLocal[mesh.index[i + 1]].pos;
				//�淨��
				mesh.faceNormal[face_index] = vec1.Cross(vec2);

				if (need_vertex) {//���ݶ����ۼ��淨��
					mesh.vlistLocal[mesh.index[i]].normal += mesh.faceNormal[face_index];
					mesh.vlistLocal[mesh.index[i + 1]].normal += mesh.faceNormal[face_index];
					mesh.vlistLocal[mesh.index[i + 2]].normal += mesh.faceNormal[face_index];
					++tmp[mesh.index[i]];
					++tmp[mesh.index[i + 1]];
					++tmp[mesh.index[i + 2]];
				}
			}
			if (need_vertex) {
				for (int i = 0; i < mesh.vlistLocal.size(); ++i) {
					if (tmp[i] != 0)
					{
						mesh.vlistLocal[i].normal /= (float)tmp[i];
						mesh.vlistLocal[i].normal.Normalize();
					}
				}
			}
		}
	};
	class Cube final :public Object
	{
	public:
		//����Cube
		Cube(float _width, float _length, float _height, const Vector4D& _worldPos, const Vector4D& _dir, const float _scale)
		{
			id = (objID++);
			name = "cube";
			worldPos = _worldPos;
			dir = _dir;

			//���ö����ģ������
			Vertex v1(_width, _length, _height), v2(_width, -_length, _height), v3(-_width, -_length, _height), v4(-_width, _length, _height),
				v5(_width, _length, -_height), v6(_width, -_length, -_height), v7(-_width, -_length, -_height), v8(-_width, _length, -_height);


			//���ö������ɫ
			v1.color.SetColor(255, 0, 0);
			v2.color.SetColor(0, 255, 0);
			v3.color.SetColor(0, 0, 255);
			v4.color.SetColor(255, 255, 0);
			v5.color.SetColor(255, 0, 255);
			v6.color.SetColor(0, 255, 255);
			v7.color.SetColor(255, 255, 255);
			v8.color.SetColor(128, 128, 128);

			//���ö����uv����
			v1.uv.x = 1.0f;
			v1.uv.y = 0.0f;
			v2.uv.x = 1.0f;
			v2.uv.y = 1.0f;
			v3.uv.x = 0.0f;
			v3.uv.y = 1.0f;
			v4.uv.x = 0.0f;
			v4.uv.y = 0.0f;

			//����������б�
			mesh.vlistLocal.push_back(std::move(v1));
			mesh.vlistLocal.push_back(std::move(v2));
			mesh.vlistLocal.push_back(std::move(v3));
			mesh.vlistLocal.push_back(std::move(v4));
			mesh.vlistLocal.push_back(std::move(v5));
			mesh.vlistLocal.push_back(std::move(v6));
			mesh.vlistLocal.push_back(std::move(v7));
			mesh.vlistLocal.push_back(std::move(v8));


			//���ö�������
			//front face
			mesh.index.push_back(0);
			mesh.index.push_back(2);
			mesh.index.push_back(1);

			mesh.index.push_back(0);
			mesh.index.push_back(3);
			mesh.index.push_back(2);

			//back face
			mesh.index.push_back(7);
			mesh.index.push_back(5);
			mesh.index.push_back(6);

			mesh.index.push_back(7);
			mesh.index.push_back(4);
			mesh.index.push_back(5);

			//left face
			mesh.index.push_back(3);
			mesh.index.push_back(6);
			mesh.index.push_back(2);

			mesh.index.push_back(3);
			mesh.index.push_back(7);
			mesh.index.push_back(6);

			//right face
			mesh.index.push_back(4);
			mesh.index.push_back(1);
			mesh.index.push_back(5);

			mesh.index.push_back(4);
			mesh.index.push_back(0);
			mesh.index.push_back(1);

			//top face
			mesh.index.push_back(4);
			mesh.index.push_back(3);
			mesh.index.push_back(0);

			mesh.index.push_back(4);
			mesh.index.push_back(7);
			mesh.index.push_back(3);

			//bottom face
			mesh.index.push_back(1);
			mesh.index.push_back(6);
			mesh.index.push_back(5);

			mesh.index.push_back(1);
			mesh.index.push_back(2);
			mesh.index.push_back(6);

			ComputeObjectRadius();
			CalculateNormal(true);

			if (!Equalf(_scale,1.0))
			{
				ScaleMath(_scale);
			}
		}
	};

	/************************��������б�*************************************/
	// ����z��Сֵ����
	inline bool CompareNearZ(const Polygon &poly_1, const Polygon & poly_2) {
		float z1, z2;

		// compute the near z of each polygon
		z1 = std::fmin(poly_1.vlist[0].pos.z, poly_1.vlist[1].pos.z);
		z1 = std::fmin(z1, poly_1.vlist[2].pos.z);

		z2 = std::fmin(poly_2.vlist[0].pos.z, poly_2.vlist[1].pos.z);
		z2 = std::fmin(z2, poly_2.vlist[2].pos.z);

		// compare z1 and z2, such that polys' will be sorted in descending Z order
		return z1 > z2;
	}
	// ����z���ֵ����
	inline bool CompareFarZ(const Polygon &poly_1, const Polygon &poly_2) {
		float z1, z2;

		// compute the near z of each polygon
		z1 = std::fmax(poly_1.vlist[0].pos.z, poly_1.vlist[1].pos.z);
		z1 = std::fmax(z1, poly_1.vlist[2].pos.z);

		z2 = std::fmax(poly_2.vlist[0].pos.z, poly_2.vlist[1].pos.z);
		z2 = std::fmax(z2, poly_2.vlist[2].pos.z);

		// compare z1 and z2, such that polys' will be sorted in descending Z order
		return z1 > z2;
	}
	// ����zƽ��ֵ����
	inline bool CompareAvgZ(const Polygon &poly_1, const Polygon &poly_2) {
		float z1, z2;

		// compute the near z of each polygon
		z1 = (poly_1.vlist[0].pos.z + poly_1.vlist[1].pos.z + poly_1.vlist[2].pos.z) / 3;
		z2 = (poly_2.vlist[0].pos.z + poly_2.vlist[1].pos.z + poly_2.vlist[2].pos.z) / 3;

		// compare z1 and z2, such that polys' will be sorted in descending Z order
		return z1 > z2;
	}

	class RenderList {
	public:
		int state{ 0 };//״̬
		int attr{ 0 };//����

		//����δ������
		std::vector<Polygon> polyData;	
		
		//����Ⱦ�б�Ķ���α任	//@parm rederList ��Ⱦ�б�mat �任���� ��type ָ��Ҫ�任������
		void TransformRenderList(Matrix<float, 4, 4> &mat, TransfromType type) {
			switch (type) {
			case TransfromType::kLocalOnly:
				for (int i = 0; i < polyData.size(); ++i) {
					auto curPoly = &polyData[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->vlist[vetNum].pos;
						curPoly->vlist[vetNum].pos = vect * mat;
					}
				}
				break;
			case TransfromType::kTransOnly:
				for (int i = 0; i < polyData.size(); ++i) {
					auto curPoly = &polyData[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->vlist[vetNum].pos;
						curPoly->vlist[vetNum].pos = vect * mat;
					}
				}
				break;
			case TransfromType::kLocalToTrans:
				for (int i = 0; i < polyData.size(); ++i) {
					auto curPoly = &polyData[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->vlist[vetNum].pos;
						curPoly->vlist[vetNum].pos = vect * mat;
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
				for (int i = 0; i < polyData.size(); ++i) {
					auto curPoly = &polyData[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						curPoly->vlist[vetNum].pos = curPoly->vlist[vetNum].pos + worldPos;
					}
				}
			}
			else {//TransfromType::kTransOnly
				for (int i = 0; i < polyData.size(); ++i) {
					auto curPoly = &polyData[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						curPoly->vlist[vetNum].pos = curPoly->vlist[vetNum].pos + worldPos;
					}
				}
			}

		}
	    
		//��������-���������
		void WorldToCamera(Camera &cam)
		{//��������任������Ⱦ�б��еĶ���λ�Ϊ�������
			for (int i = 0; i < polyData.size(); ++i) {
				auto curPoly = &polyData[i];
				if (curPoly == nullptr ||
					!(curPoly->state &PloygonStates::kActive) ||
					curPoly->state &PloygonStates::kClipped ||
					curPoly->state &PloygonStates::kBackface) {
					continue;
				}
				for (int vetNum = 0; vetNum < 3; ++vetNum) {
					curPoly->vlist[vetNum].pos = curPoly->vlist[vetNum].pos * cam.mcam;
				}
			}
		}
		
		//͸�ӱ任(�Ǿ��󷨣����������Ⱦ�б�Ķ�����ѱ��任Ϊ�������)
		void CameraToPerspective(Camera &cam) {
			for (int poly = 0; poly < polyData.size(); poly++)
			{
				// acquire current polygon
				auto currPoly = &polyData[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

			 // all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++)
				{
					float z = currPoly->vlist[vertex].pos.z;

					// transform the vertex by the view parameters in the camera
					currPoly->vlist[vertex].pos.x = cam.viewDist*currPoly->vlist[vertex].pos.x / z;
					currPoly->vlist[vertex].pos.y = cam.viewDist*currPoly->vlist[vertex].pos.y*cam.aspectRatio / z;

				} // end for vertex

			} // end for poly

		} // end CameraToPerspective

		//�ӿڱ任
		void PerspectiveToScreen(Camera &cam) {
			//��Ⱦ�б������͸�ӱ任����һ����͸������-����Ļ����
			for (int poly = 0; poly < polyData.size(); poly++){
				// acquire current polygon
				auto currPoly = &polyData[poly];

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
					currPoly->vlist[vertex].pos.x = alpha + alpha * currPoly->vlist[vertex].pos.x;
					currPoly->vlist[vertex].pos.y = beta - beta * currPoly->vlist[vertex].pos.y;
				} // end for vertex

			} // end for poly	
		}
		
		  //������굽��Ļ����ת����ֱ�ӷ���
		void CameraToScreen(Camera &cam) {
			for (int poly = 0; poly < polyData.size(); poly++){
				// acquire current polygon
				auto currPoly = &polyData[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				float alpha = (0.5*cam.viewPortWidth - 0.5);
				float beta = (0.5*cam.viewPortHeight - 0.5);

				// all good, let's transform 
				for (int vertex = 0; vertex < 3; ++vertex){
					float z = currPoly->vlist[vertex].pos.z;

					// transform the vertex by the view parameters in the camera
					currPoly->vlist[vertex].pos.x = cam.viewDist*currPoly->vlist[vertex].pos.x / z;
					currPoly->vlist[vertex].pos.y = cam.viewDist*currPoly->vlist[vertex].pos.y*cam.aspectRatio / z;

					//�������ţ���תy��
					currPoly->vlist[vertex].pos.x = alpha + alpha * currPoly->vlist[vertex].pos.x;
					currPoly->vlist[vertex].pos.y = beta - beta * currPoly->vlist[vertex].pos.y;

				} // end for vertex

			} // end for poly

		} // end 
		
		//��������
		void RemoveBackfaces(Camera cam) {
			// ���ö���α���״̬
			for (int poly = 0; poly < polyData.size(); ++poly)
			{
				// acquire current polygon
				auto currPoly = &polyData[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				//���������淨�ߣ������ǰ�˳ʱ�뷽������
				auto u = currPoly->vlist[1].pos - currPoly->vlist[0].pos;//p0->p1
				auto v = currPoly->vlist[2].pos - currPoly->vlist[0].pos;//p0->p2
				auto n = u.Cross(v);//u x v

				auto view = cam.pos - currPoly->vlist[0].pos;//�ӵ�ָ����������

				// ������
				float dp = n * view;

				// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
				if (dp <= 0.0) {
					//currPoly->state = currPoly->state & PloygonStates::kBackface;
				}

			} // end for poly

		} // end RemoveBackfacesRenderlist
		
		//���������תΪ���������
		void ConvertFromHomo4D() {
			for (int poly = 0; poly < polyData.size(); poly++)
			{
				// acquire current polygon
				auto currPoly = &polyData[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

			 // all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++)
				{
					currPoly->vlist[vertex].pos.DivByW();
				} // end for vertex

			} // end for poly		

		}

		//�����βü�
		void PolyCulling(Camera &cam) {
		
		}

		//����zֵ�Զ��������
		void SortByZ(SortType type){
			switch (type){
			case SortType::kAvgZ:  //  - sorts on average of all vertices
			{
				std::sort(polyData.begin(), polyData.end(), CompareAvgZ);
			} break;

			case SortType::kMinZ: // - sorts on closest z vertex of each poly
			{
				std::sort(polyData.begin(), polyData.end(), CompareNearZ);
			} break;

			case SortType::kMaxZ:  //  - sorts on farthest z vertex of each poly
			{
				std::sort(polyData.begin(), polyData.end(), CompareFarZ);
			} break;

			default: break;
			} // end switch

		}

	};
	
	
}