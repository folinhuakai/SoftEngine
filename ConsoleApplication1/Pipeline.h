#pragma once
#include<windows.h>
#include<vector>
#include"camera.h"
#include"material.h"
#include<list>
#include <future>
#include <limits.h>
#include"Object.h"
namespace maki {
	class Pipeline {
	public:
		//获取或删除单实例
		static Pipeline* GetInstance() {
			if (instance == nullptr) {
				instance = new Pipeline();
			}
			return instance;
		}

		//删除单实例
		static void DeleteInstance() {
			if (instance != nullptr) {
				delete instance;
			}
		}
		
		//获取hwnd
		inline HWND GetWindowHwnd() {
			return hwnd;
		};
		inline void SetWindowHwnd(HWND _hwnd) {
			hwnd = _hwnd;
		}
		//获取窗口宽度
		int GetWindowWidth() {
			return viewWidth;
		}
		//获取窗口高度
		int GetWindowHeight() {
			return viewHeight;
		}
		//获取帧缓冲
		unsigned char* GetFrameBuffer() {
			return frameBuffer;
		}
		//每帧更新
		void FrameUpdate() {
			std::fill_n(zBuffer, viewWidth * viewHeight, 2.0f);
			std::memset(frameBuffer, 0, viewWidth * viewHeight * 3 * sizeof(unsigned char));
			renderlist.polyData.clear();
			for (auto obj:objList) {
				obj->TransfromToWorld();
				obj->CullObject(mainCamera,CullType::kCullXYZPlane);
				//背面消除+光照+加入渲染列表
				BackfaceCulling(obj);
			}
			renderlist.WorldToCamera(mainCamera);
			renderlist.CameraToScreen(mainCamera);
			RasterizationDepthTest();//光栅化渲染列表

			SwapBuffer();
		}
		
		void AddObject(Object *obj) {
			objList.push_back(obj);
		}
		//背面消除+光照
		void BackfaceCulling(Object *obj) {		
			auto mesh = obj->mesh;
			Vector4D observe_vec;
			if (obj->state&ObjectState::kCull) {
				return;
			}
			int  faceIndex = 0;
			unsigned int idx0;
			unsigned int  idx1;
			unsigned int idx2 ;
			int mID ;
			for (int j = 0; j < mesh.vlistTtran.size(); j += 3, ++faceIndex) {
				observe_vec = mainCamera.pos - mesh.vlistTtran[mesh.index[j]].pos;
				if (mesh.faceNormal[faceIndex] * observe_vec < 0) {
					continue;
				}
				else
				{
					idx0 = mesh.index[j];
					idx1 = mesh.index[j + 1];
					idx2 = mesh.index[j + 2];
					mID = obj->matIdList[faceIndex];				
					
					//光照
					for (auto light : lightList) {
						if (light->state == LightState::kOff)
							continue;
						mesh.vlistTtran[idx0].color += matList[mID].CalculateFinalColor(*light, mesh.vlistTtran[idx0].pos, mesh.vlistTtran[idx0].normal,mainCamera.pos);
						mesh.vlistTtran[idx1].color += matList[mID].CalculateFinalColor(*light, mesh.vlistTtran[idx1].pos, mesh.vlistTtran[idx1].normal,mainCamera.pos);
						mesh.vlistTtran[idx2].color += matList[mID].CalculateFinalColor(*light, mesh.vlistTtran[idx2].pos, mesh.vlistTtran[idx2].normal,mainCamera.pos);	
					}
					
					//创建三角形,加入渲染列表
					Polygon tmp(mesh.vlistTtran[idx0],mesh.vlistTtran[idx1], mesh.vlistTtran[idx2],
						mesh.faceNormal[faceIndex], mID);
					renderlist.polyData.push_back(std::move(tmp));
				}
			}

		}
		//主摄像机
		Camera mainCamera;
		//创建cube
		void CreateCube(float width, float length, float height){
			Vector4D _worldPos(0, 0, -8);
			Vector4D _dir(30,0,0);
			Cube cube(width, length, height, _worldPos, _dir, 1.0f);

			objList.push_back(&cube);

			std::string texture_path = "obj_map_texture/container.jpg";
			Material cube_mat(Color(255, 255, 255), texture_path, 0.1f, 0.7f, 0.0f, 128);
			cube_mat.hasTexture = true;
			matList.push_back(cube_mat);
			cube_mat.id = matList.size()-1;

			//设置物体材质，-1表示无材质
			cube.matIdList.resize(cube.mesh.faceNormal.size(), -1);
			cube.matIdList[0] = cube_mat.id;
			cube.matIdList[1] = cube_mat.id;
		}
	protected:		
		//光栅化
		void RasterizationDepthTest() {
			auto _data = renderlist.polyData;
			for (int i = 0; i < _data.size();++i) {
				Vertex temp_v(0, 0, 0);
				Vertex v0 = _data[i].vlist[0];
				Vertex v1 = _data[i].vlist[1];
				Vertex v2 = _data[i].vlist[2];
				float dx_left = 0.0f, dx_right = 0.0f, dz_left = 0.0f, dz_right = 0.0f;

				//按照y值升序v0,v1,v2
				if (v0.pos.y > v1.pos.y) {
					temp_v = v0;
					v0 = v1;
					v1 = temp_v;
				}

				if (v1.pos.y > v2.pos.y) {
					temp_v = v1;
					v1 = v2;
					v2 = temp_v;
					if (v0.pos.y > v1.pos.y) {
						temp_v = v0;
						v0 = v1;
						v1 = temp_v;
					}
				}

				bool has_texture = false;
				if (_data[i].materialID != -1 && matList[_data[i].materialID].hasTexture) {
					has_texture = true;
				}

				if (v0.pos.y > viewHeight || v2.pos.y < 0)
				{
					continue;
				}

				int floor_v0_y = (int)(v0.pos.y + 0.5f);
				int floor_v1_y = (int)(v1.pos.y + 0.5f);
				int floor_v2_y = (int)(v2.pos.y + 0.5f);

				if (floor_v0_y == floor_v1_y && floor_v1_y == floor_v2_y)
				{
					continue;
				}

				//检查是否平底三角形
				if (floor_v1_y == floor_v2_y) {
					if (v1.pos.x < v2.pos.x) {
						DrawBottomTri(v0, v1, v2, has_texture, _data[i].materialID, 1.0f);
					}
					else {
						DrawBottomTri(v0, v2, v1, has_texture, _data[i].materialID, 1.0f);
					}
				}
				//检查是否平顶三角形
				else if (floor_v0_y == floor_v1_y) {
					if (v0.pos.x < v1.pos.x) {
						DrawTopTri(v0, v1, v2, has_texture, _data[i].materialID, 1.0f);
					}
					else {
						DrawTopTri(v1, v0, v2, has_texture, _data[i].materialID, 1.0f);
					}
				}
				//一般三角形先做分割
				else
				{
					float one_over_diffy = 1 / (v2.pos.y - v0.pos.y);
					float diffy_v1v0 = (v1.pos.y - v0.pos.y);
					//计算分割的新顶点的pos
					float new_x = v0.pos.x + diffy_v1v0 * (v2.pos.x - v0.pos.x) * one_over_diffy;
					float new_y = v1.pos.y;
					float new_z = v0.pos.z + diffy_v1v0 * (v2.pos.z - v0.pos.z) * one_over_diffy;
					//计算分割的新顶点的color
					unsigned char new_r = v0.color.r + static_cast<unsigned char>(diffy_v1v0 * (v2.color.r - v0.color.r) * one_over_diffy);
					unsigned char new_g = v0.color.g + static_cast<unsigned char>(diffy_v1v0 * (v2.color.g - v0.color.g) * one_over_diffy);
					unsigned char new_b = v0.color.b + static_cast<unsigned char>(diffy_v1v0 * (v2.color.b - v0.color.b) * one_over_diffy);
					//计算分割的新顶点的uv
					float new_u = (v0.uv.x*v0.pos.z + diffy_v1v0 * (v2.uv.x*v2.pos.z - v0.uv.x*v0.pos.z) * one_over_diffy) / new_z;
					float new_v = (v0.uv.y*v0.pos.z + diffy_v1v0 * (v2.uv.y*v2.pos.z - v0.uv.y*v0.pos.z) * one_over_diffy) / new_z;
					//设置新顶点的属性
					Vertex new_vertex(new_x, new_y, new_z, new_r, new_g, new_b);
					new_vertex.uv.x = new_u;
					new_vertex.uv.y = new_v;
					new_vertex.normal = _data[i].faceNormal;
					
					if (new_vertex.pos.y >= 0)
					{
						if (v1.pos.x < v2.pos.x) {
							DrawBottomTri(v0, v1, v2, has_texture, _data[i].materialID, 1.0f);
						}
						else {
							DrawBottomTri(v0, v2, v1, has_texture, _data[i].materialID, 1.0f);
						}
					}
					if (new_vertex.pos.y <= viewHeight)
					{
						if (v0.pos.x < v1.pos.x) {
							DrawTopTri(v0, v1, v2, has_texture, _data[i].materialID, 1.0f);
						}
						else {
							DrawTopTri(v1, v0, v2, has_texture, _data[i].materialID, 1.0f);
						}
					}
				}
			}
		}
		//平底三角形
		void DrawBottomTri(const Vertex& v0, const Vertex& v1, const Vertex& v2, bool has_texture, int32_t mat_id, float alpha) {
			//pos:x关于y的左边斜率，关于y的右边斜率，z关于y的左边斜率，z关于y的右边斜率
			float dx_left = 0.0f, dx_right = 0.0f, dz_left = 0.0f, dz_right = 0.0f;
			//color:r关于y的左边斜率，r关于y的右边斜率，g关于y的左边斜率，g关于y的右边斜率，b关于y的左边斜率，b关于y的右边斜率
			float dr_left = 0.0f, dr_right = 0.0f, dg_left = 0.0f, dg_right = 0.0f, db_left = 0.0f, db_right = 0.0f;
			//uv: s关于y的左边斜率，s关于y的右边斜率，t关于y的左边斜率，t关于y的右边斜率
			float ds_left = 0.0f, ds_right = 0.0f, dt_left = 0.0f, dt_right = 0.0f;
			//光栅化最终y的起始坐标和终止坐标
			int y_start, y_end;
			//每一行扫描x的起始坐标和终止坐标
			float x_start = 0.0f, x_end = 0.0f;
			//每一行扫描z的起始坐标和终止坐标
			float z_left = 0.0f, z_right = 0.0f;
			//每一行扫描r的起始坐标和终止坐标
			float r_left = 0.0f, r_right = 0.0f;
			//每一行扫描g的起始坐标和终止坐标
			float g_left = 0.0f, g_right = 0.0f;
			//每一行扫描b的起始坐标和终止坐标
			float b_left = 0.0f, b_right = 0.0f;
			//每一行扫描s和t的起始坐标和终止坐标
			float s_left = 0.0f, s_right = 0.0f, t_left = 0.0f, t_right = 0.0f;

			//三角形三个顶点中最小的x的值和最大的x值
			float min_x = 0.0f, max_x = 0.0f;
			//修复的x的起始和终止坐标用于三角形裁剪
			float fix_x_start = 0.0f, fix_x_end = 0.0f;
			//v0的s/z和t/z的值,用于三角形透视投影
			float v0_s_overz = 0.0f, v0_t_overz = 0.0f;

			//检查是否贴图，避免不必要计算
			if (has_texture) {
				v0_s_overz = v0.uv.x * v0.pos.z;
				v0_t_overz = v0.uv.y * v0.pos.z;
			}

			//计算左右两边斜率	默认v1.pos.x < v2.pos.x
			{
				float one_over_y_diff_left = 1 / (v0.pos.y - v1.pos.y);
				float one_over_y_diff_right = 1 / (v0.pos.y - v2.pos.y);
				//求x,z的y的左右梯度
				dx_left = (v0.pos.x - v1.pos.x) * one_over_y_diff_left;
				dx_right = (v0.pos.x - v2.pos.x) * one_over_y_diff_right;
				dz_left = (v0.pos.z - v1.pos.z) * one_over_y_diff_left;
				dz_right = (v0.pos.z - v2.pos.z) * one_over_y_diff_right;

				//求颜色r,g,b的y的左右梯度
				dr_left = (v0.color.r - v1.color.r) * one_over_y_diff_left;
				dr_right = (v0.color.r - v2.color.r) * one_over_y_diff_right;
				dg_left = (v0.color.g - v1.color.g) * one_over_y_diff_left;
				dg_right = (v0.color.g - v2.color.g) * one_over_y_diff_right;
				db_left = (v0.color.b - v1.color.b) * one_over_y_diff_left;
				db_right = (v0.color.b - v2.color.b) * one_over_y_diff_right;

				//检查是否贴图，避免不必要计算
				if (has_texture) {
					//求纹理s/z,t/z的y的左右梯度
					ds_left = (v0_s_overz - v1.uv.x * v1.pos.z) * one_over_y_diff_left;
					ds_right = (v0_s_overz - v2.uv.x * v2.pos.z) * one_over_y_diff_right;
					dt_left = (v0_t_overz - v1.uv.y* v1.pos.z) * one_over_y_diff_left;
					dt_right = (v0_t_overz - v2.uv.y * v2.pos.z) * one_over_y_diff_right;
				}

				//计算x最大值最小值,修正误差

				if (v2.pos.x < v0.pos.x) {
					min_x = v1.pos.x;
					max_x = v0.pos.x;
				}
				else
				{
					max_x = v2.pos.x;
					if (v0.pos.x < v1.pos.x) {
						min_x = v0.pos.x;
					}
					else
					{
						min_x = v1.pos.x;
					}

				}
			}

			min_x = min_x < 0 ? 0 : min_x > viewWidth ? viewWidth : min_x;
			max_x = max_x > viewWidth ? viewWidth : max_x < 0 ? 0 : max_x;

			//垂直裁剪
			y_start = v0.pos.y < 0 ? 0 : static_cast<int>(v0.pos.y + 0.5f);
			y_end = v2.pos.y > viewHeight - 1 ? viewHeight - 1 : static_cast<int>(v2.pos.y + 0.5f);

			float y_error = (y_start - v0.pos.y);
			//根据y值误差修正z,x,r,g,b,s,t
			z_left = y_error * dz_left + v0.pos.z;
			z_right = y_error * dz_right + v0.pos.z;
			x_start = y_error * dx_left + v0.pos.x;
			x_end = y_error * dx_right + v0.pos.x;

			r_left = y_error * dr_left + v0.color.r;
			r_right = y_error * dr_right + v0.color.r;
			g_left = y_error * dg_left + v0.color.g;
			g_right = y_error * dg_right + v0.color.g;
			b_left = y_error * db_left + v0.color.b;
			b_right = y_error * db_right + v0.color.b;

			//检查是否贴图
			if (has_texture) {
				s_left = y_error * ds_left + v0_s_overz;
				s_right = y_error * ds_right + v0_s_overz;
				t_left = y_error * dt_left + v0_t_overz;
				t_right = y_error * dt_right + v0_t_overz;
			}

			/*int mip_level = 0;
			int cur_level = 0;
			if (mat_id != -1)
			{
				mip_level = mat_vec_[mat_id].GetMipLevel();
			}
			if (mip_level > 1)
			{
				float ave_z = (v0.pos.z_ + v1.pos.z_ + v2.pos.z_) * 0.33f + 1.0f;
				cur_level = mip_level - 1 - (int)(mip_level * (ave_z / 2.0f));
			}*/

			int z_buffer_idx = y_start * viewWidth;

			for (int j = y_start; j <= y_end; ++j) {

				//z关于x的斜率
				float delta_zx = 0.0f;
				//r关于x的斜率
				float delta_rx = 0.0f;
				//g关于x的斜率
				float delta_gx = 0.0f;
				//b关于x的斜率
				float delta_bx = 0.0f;

				//s关于x的斜率
				float delta_sx = 0.0f;
				//t关于x的斜率
				float delta_tx = 0.0f;

				//分段线性纹理方法，只计算扫描线端点的准确纹理坐标，线性插值从左到右计算纹理坐标
				float one_over_z_left = 1 / z_left;
				float one_over_z_right = 1 / z_right;
				float s_left_cord = s_left * one_over_z_left;
				float s_right_cord = s_right * one_over_z_right;
				float t_left_cord = t_left * one_over_z_left;
				float t_right_cord = t_right * one_over_z_right;

				if (x_end - x_start != 0) {
					//计算z,r,g,b,s,t相对于x的误差
					float coeff = 1.0f / (x_end - x_start);
					delta_zx = (z_right - z_left) * coeff;
					delta_rx = (r_right - r_left) * coeff;
					delta_gx = (g_right - g_left) * coeff;
					delta_bx = (b_right - b_left) * coeff;

					//检查是否贴图，避免不必要计算
					if (has_texture) {
						delta_sx = (s_right_cord - s_left_cord) * coeff;
						delta_tx = (t_right_cord - t_left_cord) * coeff;
					}
				}

				//扫描中当前的z值，不直接用left防止裁剪导致的误差
				float z_cur = z_left;
				//扫描中当前的r值，不直接用left防止裁剪导致的误差
				float r_cur = r_left;
				//扫描中当前的g值，不直接用left防止裁剪导致的误差
				float g_cur = g_left;
				//扫描中当前的b值，不直接用left防止裁剪导致的误差
				float b_cur = b_left;
				//扫描中当前的s值，不直接用left防止裁剪导致的误差
				float s_cur = s_left_cord;
				//扫描中当前的t值，不直接用left防止裁剪导致的误差
				float t_cur = t_left_cord;
				//修复的x的起始值
				fix_x_start = x_start;
				//修复的x的终止值
				fix_x_end = x_end;

				//水平裁剪
				if (x_start < min_x) {
					float dx = min_x - x_start;
					//根据x值误差修正z,r,g,b,s,t
					z_cur += dx * delta_zx;
					r_cur += dx * delta_rx;
					g_cur += dx * delta_gx;
					b_cur += dx * delta_bx;

					//检查是否贴图，避免不必要计算
					if (has_texture) {
						s_cur += dx * delta_sx;
						t_cur += dx * delta_tx;
					}
					fix_x_start = min_x;
				}

				if (x_end < 0) {
					x_start += dx_left;
					x_end += dx_right;
					z_left += dz_left;
					z_right += dz_right;
					r_left += dr_left;
					r_right += dr_right;
					g_left += dg_left;
					g_right += dg_right;
					b_left += db_left;
					b_right += db_right;
					z_buffer_idx += viewWidth;

					//检查是否贴图，避免不必要计算
					if (has_texture) {
						s_left += ds_left;
						s_right += ds_right;
						t_left += dt_left;
						t_right += dt_right;
					}
					continue;
				}

				if (x_end > max_x) {
					fix_x_end = max_x;
				}

				int xs = static_cast<int>(fix_x_start + 0.5f);
				int xe = static_cast<int>(fix_x_end + 0.5f);

				for (int k = xs; k <= xe; ++k) {
					//CVV裁剪z
					if (z_cur >= -1 && z_cur <= 1) {
						//深度缓冲
						int z_buffer_index = z_buffer_idx + k;
						if (z_cur < zBuffer[z_buffer_index]) {
							zBuffer[z_buffer_index] = z_cur;
							Color final_color(static_cast<unsigned char>(r_cur), static_cast<unsigned char>(g_cur), static_cast<unsigned char>(b_cur));
							if (has_texture) {								
								float s_cord = s_cur < 0 ? 0 : s_cur > 1 ? 1 : s_cur;
								float t_cord = t_cur < 0 ? 0 : t_cur > 1 ? 1 : t_cur;
								Color texture_color;
								matList[mat_id].GetTexturePixel(s_cord, t_cord, texture_color);
								final_color = final_color * texture_color;
							}
							int dist = z_buffer_index * 3;
							frameBuffer[dist] = final_color.b;//Blue
							frameBuffer[dist + 1] = final_color.g;//Green
							frameBuffer[dist + 2] = final_color.r;//Red 
						}
					}
					z_cur += delta_zx;
					//限制颜色范围
					r_cur += delta_rx;
					r_cur = r_cur < 0 ? 0 : r_cur > 255 ? 255 : r_cur;
					g_cur += delta_gx;
					g_cur = g_cur < 0 ? 0 : g_cur > 255 ? 255 : g_cur;
					b_cur += delta_bx;
					b_cur = b_cur < 0 ? 0 : b_cur > 255 ? 255 : b_cur;

					//检查是否贴图，避免不必要计算
					if (has_texture) {
						//限制纹理坐标范围
						s_cur += delta_sx;
						s_cur = s_cur < 0 ? 0 : s_cur > 1 ? 1 : s_cur;
						t_cur += delta_tx;
						t_cur = t_cur < 0 ? 0 : t_cur > 1 ? 1 : t_cur;
					}
				}
				x_start += dx_left;
				x_end += dx_right;
				z_left += dz_left;
				z_right += dz_right;
				r_left += dr_left;
				r_right += dr_right;
				g_left += dg_left;
				g_right += dg_right;
				b_left += db_left;
				b_right += db_right;

				z_buffer_idx += viewWidth;
				//检查是否贴图，避免不必要计算
				if (has_texture) {
					s_left += ds_left;
					s_right += ds_right;
					t_left += dt_left;
					t_right += dt_right;
				}
			}
		}
		//平顶三角形
		void DrawTopTri(const Vertex& v0, const Vertex& v1, const Vertex& v2, bool has_texture, int32_t mat_id, float alpha) {
			//pos:x关于y的左边斜率，关于y的右边斜率，z关于y的左边斜率，z关于y的右边斜率
			float dx_left = 0.0f, dx_right = 0.0f, dz_left = 0.0f, dz_right = 0.0f;
			//color:r关于y的左边斜率，r关于y的右边斜率，g关于y的左边斜率，g关于y的右边斜率，b关于y的左边斜率，b关于y的右边斜率
			float dr_left = 0.0f, dr_right = 0.0, dg_left = 0.0f, dg_right = 0.0f, db_left = 0.0f, db_right = 0.0f;
			//uv: s关于y的左边斜率，s关于y的右边斜率，t关于y的左边斜率，t关于y的右边斜率
			float ds_left = 0.0f, ds_right = 0.0f, dt_left = 0.0f, dt_right = 0.0f;
			//光栅化最终y的起始坐标和终止坐标
			int y_start, y_end;
			//每一行扫描x的起始坐标和终止坐标
			float x_start = 0.0f, x_end = 0.0f;
			//每一行扫描z的起始坐标和终止坐标
			float z_left = 0.0f, z_right = 0.0f;
			//每一行扫描r的起始坐标和终止坐标
			float r_left = 0.0f, r_right = 0.0f;
			//每一行扫描g的起始坐标和终止坐标
			float g_left = 0.0f, g_right = 0.0f;
			//每一行扫描b的起始坐标和终止坐标
			float b_left = 0.0f, b_right = 0.0f;
			//每一行扫描s和t的起始坐标和终止坐标
			float s_left = 0.0f, s_right = 0.0f, t_left = 0.0f, t_right = 0.0f;

			//三角形三个顶点中最小的x的值和最大的x值
			float min_x = 0.0f, max_x = 0.0f;
			//修复的x的起始和终止坐标用于三角形裁剪
			float fix_x_start = 0.0f, fix_x_end = 0.0f;
			//v0的s/z和t/z的值,用于三角形透视投影
			float v2_s_overz = 0.0f, v2_t_overz = 0.0f;
			//检查是否贴图，避免不必要计算
			if (has_texture) {
				v2_s_overz = v2.uv.x * v2.pos.z;
				v2_t_overz = v2.uv.y * v2.pos.z;
			}
			//计算左右两边斜率
			{
				float one_over_y_diff_left = 1 / (v2.pos.y - v0.pos.y);
				float one_over_y_diff_right = 1 / (v2.pos.y - v1.pos.y);
				//求x,z的y的左右梯度
				dx_left = -(v2.pos.x - v0.pos.x) * one_over_y_diff_left;
				dx_right = -(v2.pos.x - v1.pos.x) * one_over_y_diff_right;
				dz_left = -(v2.pos.z - v0.pos.z) * one_over_y_diff_left;
				dz_right = -(v2.pos.z - v1.pos.z) * one_over_y_diff_right;

				//求颜色r,g,b的y的左右梯度
				dr_left = -(v2.color.r - v0.color.r) * one_over_y_diff_left;
				dr_right = -(v2.color.r - v1.color.r) * one_over_y_diff_right;
				dg_left = -(v2.color.g - v0.color.g) * one_over_y_diff_left;
				dg_right = -(v2.color.g - v1.color.g) * one_over_y_diff_right;
				db_left = -(v2.color.b - v0.color.b) * one_over_y_diff_left;
				db_right = -(v2.color.b - v1.color.b) * one_over_y_diff_right;

				//检查是否贴图，避免不必要计算
				if (has_texture) {
					//求纹理s/z,t/z的y的左右梯度
					ds_left = -(v2_s_overz - v0.uv.x * v0.pos.z) * one_over_y_diff_left;
					ds_right = -(v2_s_overz - v1.uv.x * v1.pos.z) * one_over_y_diff_right;
					dt_left = -(v2_t_overz - v0.uv.y * v0.pos.z) * one_over_y_diff_left;
					dt_right = -(v2_t_overz - v1.uv.y * v1.pos.z) * one_over_y_diff_right;
				}

				//计算x最大值最小值,修正误差
				if (v1.pos.x < v2.pos.x) {
					min_x = v0.pos.x;
					max_x = v2.pos.x;
				}
				else
				{
					max_x = v1.pos.x;
					if (v0.pos.x < v2.pos.x) {
						min_x = v0.pos.x;
					}
					else
					{
						min_x = v2.pos.x;
					}
				}
			}
		

			min_x = min_x < 0 ? 0 : min_x > viewWidth ? viewWidth : min_x;
			max_x = max_x > viewWidth ? viewWidth : max_x < 0 ? 0 : max_x;

			//垂直裁剪
			y_end = v0.pos.y < 0 ? 0 : static_cast<int>(v0.pos.y + 0.5f);
			y_start = v2.pos.y > viewHeight - 1 ? viewHeight - 1 : static_cast<int>(v2.pos.y + 0.5f);

			float y_error = (v2.pos.y - y_start);
			//根据y值误差修正z,x,r,g,b,s,t
			z_left = y_error * dz_left + v2.pos.z;
			z_right = y_error * dz_right + v2.pos.z;
			x_start = y_error * dx_left + v2.pos.x;
			x_end = y_error * dx_right + v2.pos.x;

			r_left = y_error * dr_left + v2.color.r;
			r_right = y_error * dr_right + v2.color.r;
			g_left = y_error * dg_left + v2.color.g;
			g_right = y_error * dg_right + v2.color.g;
			b_left = y_error * db_left + v2.color.b;
			b_right = y_error * db_right + v2.color.b;

			//检查是否贴图，避免不必要计算
			if (has_texture) {
				s_left = y_error * ds_left + v2_s_overz;
				s_right = y_error * ds_right + v2_s_overz;
				t_left = y_error * dt_left + v2_t_overz;
				t_right = y_error * dt_right + v2_t_overz;
			}

			int z_buffer_idx = y_start * viewWidth;

			for (int j = y_start; j > y_end; --j) {
				//z关于x的斜率
				float delta_zx = 0.0f;
				//r关于x的斜率
				float delta_rx = 0.0f;
				//g关于x的斜率
				float delta_gx = 0.0f;
				//b关于x的斜率
				float delta_bx = 0.0f;

				//s关于x的斜率
				float delta_sx = 0.0f;
				//t关于x的斜率
				float delta_tx = 0.0f;

				//分段线性纹理方法，只计算扫描线端点的准确纹理坐标，线性插值从左到右计算纹理坐标
				float one_over_z_left = 1 / z_left;
				float one_over_z_right = 1 / z_right;
				float s_left_cord = s_left * one_over_z_left;
				float s_right_cord = s_right * one_over_z_right;
				float t_left_cord = t_left * one_over_z_left;
				float t_right_cord = t_right * one_over_z_right;

				if (x_end - x_start != 0) {
					float coeff = 1.0f / (x_end - x_start);
					//计算z,r,g,b,s,t相对于x的梯度
					delta_zx = (z_right - z_left) * coeff;
					delta_rx = (r_right - r_left) * coeff;
					delta_gx = (g_right - g_left) * coeff;
					delta_bx = (b_right - b_left) * coeff;

					//检查是否贴图，避免不必要计算
					if (has_texture) {
						delta_sx = (s_right_cord - s_left_cord) * coeff;
						delta_tx = (t_right_cord - t_left_cord) * coeff;
					}
				}
				//扫描中当前的z值，不直接用left防止裁剪导致的误差
				float z_cur = z_left;
				//扫描中当前的r值，不直接用left防止裁剪导致的误差
				float r_cur = r_left;
				//扫描中当前的g值，不直接用left防止裁剪导致的误差
				float g_cur = g_left;
				//扫描中当前的b值，不直接用left防止裁剪导致的误差
				float b_cur = b_left;
				//扫描中当前的s值，不直接用left防止裁剪导致的误差
				float s_cur = s_left_cord;
				//扫描中当前的t值，不直接用left防止裁剪导致的误差
				float t_cur = t_left_cord;
				//修复的x的起始值
				fix_x_start = x_start;
				//修复的x的终止值
				fix_x_end = x_end;
				//水平裁剪
				if (x_start < min_x) {
					float dx = min_x - x_start;
					//根据x值误差修正z,r,g,b,s,t
					z_cur += dx * delta_zx;
					r_cur += dx * delta_rx;
					g_cur += dx * delta_gx;
					b_cur += dx * delta_bx;

					//检查是否贴图，避免不必要计算
					if (has_texture) {
						s_cur += dx * delta_sx;
						t_cur += dx * delta_tx;
					}
					fix_x_start = min_x;
				}

				if (x_end < 0) {
					x_start += dx_left;
					x_end += dx_right;
					z_left += dz_left;
					z_right += dz_right;
					r_left += dr_left;
					r_right += dr_right;
					g_left += dg_left;
					g_right += dg_right;
					b_left += db_left;
					b_right += db_right;
					z_buffer_idx -= viewWidth;

					//检查是否贴图，避免不必要计算
					if (has_texture) {
						s_left += ds_left;
						s_right += ds_right;
						t_left += dt_left;
						t_right += dt_right;
					}
					continue;
				}

				if (fix_x_end > max_x) {
					fix_x_end = max_x;
				}

				int xs = static_cast<int>(fix_x_start + 0.5f);
				int xe = static_cast<int>(fix_x_end + 0.5f);
				for (int k = xs; k <= xe; ++k) {
					//CVV裁剪z
					if (z_cur >= -1 && z_cur <= 1) {
						//int z_buffer_index = j * view_width_ + k;
						int z_buffer_index = z_buffer_idx + k;
						//深度缓冲检测
						if (z_cur < zBuffer[z_buffer_index]) {
							zBuffer[z_buffer_index] = z_cur;
							Color final_color(static_cast<unsigned char>(r_cur), static_cast<unsigned char>(g_cur), static_cast<unsigned char>(b_cur));
							if (has_texture) {
								float s_cord = s_cur < 0 ? 0 : s_cur > 1 ? 1 : s_cur;
								float t_cord = t_cur < 0 ? 0 : t_cur > 1 ? 1 : t_cur;
								Color texture_color;
								matList[mat_id].GetTexturePixel(s_cord, t_cord, texture_color);
								final_color = final_color * texture_color;
							}
							int dist = z_buffer_index * 3;
							int dist = z_buffer_index * 3;
							frameBuffer[dist] = final_color.b;//Blue
							frameBuffer[dist + 1] = final_color.g;//Green
							frameBuffer[dist + 2] = final_color.r;//Red 
						}
						z_cur += delta_zx;
						//限制颜色范围
						r_cur += delta_rx;
						r_cur = r_cur < 0 ? 0 : r_cur > 255 ? 255 : r_cur;
						g_cur += delta_gx;
						g_cur = g_cur < 0 ? 0 : g_cur > 255 ? 255 : g_cur;
						b_cur += delta_bx;
						b_cur = b_cur < 0 ? 0 : b_cur > 255 ? 255 : b_cur;

						//检查是否贴图，避免不必要计算
						if (has_texture) {
							//限制纹理坐标范围
							s_cur += delta_sx;
							s_cur = s_cur < 0 ? 0 : s_cur > 1 ? 1 : s_cur;
							t_cur += delta_tx;
							t_cur = t_cur < 0 ? 0 : t_cur > 1 ? 1 : t_cur;
						}

					}
				}
				x_start += dx_left;
				x_end += dx_right;
				z_left += dz_left;
				z_right += dz_right;
				r_left += dr_left;
				r_right += dr_right;
				g_left += dg_left;
				g_right += dg_right;
				b_left += db_left;
				b_right += db_right;
				z_buffer_idx -= viewWidth;

				//检查是否贴图，避免不必要计算
				if (has_texture) {
					s_left += ds_left;
					s_right += ds_right;
					t_left += dt_left;
					t_right += dt_right;
				}
			}
		}
		//双缓冲交换
		void SwapBuffer() {
			RECT rc;
			GetClientRect(hwnd, &rc);
			//创建GDI+掩码位图（画布）
			Gdiplus::Bitmap bitmap(viewWidth, viewHeight, 3 * viewWidth, PixelFormat24bppRGB, frameBuffer);
			Gdiplus::Graphics bmp_graphics(&bitmap);
			bmp_graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

			HDC hdc = GetDC(hwnd);
			Gdiplus::Graphics graphics(hdc);
			Gdiplus::CachedBitmap cachedBmp(&bitmap, &graphics);
			graphics.DrawCachedBitmap(&cachedBmp, 0, 0);
			ReleaseDC(hwnd, hdc);
		}
		//获取场景AABB包围盒
		void getSceneAABB(bool include_transparent = false);
	private:
		//视口宽度
		int viewWidth{ 800 };
		//视口高度
		int viewHeight{ 600 };
		//光源数组
		std::vector<LightBase*> lightList;
		//深度缓冲
		float* zBuffer = nullptr;
		//帧缓冲
		unsigned char* frameBuffer = nullptr;
		//渲染窗口句柄
		HWND hwnd;
		//材质数组
		std::vector<Material> matList;
		//物体列表
		std::vector<Object*> objList;
		//单实例
		static Pipeline* instance;
		RenderList renderlist;
		//构造函数
		Pipeline() {
			//相机初始化
			Point4D camPos{ 1.5f, 1.5f, 3,1.0f };
			Point4D camdir = Point4D{ 60.0f,60.0f,0.0f,1.0f };
			Camera ca1;	
			ca1.InitCamera(CameraType::kModeUvn, camPos, &camdir,nullptr,50.0f, 500.0f, 90, viewWidth, viewHeight);
			ca1.BuildCameraMatrixUVN(CameraUvnMode::kSpherical);
			//光照初始化
			lightList.push_back(&PointLight(Color(255, 255, 0), Vector4D(0.0f, 10.0f, -20.0f), 1.0, 0.0f, 0.0f));
			lightList.push_back(&AmbientLight(Color(255, 255, 0)));
			lightList.push_back(&DirectionLight(Color(255, 255, 0), Vector4D(-1.0f, 0.0f, 0.0f)));
			
			zBuffer = new float[viewWidth * viewHeight]();
			std::fill_n(zBuffer, viewWidth * viewHeight, 2.0f);
			frameBuffer = new unsigned char[viewWidth * viewHeight * 3]();
			CreateCube(10.0f,10.0f,10.0f);
		}
		//析构函数
		~Pipeline() {
			delete[] zBuffer;
			delete[] frameBuffer;
		}
	};
}