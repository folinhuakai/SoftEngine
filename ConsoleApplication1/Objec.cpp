
#include "Object.h"
namespace maki {
	/****************************************打印gameobject*****************************/
	std::ostream & operator <<(std::ostream &out, const Object &obj) {
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

	bool operator ==(const Object&a, const Object&b) {
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

	/*******************************从世界坐标到相机坐标*************************************/
	void WorldToCamera(Object &obj, Camera &cam)
	{//对object中的vlistTrans的顶点进行变换，假设物体的顶点已变换为世界坐标，并结果存在vlistTrans
		for (int vertex = 0; vertex < obj.numVertices; ++vertex)
		{
			obj.vlistTransl[vertex] = obj.vlistTransl[vertex] * cam.mcam;
		}
	}
	void WorldToCamera(RenderList &rdList, Camera &cam)
	{//根据相机变换矩阵将渲染列表中的多边形换为相机坐标
		for (int i = 0; i < rdList.numPolys; ++i) {
			auto curPoly = rdList.polyPtrs[i];
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
	/*******************************end*************************************/


	/*******************************物体剔除*************************************/
	bool CullObject(Object &obj, Camera &cam, CullType cullflags) {//根据相机信息判断物体是否在视景体内，cullflags指定在哪些轴上执行剔除
		//1.将包围球球心变换到相机坐标
		auto spherePos = obj.worldPos * cam.mcam;

		// 2: 根据cullflags剔除物体
		if (cullflags & CullType::kCullZPlane) {//远近裁剪面
			if (((spherePos.z - obj.maxRadius) > cam.farClipZ) ||
				((spherePos.z + obj.maxRadius) < cam.nearClipZ)) {
				obj.state = obj.state | ObjectState::kCull;
				return false;
			}
		} // end if

		if (cullflags & CullType::kCullXPlane) {//左右裁剪面（使用三角相似）

			float z_test = (0.5)*cam.viewPlaneWidth*spherePos.z / cam.viewDist;

			if (((spherePos.x - obj.maxRadius) > z_test) || // right side
				((spherePos.x + obj.maxRadius) < -z_test))  // left side, note sign change
			{
				obj.state = obj.state | ObjectState::kCull;
				return false;
			} // end if
		} // end if

		if (cullflags & CullType::kCullYPlane) {//上下裁剪面（使用三角相似）
			float z_test = (0.5)*cam.viewPlaneHeight *spherePos.z / cam.viewDist;
			if (((spherePos.y - obj.maxRadius) > z_test) || // top side
				((spherePos.y + obj.maxRadius) < -z_test))  // bottom side, note sign change
			{
				obj.state = obj.state | ObjectState::kCull;
				return false;
			} // end if

		} // end if

		// return failure to cull
		return true;
	}
	/*******************************清除物体状态*************************************/
	void ResetOjbectState(Object &obj){
		//清除物体剔除状态
		obj.state = obj.state & (!ObjectState::kCull);

		// 重置多边形裁剪和背面剔除标记
		for (int poly = 0; poly < obj.numPolygons; poly++)
		{
			auto curPoly = obj.plist[poly];
			if (!(curPoly.state &PloygonStates::kActive)) {
				continue;
			}
			curPoly.state = curPoly.state & (!PloygonStates::kClipped);
			curPoly.state = curPoly.state & (!PloygonStates::kBackface);

		} 
	}

	/*******************************背面清除*************************************/
	void RemoveBackfacesObject(Object &obj, Camera cam){
		//根据vlistTrans中顶点数据及相机位置消除物体的背面多边形（面法线与视点间的向量点乘，<90°即背面）
		if (obj.state & ObjectState::kCull)
			return;

		// 处理每个多边形
		for (int poly = 0; poly < obj.numPolygons; ++poly)
		{
			// acquire polygon
			auto currPoly = obj.plist[poly];		
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
			auto u = obj.vlistTransl[vindex_1] - obj.vlistTransl[vindex_0];//p0->p1
			auto v = obj.vlistTransl[vindex_2] - obj.vlistTransl[vindex_0];//p0->p2
			auto n = u.Cross(v);//u x v

			auto view = cam.pos - obj.vlistTransl[vindex_0];//视点指向多边形向量

			// 计算点积
			float dp = n * view;

			// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
			if (dp <= 0.0) {
				currPoly.state = currPoly.state & PloygonStates::kBackface;
			}

		} // end for poly

	} // end Remove_Backfaces_OBJECT4DV1
}