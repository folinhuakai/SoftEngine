#pragma once
#include "Camera.h"
#include <string>
#include "Math.h"
#include "Light.h"


namespace maki{
	constexpr auto MAX_VERTICES = 128; //������󶥵���
	constexpr auto MAX_POLYS = 128;//�������������
	constexpr auto MAX_RENDER_POLYGON = 128;//��Ⱦ�б����������
	

	//����� attr
	constexpr auto POLY4DV1_ATTR_2SIDED = 0x0001;
	constexpr auto POLY4DV1_ATTR_TRANSPARENT = 0x0002;
	constexpr auto POLY4DV1_ATTR_8BITCOLOR = 0x0004;
	constexpr auto POLY4DV1_ATTR_RGB16 = 0x0008;
	constexpr auto POLY4DV1_ATTR_RGB24 = 0x0010;

	constexpr auto POLY4DV1_ATTR_SHADE_MODE_PURE = 0x0020;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_CONSTANT = 0x0020;// (alias)
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_FLAT = 0x0040;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_GOURAUD = 0x0080;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_PHONG = 0x0100;
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_FASTPHONG = 0x0100;// (alias)
	constexpr auto POLY4DV1_ATTR_SHADE_MODE_TEXTURE = 0x0200;

	// �����states
	enum  PloygonStates
	{
		kInit = 0,
		kActive = 0x0001,//��Ծ
		kClipped = 0x0002,//�ü�
		kBackface = 0x0004//����
	};
	enum class TransfromType
	{
		kLocalOnly,
		kTransOnly,
		kLocalToTrans
	};	
	enum  ObjectState
	{
		kActiveS	= 0x0001,
		kVisible	= 0x0002,
		kCull		= 0x0004,	
	};


	enum CullType {
		kCullXPlane = 0x0001,
		kCullYPlane = 0x0002,
		kCullZPlane = 0x0004,
		kCullXYZPlane = 0x0007,
	};
	/**************************�����***************************/
	class Polygon
	{
	public:
		Polygon(){}
		~Polygon() {}
		int state{ PloygonStates::kInit };//״̬
		int attr{ 0 };//��������
		int color{ 0 };//��ɫ
		Point4D *vlist{};//�����б�
		int vert[3];//�����б���Ԫ������
	};

	/***********************�԰��������******************************/
	class PolygonFull
	{
	public:
		PolygonFull() {}
		~PolygonFull() {}
		int state{ PloygonStates::kInit };//״̬
		int attr{ 0 };
		int color{ 0 };

		Point4D vlist[3];//�����ζ���
		Point4D tvlist[3];//�任�󶥵�

		Polygon *next{};
		Polygon *prev{};
		
	};

	/*************************����************************************/
	class Object
	{
	public:
		Object() = default;
		~Object() {}
		int id{ 0 };//�����ʶ
		std::string name;//����
		int state{ 0 };//״̬
		int attr{ 0 };//����
		float avgRadius{ 0 };//����ƽ���뾶
		float maxRadius{ 0 };//���뾶
		Vector4D worldPos;//��������
		Vector4D dir;//�����ھֲ�����ϵ����ת�Ƕ�
		Vector4D ux, uy, uz;//����ֲ����꣬���ڴ洢����ĵĳ�����ת�ڼ佫���Զ�����
		int numVertices{ 0 };//���嶥����
		Point4D vlistLocal[MAX_VERTICES];//���ڴ洢����ֲ����������
		Point4D vlistTransl[MAX_VERTICES];//�洢�任��Ķ�����������
		int numPolygons{ 0 };//��������������
		Polygon plist[MAX_POLYS];//���������
		//������Ķ����б���б任	//@parm obj��Ҫ�任�����壬mat �任���� ��type ָ��Ҫ�任���б� isChange �Ƿ�Գ��������任
		void TransfromObject(Matrix<float, 4, 4> &mat,const TransfromType type, bool isChange) {
			switch (type) {
			case TransfromType::kLocalOnly:
				for (int i = 0; i < numVertices; ++i) {
					vlistLocal[i] = vlistLocal[i] * mat;
				}
				break;
			case TransfromType::kTransOnly:
				for (int i = 0; i < numVertices; ++i) {
					vlistTransl[i] = vlistTransl[i] * mat;
				}
				break;
			case TransfromType::kLocalToTrans:
				for (int i = 0; i < numVertices; ++i) {
					vlistTransl[i] = vlistLocal[i] * mat;
				}
				break;
			default:
				break;
			}

			//�Ƿ�任���峯��
			if (isChange) {
				ux = ux * mat;
				uy = uy * mat;
				uz = uz * mat;
			}
		}
		// ������Ӿֲ�����ת������������(ֱ�ӷ�)
		void TransfromToWorld(const TransfromType type = TransfromType::kLocalToTrans) {
			if (type == TransfromType::kLocalToTrans) {
				for (int i = 0; i < numVertices; ++i) {
					vlistTransl[i] = vlistLocal[i] + worldPos;
				}
			}
			else {//TransfromType::kTransOnly
				for (int i = 0; i < numVertices; ++i) {
					vlistTransl[i] = vlistTransl[i] + worldPos;
				}
			}

		}
		// ����
		void TransfromToWorldMat(TransfromType type = TransfromType::kLocalToTrans,bool isChange = true) {
			Matrix<float, 4, 4> mat = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				worldPos.x,worldPos.y,worldPos.z,1 };
			TransfromObject(mat, TransfromType::kLocalToTrans, isChange);
		}
		//�������굽�������
		void WorldToCamera(Camera &cam)
		{//��object�е�vlistTrans�Ķ�����б任����������Ķ����ѱ任Ϊ�������꣬���������vlistTrans
			for (int vertex = 0; vertex < numVertices; ++vertex)
			{
				vlistTransl[vertex] = vlistTransl[vertex] * cam.mcam;
			}
		}
		//������굽��Ļ����任
		void CameraToScreen(Camera &cam) {

			float alpha = (0.5*cam.viewPortWidth - 0.5);
			float beta = (0.5*cam.viewPortHeight - 0.5);

			for (int vertex = 0; vertex < numVertices; ++vertex) {
				float z = vlistTransl[vertex].z;

				// transform the vertex by the view parameters in the camera
				vlistTransl[vertex].x = cam.viewDist * vlistTransl[vertex].x / z;
				vlistTransl[vertex].y = cam.viewDist * vlistTransl[vertex].y*cam.aspectRatio / z;
				// z = z, so no change

				vlistTransl[vertex].x = alpha + alpha * vlistTransl[vertex].x;
				vlistTransl[vertex].y = beta - beta * vlistTransl[vertex].y;
			} // end for vertex

		} // end Camera_To_P

		//͸�ӱ任
		void CameraToPerspective(Camera &cam) {
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				float z = vlistTransl[vertex].z;

				// transform the vertex by the view parameters in the camera
				vlistTransl[vertex].x = cam.viewDist * vlistTransl[vertex].x / z;
				vlistTransl[vertex].y = cam.viewDist * vlistTransl[vertex].y * cam.aspectRatio / z;
			} // end for vertex
		}

		//�ӿڱ任
		void PerspectiveToScreen(Camera &cam) {
			float alpha = (0.5*cam.viewPortWidth - 0.5);
			float beta = (0.5*cam.viewPortHeight - 0.5);

			for (int vertex = 0; vertex < numVertices; ++vertex) {

				// transform the vertex by the view parameters in the camera
				vlistTransl[vertex].x = alpha + alpha * vlistTransl[vertex].x;
				vlistTransl[vertex].y = beta - beta * vlistTransl[vertex].y;

			} // end for vertex
		}

		// �����޳�
		bool CullObject(Camera &cam, CullType cullflags) {//���������Ϣ�ж������Ƿ����Ӿ����ڣ�cullflagsָ������Щ����ִ���޳�
		//1.����Χ�����ı任���������
			auto spherePos = worldPos * cam.mcam;

			// 2: ����cullflags�޳�����
			if (cullflags & CullType::kCullZPlane) {//Զ���ü���
				if (((spherePos.z - maxRadius) > cam.farClipZ) ||
					((spherePos.z + maxRadius) < cam.nearClipZ)) {
					state = state | ObjectState::kCull;
					return false;
				}
			} // end if

			if (cullflags & CullType::kCullXPlane) {//���Ҳü��棨ʹ���������ƣ�

				float z_test = (0.5)*cam.viewPlaneWidth*spherePos.z / cam.viewDist;

				if (((spherePos.x - maxRadius) > z_test) || // right side
					((spherePos.x + maxRadius) < -z_test))  // left side, note sign change
				{
					state = state | ObjectState::kCull;
					return false;
				} // end if
			} // end if

			if (cullflags & CullType::kCullYPlane) {//���²ü��棨ʹ���������ƣ�
				float z_test = (0.5)*cam.viewPlaneHeight *spherePos.z / cam.viewDist;
				if (((spherePos.y - maxRadius) > z_test) || // top side
					((spherePos.y + maxRadius) < -z_test))  // bottom side, note sign change
				{
					state = state | ObjectState::kCull;
					return false;
				} // end if

			} // end if

			return true;
		}
		// ��������
		void RemoveBackfaces(Camera cam) {
			//����vlistTrans�ж������ݼ����λ����������ı������Σ��淨�����ӵ���������ˣ�<90�㼴���棩
			if (state & ObjectState::kCull)
				return;

			// ����ÿ�������
			for (int poly = 0; poly < numPolygons; ++poly)
			{
				// acquire polygon
				auto currPoly = plist[poly];
				// �ж϶������Ч��
				if (!(currPoly.state & PloygonStates::kActive) ||
					(currPoly.state & PloygonStates::kBackface) ||
					(currPoly.attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly.state & PloygonStates::kClipped))
					continue; // move onto next poly

				 // ��ö����±꣨�����԰�����
				int vindex_0 = currPoly.vert[0];
				int vindex_1 = currPoly.vert[1];
				int vindex_2 = currPoly.vert[2];

				//���������淨�ߣ������ǰ�˳ʱ�뷽������
				auto u = vlistTransl[vindex_1] - vlistTransl[vindex_0];//p0->p1
				auto v = vlistTransl[vindex_2] - vlistTransl[vindex_0];//p0->p2
				auto n = u.Cross(v);//u x v

				auto view = cam.pos - vlistTransl[vindex_0];//�ӵ�ָ����������

				// ������
				float dp = n * view;

				// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
				if (dp <= 0.0) {
					currPoly.state = currPoly.state & PloygonStates::kBackface;
				}

			} // end for poly

		} // end

		//������壨�ü��ͱ��棩״̬��ʶ
		void ResetOjbectState() {
			//��������޳�״̬
			state = state & (!ObjectState::kCull);

			// ���ö���βü��ͱ����޳����
			for (int poly = 0; poly < numPolygons; poly++)
			{
				auto curPoly = plist[poly];
				if (!(curPoly.state &PloygonStates::kActive)) {
					continue;
				}
				curPoly.state = curPoly.state & (!PloygonStates::kClipped);
				curPoly.state = curPoly.state & (!PloygonStates::kBackface);

			}
		}

		//���ռ���
		bool LightInWorld(Camera &cam, std::vector<Light>  &lights){
			//���ݹ�Դ�б�����������ִ�й��ռ��㣬֧�̶ֹ���ɫ�ͺ㶨��ɫ
			unsigned int r_base, g_base, b_base,  // ��ʼ��ɫ
				r_sum, g_sum, b_sum,   // ȫ������
				shaded_color;          // ������ɫ

			float dp,     // ���
				dist,   // ����͹�Դ����
				i,      // ǿ��
				nl,     // ���߳���
				atten;  // ˥��������

		  // test if the object is culled
			if (!(state & ObjectState::kActiveS) ||
				(state & ObjectState::kCull) ||
				!(state & ObjectState::kVisible))
				return false;

			// process each poly in mesh
			for (int poly = 0; poly < numPolygons; poly++)
			{
				// acquire polygon
				auto currPoly = plist[poly];

				// �ж϶������Ч��
				if (!(currPoly.state & PloygonStates::kActive) ||
					(currPoly.state & PloygonStates::kBackface) ||
					(currPoly.attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly.state & PloygonStates::kClipped))
					continue; // move onto next poly

				 // ��ö����±꣨�����԰�����
				int vindex_0 = currPoly.vert[0];
				int vindex_1 = currPoly.vert[1];
				int vindex_2 = currPoly.vert[2];
				// we will use the transformed polygon vertex list since the backface removal
				// only makes sense at the world coord stage further of the pipeline 

				// test the lighting mode of the polygon (use flat for flat, gouraud))
				if (curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_FLAT || curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_GOURAUD)
				{
					// step 1: extract the base color out in RGB mode
					if (dd_pixel_format == DD_PIXEL_FORMAT565)
					{
						_RGB565FROM16BIT(curr_poly->color, &r_base, &g_base, &b_base);

						// scale to 8 bit 
						r_base <<= 3;
						g_base <<= 2;
						b_base <<= 3;
					} // end if
					else
					{
						_RGB555FROM16BIT(curr_poly->color, &r_base, &g_base, &b_base);

						// scale to 8 bit 
						r_base <<= 3;
						g_base <<= 3;
						b_base <<= 3;
					} // end if

				 // initialize color sum
					r_sum = 0;
					g_sum = 0;
					b_sum = 0;

					// loop thru lights
					for (int curr_light = 0; curr_light < max_lights; curr_light++)
					{
						// is this light active
						if (lights[curr_light].state == LIGHTV1_STATE_OFF)
							continue;

						// what kind of light are we dealing with
						if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)
						{
							// simply multiply each channel against the color of the 
							// polygon then divide by 256 to scale back to 0..255
							// use a shift in real life!!! >> 8
							r_sum += ((lights[curr_light].c_ambient.r * r_base) / 256);
							g_sum += ((lights[curr_light].c_ambient.g * g_base) / 256);
							b_sum += ((lights[curr_light].c_ambient.b * b_base) / 256);

							// there better only be one ambient light!

						} // end if
						else
							if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)
							{
								// infinite lighting, we need the surface normal, and the direction
								// of the light source

								// we need to compute the normal of this polygon face, and recall
								// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
								VECTOR4D u, v, n;

								// build u, v
								VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
								VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);

								// compute cross product
								VECTOR4D_Cross(&u, &v, &n);

								// at this point, we are almost ready, but we have to normalize the normal vector!
								// this is a key optimization we can make later, we can pre-compute the length of all polygon
								// normals, so this step can be optimized
								// compute length of normal
								nl = VECTOR4D_Length_Fast(&n);

								// ok, recalling the lighting model for infinite lights
								// I(d)dir = I0dir * Cldir
								// and for the diffuse model
								// Itotald =   Rsdiffuse*Idiffuse * (n . l)
								// so we basically need to multiple it all together
								// notice the scaling by 128, I want to avoid floating point calculations, not because they 
								// are slower, but the conversion to and from cost cycles

								dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);

								// only add light if dp > 0
								if (dp > 0)
								{
									i = 128 * dp / nl;
									r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
									g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
									b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
								} // end if

							} // end if infinite light
							else
								if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)
								{
									// perform point light computations
									// light model for point light is once again:
									//              I0point * Clpoint
									//  I(d)point = ___________________
									//              kc +  kl*d + kq*d2              
									//
									//  Where d = |p - s|
									// thus it's almost identical to the infinite light, but attenuates as a function
									// of distance from the point source to the surface point being lit

									// we need to compute the normal of this polygon face, and recall
									// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
									VECTOR4D u, v, n, l;

									// build u, v
									VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
									VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);

									// compute cross product
									VECTOR4D_Cross(&u, &v, &n);

									// at this point, we are almost ready, but we have to normalize the normal vector!
									// this is a key optimization we can make later, we can pre-compute the length of all polygon
									// normals, so this step can be optimized
									// compute length of normal
									nl = VECTOR4D_Length_Fast(&n);

									// compute vector from surface to light
									VECTOR4D_Build(&obj->vlist_trans[vindex_0], &lights[curr_light].pos, &l);

									// compute distance and attenuation
									dist = VECTOR4D_Length_Fast(&l);

									// and for the diffuse model
									// Itotald =   Rsdiffuse*Idiffuse * (n . l)
									// so we basically need to multiple it all together
									// notice the scaling by 128, I want to avoid floating point calculations, not because they 
									// are slower, but the conversion to and from cost cycles
									dp = VECTOR4D_Dot(&n, &l);

									// only add light if dp > 0
									if (dp > 0)
									{
										atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);

										i = 128 * dp / (nl * dist * atten);

										r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
										g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
										b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
									} // end if

								} // end if point
								else
									if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
									{
										// perform spotlight/point computations simplified model that uses
										// point light WITH a direction to simulate a spotlight
										// light model for point light is once again:
										//              I0point * Clpoint
										//  I(d)point = ___________________
										//              kc +  kl*d + kq*d2              
										//
										//  Where d = |p - s|
										// thus it's almost identical to the infinite light, but attenuates as a function
										// of distance from the point source to the surface point being lit

										// we need to compute the normal of this polygon face, and recall
										// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
										VECTOR4D u, v, n, l;

										// build u, v
										VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
										VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);

										// compute cross product (we need -n, so do vxu)
										VECTOR4D_Cross(&v, &u, &n);

										// at this point, we are almost ready, but we have to normalize the normal vector!
										// this is a key optimization we can make later, we can pre-compute the length of all polygon
										// normals, so this step can be optimized
										// compute length of normal
										nl = VECTOR4D_Length_Fast(&n);

										// compute vector from surface to light
										VECTOR4D_Build(&obj->vlist_trans[vindex_0], &lights[curr_light].pos, &l);

										// compute distance and attenuation
										dist = VECTOR4D_Length_Fast(&l);

										// and for the diffuse model
										// Itotald =   Rsdiffuse*Idiffuse * (n . l)
										// so we basically need to multiple it all together
										// notice the scaling by 128, I want to avoid floating point calculations, not because they 
										// are slower, but the conversion to and from cost cycles

										// note that I use the direction of the light here rather than a the vector to the light
										// thus we are taking orientation into account which is similar to the spotlight model
										dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);

										// only add light if dp > 0
										if (dp > 0)
										{
											atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);

											i = 128 * dp / (nl * atten);

											r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
											g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
											b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
										} // end if

									} // end if spotlight1
									else
										if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2) // simple version
										{
											// perform spot light computations
											// light model for spot light simple version is once again:
											//         	     I0spotlight * Clspotlight * MAX( (l . s), 0)^pf                     
											// I(d)spotlight = __________________________________________      
											//               		 kc + kl*d + kq*d2        
											// Where d = |p - s|, and pf = power factor

											// thus it's almost identical to the point, but has the extra term in the numerator
											// relating the angle between the light source and the point on the surface

											// we need to compute the normal of this polygon face, and recall
											// that the vertices are in cw order, u=p0->p1, v=p0->p2, n=uxv
											VECTOR4D u, v, n, d, s;

											// build u, v
											VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
											VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);

											// compute cross product (v x u, to invert n)
											VECTOR4D_Cross(&v, &u, &n);

											// at this point, we are almost ready, but we have to normalize the normal vector!
											// this is a key optimization we can make later, we can pre-compute the length of all polygon
											// normals, so this step can be optimized
											// compute length of normal
											nl = VECTOR4D_Length_Fast(&n);

											// and for the diffuse model
											// Itotald =   Rsdiffuse*Idiffuse * (n . l)
											// so we basically need to multiple it all together
											// notice the scaling by 128, I want to avoid floating point calculations, not because they 
											// are slower, but the conversion to and from cost cycles
											dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);

											// only add light if dp > 0
											if (dp > 0)
											{
												// compute vector from light to surface (different from l which IS the light dir)
												VECTOR4D_Build(&lights[curr_light].pos, &obj->vlist_trans[vindex_0], &s);

												// compute length of s (distance to light source) to normalize s for lighting calc
												dist = VECTOR4D_Length_Fast(&s);

												// compute spot light term (s . l)
												float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir) / dist;

												// proceed only if term is positive
												if (dpsl > 0)
												{
													// compute attenuation
													atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);

													// for speed reasons, pf exponents that are less that 1.0 are out of the question, and exponents
													// must be integral
													float dpsl_exp = dpsl;

													// exponentiate for positive integral powers
													for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
														dpsl_exp *= dpsl;

													// now dpsl_exp holds (dpsl)^pf power which is of course (s . l)^pf 

													i = 128 * dp * dpsl_exp / (nl * atten);

													r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
													g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
													b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);

												} // end if

											} // end if

										} // end if spot light

					} // end for light

				// make sure colors aren't out of range
					if (r_sum > 255) r_sum = 255;
					if (g_sum > 255) g_sum = 255;
					if (b_sum > 255) b_sum = 255;

					// write the color
					shaded_color = RGB16Bit(r_sum, g_sum, b_sum);
					curr_poly->color = (int)((shaded_color << 16) | curr_poly->color);

				} // end if
				else // assume POLY4DV1_ATTR_SHADE_MODE_CONSTANT
				{
					// emmisive shading only, copy base color into upper 16-bits
					// without any change
					curr_poly->color = (int)((curr_poly->color << 16) | curr_poly->color);
				} // end if

			} // end for poly

		// return success
			return(1);

		} // end Light_OBJECT4DV1_World16
	};


	/************************��������б�*************************************/
	class RenderList {
	public:
		RenderList(){}
		~RenderList() {}
		void Reset() {
			numPolys = 0;
		}
		int state{ 0 };//״̬
		int attr{ 0 };//����
		//��Ⱦ�б���һ��ָ�����飬����ÿ��ָ��ָ��һ���԰����ġ�����Ⱦ�Ķ����
		//����Ҫ����zֵ����ʱ���޸ĵ�����Ⱦ�б�
		PolygonFull* polyPtrs[MAX_RENDER_POLYGON];
		//����δ�����飬Ϊ����ÿ֡��Ϊ����η���/�ͷ��ڴ�
		PolygonFull polyData[MAX_RENDER_POLYGON];
		int numPolys{ 0 };//��Ⱦ�б��а����Ķ������Ŀ
		
		//����Ⱦ�б�Ķ���α任	//@parm rederList ��Ⱦ�б�mat �任���� ��type ָ��Ҫ�任������
		void TransformRenderList(Matrix<float, 4, 4> &mat, TransfromType type) {
			switch (type) {
			case TransfromType::kLocalOnly:
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->vlist[vetNum];
						curPoly->vlist[vetNum] = vect * mat;
					}
				}
				break;
			case TransfromType::kTransOnly:
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->tvlist[vetNum];
						curPoly->tvlist[vetNum] = vect * mat;
					}
				}
				break;
			case TransfromType::kLocalToTrans:
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						auto vect = curPoly->vlist[vetNum];
						curPoly->tvlist[vetNum] = vect * mat;
					}
				}
				break;
			default:
				break;
			}
		}
		
		//ת������������
		void TransfromToWorld(Vector4D worldPos,TransfromType type = TransfromType::kLocalToTrans) {
			if (type == TransfromType::kLocalToTrans) {
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						curPoly->tvlist[vetNum] = curPoly->vlist[vetNum] + worldPos;
					}
				}
			}
			else {//TransfromType::kTransOnly
				for (int i = 0; i < numPolys; ++i) {
					auto curPoly = polyPtrs[i];
					if (curPoly == nullptr ||
						!(curPoly->state &PloygonStates::kActive) ||
						curPoly->state &PloygonStates::kClipped ||
						curPoly->state &PloygonStates::kBackface) {
						continue;
					}
					for (int vetNum = 0; vetNum < 3; ++vetNum) {
						curPoly->tvlist[vetNum] = curPoly->tvlist[vetNum] + worldPos;
					}
				}
			}

		}
	    
		//��������-���������
		void WorldToCamera(Camera &cam)
		{//��������任������Ⱦ�б��еĶ���λ�Ϊ�������
			for (int i = 0; i < numPolys; ++i) {
				auto curPoly = polyPtrs[i];
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
		
		//͸�ӱ任(�Ǿ��󷨣����������Ⱦ�б�Ķ�����ѱ��任Ϊ�������)
		void CameraToPerspective(Camera &cam) {
			for (int poly = 0; poly < numPolys; poly++)
			{
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

			 // all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++)
				{
					float z = currPoly->tvlist[vertex].z;

					// transform the vertex by the view parameters in the camera
					currPoly->tvlist[vertex].x = cam.viewDist*currPoly->tvlist[vertex].x / z;
					currPoly->tvlist[vertex].y = cam.viewDist*currPoly->tvlist[vertex].y*cam.aspectRatio / z;

				} // end for vertex

			} // end for poly

		} // end CameraToPerspective

		//�ӿڱ任
		void PerspectiveToScreen(Camera &cam) {
			//��Ⱦ�б������͸�ӱ任����һ����͸������-����Ļ����
			for (int poly = 0; poly < numPolys; poly++){
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				float alpha = (0.5*cam.viewPortWidth - 0.5);
				float beta = (0.5*cam.viewPortHeight - 0.5);

				// all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++)
				{//�������ţ���תy��
					currPoly->tvlist[vertex].x = alpha + alpha * currPoly->tvlist[vertex].x;
					currPoly->tvlist[vertex].y = beta - beta * currPoly->tvlist[vertex].y;
				} // end for vertex

			} // end for poly	
		}
		
		  //������굽��Ļ����ת����ֱ�ӷ���
		void CameraToScreen(Camera &cam) {
			for (int poly = 0; poly < numPolys; poly++){
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				float alpha = (0.5*cam.viewPortWidth - 0.5);
				float beta = (0.5*cam.viewPortHeight - 0.5);

				// all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++){
					float z = currPoly->tvlist[vertex].z;

					// transform the vertex by the view parameters in the camera
					currPoly->tvlist[vertex].x = cam.viewDist*currPoly->tvlist[vertex].x / z;
					currPoly->tvlist[vertex].y = cam.viewDist*currPoly->tvlist[vertex].y*cam.aspectRatio / z;

					//�������ţ���תy��
					currPoly->tvlist[vertex].x = alpha + alpha * currPoly->tvlist[vertex].x;
					currPoly->tvlist[vertex].y = beta - beta * currPoly->tvlist[vertex].y;

				} // end for vertex

			} // end for poly

		} // end 
		
		//��������
		void RemoveBackfaces(Camera cam) {
			// ���ö���α���״̬
			for (int poly = 0; poly < numPolys; ++poly)
			{
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

				//���������淨�ߣ������ǰ�˳ʱ�뷽������
				auto u = currPoly->tvlist[1] - currPoly->tvlist[0];//p0->p1
				auto v = currPoly->tvlist[2] - currPoly->tvlist[0];//p0->p2
				auto n = u.Cross(v);//u x v

				auto view = cam.pos - currPoly->tvlist[0];//�ӵ�ָ����������

				// ������
				float dp = n * view;

				// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
				if (dp <= 0.0) {
					currPoly->state = currPoly->state & PloygonStates::kBackface;
				}

			} // end for poly

		} // end RemoveBackfacesRenderlist
		
		//���������תΪ���������
		void ConvertFromHomo4D() {
			for (int poly = 0; poly < numPolys; poly++)
			{
				// acquire current polygon
				auto currPoly = polyPtrs[poly];

				if ((currPoly == nullptr) || !(currPoly->state & PloygonStates::kActive) ||
					(currPoly->state & PloygonStates::kClipped) ||
					(currPoly->attr  & POLY4DV1_ATTR_2SIDED) ||
					(currPoly->state & PloygonStates::kBackface))
					continue; // move onto next poly

			 // all good, let's transform 
				for (int vertex = 0; vertex < 3; vertex++)
				{
					currPoly->tvlist[vertex].DivByW();
				} // end for vertex

			} // end for poly		

		}



	};
	
	inline std::ostream & operator <<(std::ostream &out, const Object &obj) {
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

	inline bool operator ==(const Object&a, const Object&b) {
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
}