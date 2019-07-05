#include "LoadObject.h"
#include<fstream>
#include<vector>
#include<sstream>
#include <functional>
#include <algorithm>
#include "Object.h"
#include<string>
#include"Math.h"
namespace maki {
	int objID = 1;
	/********************************从.obj文件中读取数据创建对象*******************************/
	void LoadFromFile(Object& obj, const std::string &fileName, const Vector4D &wordlPos, const Vector4D &initScale, float scale) {
		obj.id = objID;
		++objID;
		obj.name = fileName;
		std::ifstream ifs(fileName, std::ifstream::in);
		std::vector<float> uv;
		std::vector<float> v_normal;
		float tmp_x, tmp_y, tmp_z;
		int v_index, uv_index, n_index, v_idx, uv_idx, n_idx;
		bool has_pos = false;
		bool has_uv = false;
		bool has_normal = false;
		std::string head;
		std::stringstream ss;
		if (ifs) {
			std::string str;
			while (getline(ifs, str))
			{
				if (str.length() < 2) continue;
				if (str[0] == 'v')
				{
					if (str[1] == 't')
					{
						//纹理
						ss.clear();
						ss.str("");
						ss << str;
						ss >> head >> tmp_x >> tmp_y;
						uv.push_back(tmp_x);
						uv.push_back(tmp_y);
						has_uv = true;
					}
					else if (str[1] == 'n')
					{
						//法线
						ss.clear();
						ss.str("");
						ss << str;
						ss >> head >> tmp_x >> tmp_y >> tmp_z;
						v_normal.push_back(tmp_x);
						v_normal.push_back(tmp_y);
						v_normal.push_back(tmp_z);
						has_normal = true;
					}
					else
					{
						//顶点
						ss.clear();
						ss.str("");
						ss << str;
						ss >> head >> tmp_x >> tmp_y >> tmp_z;
						obj.mesh.vlistLocal.push_back(Vertex(tmp_x, tmp_y, tmp_z));
						has_pos = true;
					}
				}
				else if (str[0] == 'f')
				{
					for (uint32_t k = str.size() - 1; k > 0; --k)
					{
						if (str[k] == '/')
						{
							str[k] = ' ';
						}
					}
					ss.clear();
					ss.str("");
					ss << str;
					ss >> head;
					for (uint32_t i = 0; i < 3; ++i)
					{
						if (has_pos)
						{
							ss >> v_index;
							v_idx = v_index - 1;
							obj.mesh.index.push_back(v_idx);
						}
						if (has_uv)
						{
							ss >> uv_index;
							uv_idx = (uv_index - 1) << 1;
							obj.mesh.vlistLocal[v_idx].uv.x = uv[uv_idx];
							obj.mesh.vlistLocal[v_idx].uv.y = uv[uv_idx + 1];
						}
						if (has_normal)
						{
							ss >> n_index;
							n_idx = (n_index - 1) * 3;
							obj.mesh.vlistLocal[v_idx].normal.x = v_normal[n_idx];
							obj.mesh.vlistLocal[v_idx].normal.y = v_normal[n_idx + 1];
							obj.mesh.vlistLocal[v_idx].normal.z = v_normal[n_idx + 2];
						}
					}
				}
			}

			obj.ComputeObjectRadius();			
			obj.worldPos = wordlPos;

			if (scale -1.0 <= EPSILON_E5)
			{
				obj.ScaleMath(scale);
			}
		}
		ifs.close();
	}
		
}

