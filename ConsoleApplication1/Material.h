#pragma once
#include<string>
#include "Color.h"
#include"Light.h"
#include<Windows.h>
#include <gdiplus.h>
namespace maki {
	using uchar = unsigned char;
	//string ת wstring
	std::wstring StringToWString(const std::string& str)
	{
		setlocale(LC_ALL, "chs");
		const char* point_to_source = str.c_str();
		size_t new_size = str.size() + 1;
		wchar_t *point_to_destination = new wchar_t[new_size];
		wmemset(point_to_destination, 0, new_size);
		size_t converted = 0;
		mbstowcs_s(&converted, point_to_destination, new_size, point_to_source, _TRUNCATE);
		std::wstring result = point_to_destination;
		delete[]point_to_destination;
		setlocale(LC_ALL, "C");
		return result;
	}

	class BitMap {
	public:
		BitMap() = default;
		BitMap(const std::string& file_name) {
			std::wstring infilename = StringToWString(file_name);
			//��ͼƬ
			Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(infilename.c_str());

			height = bmp->GetHeight();
			width = bmp->GetWidth();

			Gdiplus::Color color;
			pixel = new Color[height * width];
			int cur_idx = 0;
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					bmp->GetPixel(j, i, &color);
					pixel[cur_idx].r = color.GetRed();
					pixel[cur_idx].g = color.GetGreen();
					pixel[cur_idx].b = color.GetBlue();
					pixel[cur_idx].a = color.GetAlpha();
					++cur_idx;
				}
			}
			delete bmp;
		}
		//��������
		~BitMap() {
			if (pixel != nullptr) {
				delete[] pixel;
				pixel = nullptr;
			}				
		}

		//�������캯��
		BitMap(const BitMap& other){
			width = other.width;
			height = other.height;
			pixel = new Color[height * width];
			int cur_idx = 0;
			for (int i = 0; i < height; ++i) {
				for (int j = 0; j < width; ++j) {
					pixel[cur_idx] = other.pixel[cur_idx];
					++cur_idx;
				}
			}
		}

		//���
		BitMap& operator=(const BitMap& other)
		{
			if (this != &other) {
				if (width != other.width || height != other.height) {
					delete[]pixel;
					pixel = new Color[other.height * other.width];
				}				

				width = other.width;
				height = other.height;	
				int cur_idx = 0;
				for (int i = 0; i < height; ++i) {
					for (int j = 0; j < width; ++j) {
						pixel[cur_idx] = other.pixel[cur_idx];
						++cur_idx;
					}
				}
			}
			return *this;
		}

		int GetWidth() const { return width; }
		//��ȡ�߶�
		int GetHeight() const { return height; }
		//��ȡָ��λ������,����
		void GetPixel(int x, int y, Color& _pixel) const { _pixel = pixel[y * width + x]; }
		//��ȡָ��λ������,��������
		void GetPixel(int idx, Color& _pixel) const { _pixel = pixel[idx]; }
		//��ȡָ��λ�õ�ͨ������ɫֵ,����
		void GetSingleChannelColor(int x, int y, int& value) const { value = (int)pixel[y * width + x].r; }
		//��ȡָ��λ�õ�ͨ������ɫֵ,��������
		void GetSingleChannelColor(int idx, int& value) const { value = (int)pixel[idx].r; }

		//��ȡ��ͼ��ɫ(���ݱ������)
		bool GetTextureColor(float s, float t,Color& color) {
			if (width == 0 || height == 0|| s>1.0 || t>1.0) {
				return false;
			}			
			else
			{
				auto x = static_cast<int>(s * (width - 1));
				auto y = static_cast<int>(t * (height - 1));
				GetPixel(x, y, color);
				return true; 
			}
		}

	private:
		int width, height;  // size of bitmap
		Color* pixel;
	};
	/**********************************����************************************/
	class Material {
	public:
		//���캯��
		Material() = default;
		Material(const Color& _color, const std::string& _texturePath, float _ka, float _kd, float _ks, float _power)
			:color(_color), texturePath(_texturePath), ka(_ka), kd(_kd), ks(_ks), power(_power),texture(BitMap(_texturePath)){
			++GlobalId;
			id = GlobalId;
		}
		//�������������ɫ
		Color CalculateFinalColor(LightBase &light, const Vector4D& vertex_pos, const Vector4D& vertex_normal, const Vector4D& camera_pos) {
			if (light.GetType() == LightType::kAmbient) {
				//ֻ���㻷����
				return light.LightCalculate(vertex_pos, vertex_normal) * color * ka;
			}
			else
			{
				Color light_color = light.LightCalculate(vertex_pos, vertex_normal);
				auto light_vec = light.GetPos() - vertex_pos;
				auto reflect_vec = (vertex_normal * (2.0f * light_vec* vertex_normal)) - light_vec;//��������
				auto observe_vec = camera_pos - vertex_pos;
				//�������뾵�淴��
				return light_color * color * kd * (vertex_normal*light_vec)+ light_color * color * ks * (float)pow(max(reflect_vec*observe_vec, 0.0f), power);
			}
		}
		//��ȡ��ͼ����
		bool GetTexturePixel(float s, float t, Color& color) {
			return texture.GetTextureColor(s,t,color);
		}

		int id;              
		std::string name;    // ����
		Color color;            // ��ɫ
		float ka, kd, ks, power; // �Ի����⡢ɢ��⡢���淴���ķ���ϵ�������淴��ָ��

		bool hasTexture = false;
		std::string texturePath;  // ����λ��
		BitMap texture;    // actual texture map (if any)

		static int GlobalId;
	};

}