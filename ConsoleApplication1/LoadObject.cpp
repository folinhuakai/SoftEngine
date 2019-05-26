#include "LoadObject.h"
#include<fstream>
namespace maki {
	// λ����
	constexpr auto PLX_SHADE_MODE_MASK = 0x6000;//��ȡ��ɫģʽ����

	// �������ɫģʽ�����plg�ļ���
	constexpr auto PLX_SHADE_MODE_PURE_FLAG = 0x0000;//�̶���ɫ
	constexpr auto PLX_SHADE_MODE_CONSTANT_FLAG = 0x0000;//(����)
	constexpr auto PLX_SHADE_MODE_FLAT_FLAG = 0x2000;//�㶨��ɫ
	constexpr auto PLX_SHADE_MODE_GOURAUD_FLAG = 0x4000;//gouraud��ɫ
	constexpr auto PLX_SHADE_MODE_PHONG_FLAG = 0x6000;//phong��ɫ
	constexpr auto PLX_SHADE_MODE_FASTPHONG_FLAG = 0x6000;//����
	constexpr auto PLX_2SIDED_FLAG = 0x1000;//˫��
	constexpr auto PLX_1SIDED_FLAG = 0x0000;//����
	constexpr auto PLX_COLOR_MODE_RGB_FLAG = 0x8000;//rgbģʽ
	/********************��ȡplg�ļ���һ�У�����ע�ͺͿ���****************************/
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

	/****************************��������ƽ���뾶�����뾶***********************************************/
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

	/*******************************�Կո���ָ��ַ���********************************/
	std::vector<std::string> Split(const std::string &s, const std::string &seperator) {
		std::vector<std::string> result;
		typedef std::string::size_type string_size;
		string_size i = 0;

		while (i != s.size()) {
			//�ҵ��ַ������׸������ڷָ�������ĸ��
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

			//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
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

	/**********************************����plg�ļ�***************************************/
	int Load_OBJECT4DV1_PLG(Object& obj, //����
		const std::string &filename,     //�ļ���
		Vector4D &initScale,	 //��ʼ����
		Vector4D &wordlPos,       //��������λ��
		Vector4D &rot,
		bool isPrint)       // �Ƿ��ӡ���������
	{
		//memset(&obj, 0, sizeof(Object));
		std::string line;  // working buffer
		// ��������״̬Ϊ����ɼ�
		obj.state = OBJECT_STATE_ACTIVE | OBJECT_STATE_VISIBLE;
		// ����������������
		obj.worldPos = wordlPos;
		// Step 2: open the file for reading
		std::ifstream fp(filename);
		if (!fp.is_open()) {
			std::cout << "failed to open " << filename << '\n';
		}
		// Step 3: ��ȡ����������
		if (!(GetLine(fp, line)))
		{
			std::cout << "PLG file error with file " << filename << "(object descriptor invalid).";
			return(0);
		} // end if
		if (isPrint) {
			std::cout << "Object Descriptor:" << line << std::endl;
		}		
		// ��������������
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

		// Step 4: �����б�
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

		// ����ƽ���뾶�����뾶
		ComputeObjectRadius(obj);


		int poly_surface_desc = 0; // PLG/PLX surface descriptor
		int poly_num_verts = 0; // ��ǰ����ζ�������һ��Ϊ3��
		std::string tmp_string; // ��������						

		// Step 5: ���ض�����б�
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

			// ��������������16����
			if (tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
				poly_surface_desc = std::stoi(result[0], 0, 16);
			else
				poly_surface_desc = std::stoi(tmp_string);

			// ����ζ����б�ָ�����嶥���б�
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
				// ʹ�ð�λ��ɫ����
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

