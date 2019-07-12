#pragma once
#include "VectorXD.h"
#include "Matrix.h"
#include "ParmLine.h"

namespace maki {
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

			viewPortWidth = _viewportWidth;   // 视口大小
			viewPortHeight = _viewportHeight;

			viewPortCenterX = (_viewportWidth - 1) / 2; // center of viewport
			viewPortCenterY = (_viewportHeight - 1) / 2;

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

			fov = _fov;

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
		// 获取透视变换矩阵（需要相机已初始化）
		Matrix<float, 4, 4> GetCameraToPerspectiveMtri()
		{//如果使用透视矩阵进行变换，则变换后w不再为1，需要调用ConvertFromHomo4DRend
			Matrix<float, 4, 4> mt{
				viewDist, 0, 0, 0,
				0, viewDist * aspectRatio, 0, 0,
				0, 0, 1, 1,
				0, 0, 0, 0 };

			return mt;
		}
	};
}