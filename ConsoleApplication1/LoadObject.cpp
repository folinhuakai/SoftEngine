#include "LoadObject.h"
#include<fstream>
namespace maki {
	// 位掩码
	constexpr auto PLX_SHADE_MODE_MASK = 0x6000;//抽取着色模式掩码

	// 多边形着色模式（针对plg文件）
	constexpr auto PLX_SHADE_MODE_PURE_FLAG = 0x0000;//固定颜色
	constexpr auto PLX_SHADE_MODE_CONSTANT_FLAG = 0x0000;//(别名)
	constexpr auto PLX_SHADE_MODE_FLAT_FLAG = 0x2000;//恒定颜色
	constexpr auto PLX_SHADE_MODE_GOURAUD_FLAG = 0x4000;//gouraud着色
	constexpr auto PLX_SHADE_MODE_PHONG_FLAG = 0x6000;//phong着色
	constexpr auto PLX_SHADE_MODE_FASTPHONG_FLAG = 0x6000;//别名
	constexpr auto PLX_2SIDED_FLAG = 0x1000;//双面
	constexpr auto PLX_1SIDED_FLAG = 0x0000;//单面
	constexpr auto PLX_COLOR_MODE_RGB_FLAG = 0x8000;//rgb模式
	/********************读取plg文件的一行，跳过注释和空行****************************/
	bool GetLine(std::ifstream &fp, std::string &out)
	{
		while (!fp.eof())
		{
			std::getline(fp, out);
			// test if this was a blank line or a comment
			if (out == "" || out[0] == '#')
				continue;

			// at this point we have a good line
			return true;
		}
		return false;
	} // end GetLine

	/****************************计算物体平均半径和最大半径***********************************************/
	float ComputeObjectRadius(Object &obj)
	{
		// this function computes the average and maximum radius for 
		// sent object and opdates the object data

		// reset incase there's any residue
		obj.avgRadius = 0;
		obj.maxRadius = 0;

		// loop thru and compute radius
		for (int vertex = 0; vertex < obj.numVertices; vertex++)
		{
			// update the average and maximum radius
			float dist_to_vertex =
				sqrt(obj.vlistLocal[vertex].x*obj.vlistLocal[vertex].x +
					obj.vlistLocal[vertex].y*obj.vlistLocal[vertex].y +
					obj.vlistLocal[vertex].z*obj.vlistLocal[vertex].z);

			// accumulate total radius
			obj.avgRadius += dist_to_vertex;

			// update maximum radius   
			if (dist_to_vertex > obj.maxRadius)
				obj.maxRadius = dist_to_vertex;

		} // end for vertex

		// finallize average radius computation
		obj.avgRadius /= obj.numVertices;

		// return max radius
		return(obj.maxRadius);

	}

	/*******************************以空格符分割字符串********************************/
	std::vector<std::string> Split(const std::string &s, const std::string &seperator) {
		std::vector<std::string> result;
		typedef std::string::size_type string_size;
		string_size i = 0;

		while (i != s.size()) {
			//找到字符串中首个不等于分隔符的字母；
			int flag = 0;
			while (i != s.size() && flag == 0) {
				flag = 1;
				for (string_size x = 0; x < seperator.size(); ++x)
					if (s[i] == seperator[x]) {
						++i;
						flag = 0;
						break;
					}
			}

			//找到又一个分隔符，将两个分隔符之间的字符串取出；
			flag = 0;
			string_size j = i;
			while (j != s.size() && flag == 0) {
				for (string_size x = 0; x < seperator.size(); ++x)
					if (s[j] == seperator[x]) {
						flag = 1;
						break;
					}
				if (flag == 0)
					++j;
			}
			if (i != j) {
				result.push_back(s.substr(i, j - i));
				i = j;
			}
		}
		return result;
	}

	/**********************************加载plg文件***************************************/
	int Load_OBJECT4DV1_PLG(Object& obj, //物体
		const std::string &filename,     //文件名
		Vector4D &initScale,	 //初始缩放
		Vector4D &wordlPos,       //世界坐标位置
		Vector4D &rot,
		bool isPrint)       // 是否打印读入的数据
	{
		//memset(&obj, 0, sizeof(Object));
		std::string line;  // working buffer
		// 设置物体状态为活动、可见
		obj.state = OBJECT_STATE_ACTIVE | OBJECT_STATE_VISIBLE;
		// 设置物体世界坐标
		obj.worldPos = wordlPos;
		// Step 2: open the file for reading
		std::ifstream fp(filename);
		if (!fp.is_open()) {
			std::cout << "failed to open " << filename << '\n';
		}
		// Step 3: 读取物体描述符
		if (!(GetLine(fp, line)))
		{
			std::cout << "PLG file error with file " << filename << "(object descriptor invalid).";
			return(0);
		} // end if
		if (isPrint) {
			std::cout << "Object Descriptor:" << line << std::endl;
		}		
		// 设置物体描述符
		auto result = Split(line, " ");
		if (result.size() >= 3) {
			obj.name = result[0];
			obj.numVertices = std::stoi(result[1]);
			obj.numPolygons = std::stoi(result[2]);
		}
		else {
			std::cout << "object name error";
			return(0);
		}

		// Step 4: 顶点列表
		for (int vertex = 0; vertex < obj.numVertices; vertex++)
		{
			// get the next vertex
			if (!(GetLine(fp, line)))
			{
				std::cout << "PLG file error with file vertex list invalid.";
				return(0);
			} // end if
			if (isPrint) {
				std::cout << "one Point:" << line << std::endl;
			}			
			result = Split(line, " ");
			if (result.size() >= 3) {
				obj.vlistLocal[vertex].x = std::stof(result[0]);
				obj.vlistLocal[vertex].y = std::stof(result[1]);
				obj.vlistLocal[vertex].z = std::stof(result[2]);
				obj.vlistLocal[vertex].w = 1;
			}
			else {
				std::cout << "object vertex number error";
				return(0);
			}
			// scale vertices
			obj.vlistLocal[vertex].x *= initScale.x;
			obj.vlistLocal[vertex].y *= initScale.y;
			obj.vlistLocal[vertex].z *= initScale.z;
		} // end for vertex

		// 计算平均半径和最大半径
		ComputeObjectRadius(obj);


		int poly_surface_desc = 0; // PLG/PLX surface descriptor
		int poly_num_verts = 0; // 当前多边形顶点数（一般为3）
		std::string tmp_string; // 面描述符						

		// Step 5: 加载多边形列表
		for (int poly = 0; poly < obj.numPolygons; poly++)
		{
			// get the next polygon descriptor
			if (!(GetLine(fp, line)))
			{
				std::cout << "PLG file error " << std::endl;
				return(0);
			} // end if
			if (isPrint) {
				std::cout << "one polygon:" << line << std::endl;
			}
			
			result = Split(line, " ");
			if (result.size() >= 5) {
				tmp_string = result[0];
				poly_num_verts = std::stoi(result[1]);
				obj.plist[poly].vert[0] = std::stoi(result[2]);
				obj.plist[poly].vert[1] = std::stoi(result[3]);
				obj.plist[poly].vert[2] = std::stoi(result[4]);
			}
			else {
				std::cout << "object polygon number error";
				return(0);
			}

			// 面描述符可以是16进制
			if (tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
				poly_surface_desc = std::stoi(result[0], 0, 16);
			else
				poly_surface_desc = std::stoi(tmp_string);

			// 多边形顶点列表指向物体顶点列表
			obj.plist[poly].vlist = obj.vlistLocal;

			if ((poly_surface_desc & PLX_2SIDED_FLAG))
			{
				obj.plist[poly].attr = obj.plist[poly].attr | POLY4DV1_ATTR_2SIDED;
			} // end if

		 // now let's set the color type and color
			if ((poly_surface_desc & PLX_COLOR_MODE_RGB_FLAG))
			{
				// this is an RGB 4.4.4 surface
				obj.plist[poly].attr = obj.plist[poly].attr | POLY4DV1_ATTR_RGB16;

				// now extract color and copy into polygon color
				// field in proper 16-bit format 
				// 0x0RGB is the format, 4 bits per pixel 
				int red = ((poly_surface_desc & 0x0f00) >> 8);
				int green = ((poly_surface_desc & 0x00f0) >> 4);
				int blue = (poly_surface_desc & 0x000f);

				// although the data is always in 4.4.4 format, the graphics card
				// is either 5.5.5 or 5.6.5, but our virtual color system translates
				// 8.8.8 into 5.5.5 or 5.6.5 for us, but we have to first scale all
				// these 4.4.4 values into 8.8.8
				obj.plist[poly].color = (red << 4 << 16) + (green << 4 << 8) + (blue << 4);
			} // end if
			else
			{
				// 使用八位颜色索引
				obj.plist[poly].attr = obj.plist[poly].attr | POLY4DV1_ATTR_8BITCOLOR;

				// and simple extract the last 8 bits and that's the color index
				obj.plist[poly].color = (poly_surface_desc & 0x00ff);

			} // end else

		// handle shading mode
			int shade_mode = (poly_surface_desc & PLX_SHADE_MODE_MASK);

			// set polygon shading mode
			switch (shade_mode)
			{
			case PLX_SHADE_MODE_PURE_FLAG: {
				obj.plist[poly].attr = obj.plist[poly].attr | POLY4DV1_ATTR_SHADE_MODE_PURE;
			} break;

			case PLX_SHADE_MODE_FLAT_FLAG: {
				obj.plist[poly].attr = obj.plist[poly].attr | POLY4DV1_ATTR_SHADE_MODE_FLAT;

			} break;

			case PLX_SHADE_MODE_GOURAUD_FLAG: {
				obj.plist[poly].attr = obj.plist[poly].attr | POLY4DV1_ATTR_SHADE_MODE_GOURAUD;
			} break;

			case PLX_SHADE_MODE_PHONG_FLAG: {
				obj.plist[poly].attr = obj.plist[poly].attr | POLY4DV1_ATTR_SHADE_MODE_PHONG;
			} break;

			default: break;
			} // end switch

	   // finally set the polygon to active
			obj.plist[poly].state = PloygonStates::kActive;

		} // end for poly

	// close the file
		fp.close();

		// return success
		return(1);

	} // end Load_OBJECT4DV1_PLG}
}

