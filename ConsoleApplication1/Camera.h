#pragma once
#include "VectorXD.h"
#include "Matrix.h"
#include "ParmLine.h"

namespace maki {
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
		void InitCamera(CameraType camAttr, const Point4D &camPos, const Vector4D *camdir, const Point4D *camTarget,
			float _nearClipZ, float _farClipZ, float _fov, float _viewportWidth, float _viewportHeight) {
			attr = camAttr;
			pos = camPos;
			if (camdir != nullptr) {
				dir = *camdir;
			}
			if (camTarget != nullptr) {
				target = *camTarget;
			}
			
			nearClipZ = _nearClipZ;
			farClipZ = _farClipZ;

			viewPortWidth = _viewportWidth;   // �ӿڴ�С
			viewPortHeight = _viewportHeight;

			viewPortCenterX = (_viewportWidth - 1) / 2; // center of viewport
			viewPortCenterY = (_viewportHeight - 1) / 2;

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

			fov = _fov;

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
			Matrix<float, 4, 4> mzInv{
				cos_theta, sin_theta, 0, 0,
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
				float theta = DegToRad(dir.y); // heading

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
		// ��ȡ͸�ӱ任������Ҫ����ѳ�ʼ����
		Matrix<float, 4, 4> GetCameraToPerspectiveMtri()
		{//���ʹ��͸�Ӿ�����б任����任��w����Ϊ1����Ҫ����ConvertFromHomo4DRend
			Matrix<float, 4, 4> mt{
				viewDist, 0, 0, 0,
				0, viewDist * aspectRatio, 0, 0,
				0, 0, 1, 1,
				0, 0, 0, 0 };

			return mt;
		}
	};
}