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
	enum  class CameraType
	{
		kModeEuler,
		kModeUvn,
	};

	enum class CameraRotSeq
	{//��ת˳��
		kSeqXYZ,
		kSeqYXZ,
		kSeqXZY,
		kSeqYZX,
		kSeqZYX,
		kSeqZXY,
	};
	enum class CameraUvnMode
	{//Uvn ���ģ��
		kSimple,//��ģ�ͣ�ʹ��Ŀ��λ�ú͹۲�ο���
		kSpherical//��������ģʽ������x/y��Ϊ�۲������ķ�λ�Ǻ�����
	};
	class Camera {
	public:
		int state{ 0 };
		CameraType attr{ CameraType::kModeUvn };
		Point4D pos;//��������������е�λ��
		Vector4D dir;//ŷ���ǶȻ�UVN���ģ�͵�ע�ӷ���

		Vector4D u{ 1.0,0.0,0.0,1.0 };//UVN���ģ�͵ĳ���
		Vector4D v{ 0.0,1.0,0.0,1.0 };
		Vector4D n{ 0.0,0.0,1.0,1.0 };
		Point4D target;//UVNģ��Ŀ���λ��

		float viewDist{ 0.0 };//�Ӿ�
		float viewDistH{ 0.0 };//ˮƽ�Ӿ�/��ֱ�Ӿࣨ͸�ӱ任��ʹ�ã�
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
		void InitCamera(CameraType camAttr, const Point4D &camPos, const Vector4D &camdir, const Point4D &camTarget,
			float nearClipZ, float farClipZ, float fov, float viewportWidth, float viewportHeight) {
			attr = camAttr;
			pos = camPos;
			dir = camdir;

			target = camTarget;
			nearClipZ = nearClipZ;
			farClipZ = farClipZ;

			viewPortWidth = viewportWidth;   // �ӿڴ�С
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
			float tan_fov_div2 = 1 / tan(DegToRad(fov / 2));

			viewDist = (0.5)*(viewPlaneWidth)*tan_fov_div2;

			if (fabs(fov - 90.0f) < EPSILON_E5)
			{
				Point3D ptOri;

				Vector3D vn{ 1.f,0.f,-1.f }; // �淨��
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
				Point3D ptOrigin;//ƽ���ϵ�һ����
				Vector3D vn; // �淨��

				//�Ҳü���
				vn = Vector3D{ viewDist,0.0f,-viewPlaneWidth / 2.0f };
				rtClipPlane = Plane3D{ ptOrigin ,vn,true };

				// ��ü��棬���Ҳü������z��Գ�
				vn = Vector3D{ -viewDist,0.0,-viewPlaneWidth / 2.0f };
				ltClipPlane = Plane3D{ ptOrigin ,vn,true };

				// �ϲü���
				vn = Vector3D{ 0.0f,viewDist,-viewPlaneWidth / 2.0f };
				tpClipPlane = Plane3D{ ptOrigin ,vn,true };

				//�²ü���
				vn = Vector3D{ 0.0f,-viewDist,-viewPlaneWidth / 2.0f };
				btClipPlane = Plane3D{ ptOrigin ,vn,true };
			}

		} // end Init_CAM4DV1

		//����ŷ���Ƕȼ�������任����seqȷ����ת˳���������굽�������任����
		void BuildMatrixEuler(CameraRotSeq seq)
		{
			//���ƽ�ƾ��������
			Matrix<float, 4, 4> mtInv{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				-pos.x, -pos.y, -pos.z, 1 };

			Matrix<float, 4, 4>	mtmp;    // ��������ת�����

			//��ȡŷ���Ƕ�
			float theta_x = dir.x;
			float theta_y = dir.y;
			float theta_z = dir.z;

			// compute the sine and cosine of the angle x
			float cos_theta = cosf(theta_x);  // no change since cos(-x) = cos(x)
			float sin_theta = -sinf(theta_x); // sin(-x) = -sin(x)
			// ��x����ת����� 
			Matrix<float, 4, 4>	mxInv{
				1, 0, 0, 0,
				0, cos_theta, sin_theta, 0,
				0, -sin_theta, cos_theta, 0,
				0, 0, 0, 1 };

			// compute the sine and cosine of the angle y
			cos_theta = cosf(theta_y);  // no change since cos(-x) = cos(x)
			sin_theta = -sinf(theta_y); // sin(-x) = -sin(x)
			//��y����ת�����
			Matrix<float, 4, 4> myInv{
				cos_theta, 0, -sin_theta, 0,
				0, 1, 0, 0,
				sin_theta, 0, cos_theta, 0,
				0, 0, 0, 1 };

			// compute the sine and cosine of the angle z
			cos_theta = cosf(theta_z);  // no change since cos(-x) = cos(x)
			sin_theta = -sinf(theta_z); // sin(-x) = -sin(x)
			// ��z����ת�����
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
			// ��ƽ�ƾ��������ת���󣬵õ����ձ任����
			mcam = mtInv * mtmp;
		}

		//����ע������n,v,u��������任����
		void BuildCameraMatrixUVN(CameraUvnMode mode) {
			// step 1:ƽ�������
			Matrix<float, 4, 4> mtInv{
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					-pos.x, -pos.y, -pos.z, 1 };

			// step 2: ����ģ�ͣ�����Ŀ���
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

		 // ���� u,v,n
		 // Step 1: n = <target position - view reference point>
			n = target - pos;

			// Step 2: Let v = <0,1,0>
			v = Vector4D{ 0.f, 1.f, 0.f ,1.f };

			// Step 3: u = (v x n)
			u = v.Cross(n);

			// Step 4: v = (n x u)
			v = n.Cross(u);

			// Step 5: ��һ��
			u.Normalize();
			v.Normalize();
			n.Normalize();

			Matrix<float, 4, 4> mtUvn = {
				u.x, v.x, n.x, 0,
				u.y, v.y, n.y, 0,
				u.z, v.z,n.z, 0,
				0, 0, 0, 1 };
			// ����任����
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