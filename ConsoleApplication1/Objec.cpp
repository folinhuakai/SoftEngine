
#include "Object.h"
namespace maki {
	/****************************************��ӡgameobject*****************************/
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

	/*******************************���������굽�������*************************************/
	void WorldToCamera(Object &obj, Camera &cam)
	{//��object�е�vlistTrans�Ķ�����б任����������Ķ����ѱ任Ϊ�������꣬���������vlistTrans
		for (int vertex = 0; vertex < obj.numVertices; ++vertex)
		{
			obj.vlistTransl[vertex] = obj.vlistTransl[vertex] * cam.mcam;
		}
	}
	void WorldToCamera(RenderList &rdList, Camera &cam)
	{//��������任������Ⱦ�б��еĶ���λ�Ϊ�������
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


	/*******************************�����޳�*************************************/
	bool CullObject(Object &obj, Camera &cam, CullType cullflags) {//���������Ϣ�ж������Ƿ����Ӿ����ڣ�cullflagsָ������Щ����ִ���޳�
		//1.����Χ�����ı任���������
		auto spherePos = obj.worldPos * cam.mcam;

		// 2: ����cullflags�޳�����
		if (cullflags & CullType::kCullZPlane) {//Զ���ü���
			if (((spherePos.z - obj.maxRadius) > cam.farClipZ) ||
				((spherePos.z + obj.maxRadius) < cam.nearClipZ)) {
				obj.state = obj.state | ObjectState::kCull;
				return false;
			}
		} // end if

		if (cullflags & CullType::kCullXPlane) {//���Ҳü��棨ʹ���������ƣ�

			float z_test = (0.5)*cam.viewPlaneWidth*spherePos.z / cam.viewDist;

			if (((spherePos.x - obj.maxRadius) > z_test) || // right side
				((spherePos.x + obj.maxRadius) < -z_test))  // left side, note sign change
			{
				obj.state = obj.state | ObjectState::kCull;
				return false;
			} // end if
		} // end if

		if (cullflags & CullType::kCullYPlane) {//���²ü��棨ʹ���������ƣ�
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
	/*******************************�������״̬*************************************/
	void ResetOjbectState(Object &obj)
	{
		// this function resets the sent object and redies it for 
		// transformations, basically just resets the culled, clipped and
		// backface flags, but here's where you would add stuff
		// to ready any object for the pipeline
		// the object is valid, let's rip it apart polygon by polygon

		// reset object's culled flag
		/*RESET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);

		// now the clipped and backface flags for the polygons 
		for (int poly = 0; poly < obj->num_polys; poly++)
		{
			// acquire polygon
			POLY4DV1_PTR curr_poly = &obj->plist[poly];

			// first is this polygon even visible?
			if (!(curr_poly->state & POLY4DV1_STATE_ACTIVE))
				continue; // move onto next poly

			 // reset clipped and backface flags
			RESET_BIT(curr_poly->state, POLY4DV1_STATE_CLIPPED);
			RESET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);

		} // end for poly*/

	} // end Reset_OBJECT4DV1
}