#include "Object.h"
#include<vector>
#include<string>
#include<fstream>

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
char *Get_Line_PLG(char *buffer, int maxlength, std::ifstream &fp)
{
	int index = 0;  // general index
	int length = 0; // general length

	while (fp.good() && !fp.eof())
	{
		memset(buffer, 0, maxlength);
		fp.getline(buffer, maxlength);
		for (length = strlen(buffer), index = 0; isspace(buffer[index]); index++);

		// test if this was a blank line or a comment
		if (index >= length || buffer[index] == '#')
			continue;

		// at this point we have a good line
		return(&buffer[index]);
	}
} // end Get_Line_PLG

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

int Load_OBJECT4DV1_PLG(Object& obj, //物体
	const char * filename,     //文件名
	Vector4D &scale,	 //初始缩放
	Vector4D &pos,       //世界坐标位置
	Vector4D &rot)       // 初始旋转
{
	// 从磁盘文件内读取plg数据, 允许调用程序对物体进行缩放旋转等操作
	std::ifstream fp;
	char buffer[256];  // working buffer

	char *token_string;  //指向要分析的物体数据文本指针

	// plg文件格式

	// # object描述符
	// object_name_string num_verts_int num_polys_int

	// # 顶点列表
	// x0_float y0_float z0_float
	// x1_float y1_float z1_float
	// x2_float y2_float z2_float
	// .
	// .
	// xn_float yn_float zn_float
	//
	// # 多边形列表
	// surface_description_ushort num_verts_int v0_index_int v1_index_int ..  vn_index_int
	// .
	// .
	// surface_description_ushort num_verts_int v0_index_int v1_index_int ..  vn_index_int

	// lets keep it simple and assume one element per line
	// hence we have to find the object descriptor, read it in, then the
	// vertex list and read it in, and finally the polygon list -- simple :)

	// Step 1: clear out the object and initialize it a bit
	memset(&obj, 0, sizeof(Object));
	// 设置物体状态为活动、可见
	obj.state = OBJECT_STATE_ACTIVE | OBJECT_STATE_ACTIVE;

	// 设置物体世界坐标
	obj.worldPos = pos;

	// Step 2: open the file for reading
	fp.open(filename);
	if (fp.is_open())
	{
		std::cout<<"Couldn't open PLG file "<< filename;
		return(0);
	} // end if

	// Step 3: 读取物体描述符
	if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
	{
		std::cout<<"PLG file error with file "<< filename<<"(object descriptor invalid).";
		return(0);
	} // end if

	std::cout<<"Object Descriptor:"<< token_string;

	std::vector<char> buff(256);
	// 设置物体描述符
	sscanf(token_string, "%s %d %d", buff.data(), &obj.numVertices, &obj.numPolygons);
	buff[buff.size() - 1] = '\0';
	obj.name = std::string(buff.data());


	// Step 4: 顶点列表
	for (int vertex = 0; vertex < obj.numVertices; vertex++)
	{
		// get the next vertex
		if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
		{
			std::cout<<"PLG file error with file vertex list invalid.";
			return(0);
		} // end if

	// parse out vertex
		sscanf(token_string, "%f %f %f", &obj.vlistLocal[vertex].x,
			&obj.vlistLocal[vertex].y,
			&obj.vlistLocal[vertex].z);
		obj.vlistLocal[vertex].w = 1;

		// scale vertices
		obj.vlistLocal[vertex].x *= scale.x;
		obj.vlistLocal[vertex].y *= scale.y;
		obj.vlistLocal[vertex].z *= scale.z;
	} // end for vertex

	// 计算平均半径和最大半径
	ComputeObjectRadius(obj);


	int poly_surface_desc = 0; // PLG/PLX surface descriptor
	int poly_num_verts = 0; // number of vertices for current poly (always 3)
	char tmp_string[8];        // temp string to hold surface descriptor in and
							   // test if it need to be converted from hex

	// Step 5: load the polygon list
	for (int poly = 0; poly < obj.numPolygons; poly++)
	{
		// get the next polygon descriptor
		if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
		{
			std::cout<<"PLG file error "<<std::endl;
			return(0);
		} // end if

		// each vertex list MUST have 3 vertices since we made this a rule that all models
		// must be constructed of triangles
		// read in surface descriptor, number of vertices, and vertex list
		sscanf(token_string, "%s %d %d %d %d", tmp_string,
			&poly_num_verts, // should always be 3 
			&obj.plist[poly].vert[0],
			&obj.plist[poly].vert[1],
			&obj.plist[poly].vert[2]);


		// 面描述符可以是16进制
		if (tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
			sscanf(tmp_string, "%x", &poly_surface_desc);
		else
			poly_surface_desc = atoi(tmp_string);

		// 多边形顶点列表指向物体顶点列表
		obj.plist[poly].vlist = obj.vlistLocal;

		// now we that we have the vertex list and we have entered the polygon
		// vertex index data into the polygon itself, now let's analyze the surface
		// descriptor and set the fields for the polygon based on the description

		// extract out each field of data from the surface descriptor
		// first let's get the single/double sided stuff out of the way
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
			obj.plist[poly].color = (red <<4<<16) + (green <<4<<8)+ (blue<<4);
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
		obj.plist[poly].state = POLY4DV1_STATE_ACTIVE;

	} // end for poly

// close the file
	fp.close();

	// return success
	return(1);

} // end Load_OBJECT4DV1_PLG