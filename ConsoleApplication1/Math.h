#pragma once
#include<iostream>
#include <initializer_list>
class Polar2D;
class Spherical3D;
class Cylindrical;
using FloatType = float;
const float PI = 3.141592654f;
const float PI_2 = 6.283185307f;
const float PI_DIV_2 = 1.570796327f;
const float PI_DIV_4 = 0.785398163f;
//非常小的数的常量
const float EPSILON_E4 = 1E-4f;
const float EPSILON_E5 = 1E-5f;
const float EPSILON_E6 = 1E-6f;
//参数化直线交点
const int PARM_LINE_NO_INTERSECT = 0;
const int PARM_LINE_NO_INTERSECT_IN_SEGMENT = 1;
const int PARM_LINE_INTERSECT_OUT_SEGMENT = 2;
const int PARM_LINE_EVERYWHERE = 3;

//比较两个数大小并返回
template <class T>
inline T Min(T a, T b) {
	return (a < b) ? a : b;
}
template <class T>
inline T Max(T a, T b) {
	return (a > b) ? a : b;
}
//交换数据
template <class T>
inline void Swap(T& a, T& b) {
	T tmp;
	tmp = a;
	a = b;
	b = tmp;
}
//转换角度和弧度
inline FloatType DegToRad(FloatType ang) {
	return ang * PI / 180.0f;
}
inline FloatType RadToDeg(FloatType rads) {
	return rads * 180.0f / PI;
}
//三角函数函数，查表法
extern FloatType sinLook[361];
extern FloatType cosLook[361];
FloatType FastSin(FloatType theta);
FloatType FastCos(FloatType theta);

/******************2维点或向量***************/
class Vector2D {
public:
	//构造函数
	Vector2D() :x(0.0f), y(0.0f) {}
	Vector2D(FloatType x, FloatType y) : x(x), y(y) {}
	Vector2D(const Vector2D& v) : x(v.x), y(v.y) {}	
	~Vector2D() {};

	FloatType Length()const{//向量长度
		return sqrtf(x*x + y * y);
	}
	bool Normalize(){//归一化
		auto length = Length();

		if (length < EPSILON_E5)
			return false;
		auto inv = 1 / length;
		x *= inv;
		y *= inv;
		return true;
	}	
	void Vector2DZero() {
		x = 0.0f;
		y = 0.0f;
	}	
	Vector2D& operator =(const Vector2D &v) {
		if (this != &v) {
			x = v.x;
			y = v.y;
		}
		return *this;
	}
	Polar2D ConvertToPolar2D()const;//转换成极坐标
	//创建从init到term的向量
	friend void BuildVector2D(const Vector2D& init, const Vector2D& term, Vector2D& result);	
	FloatType x, y;
};

inline const Vector2D operator +(const Vector2D &a, const Vector2D &b) {
	Vector2D sum(a.x + b.x, a.y + b.y);
	return sum;
}
inline const Vector2D operator -(const Vector2D &a, const Vector2D &b) {
	Vector2D sub(a.x - b.x, a.y - b.y);
	return sub;
}
inline FloatType operator *(const Vector2D &a, const Vector2D &b) {
	return(b.x*a.x + b.y*a.y);
}
inline Vector2D operator *(const FloatType k, const Vector2D &v) {
	Vector2D scale(v.x*k, v.y *k);
	return scale;
}
inline Vector2D operator *(const Vector2D &v, const FloatType k) {
	Vector2D scale(v.x*k, v.y *k);
	return scale;
}

inline void BuildVector2D(const Vector2D& init, const Vector2D& term, Vector2D& result) {
	result.x = term.x - init.x;
	result.y = term.y - init.y;
}
inline FloatType CosTh(const Vector2D& a, const Vector2D& b) {
	auto tmp = a * b / (a.Length() *b.Length());
	return tmp;
}
inline std::ostream & operator <<(std::ostream &out, const Vector2D &vec) {
	out << "x = " << vec.x << " y = " << vec.y << std::endl;
	return out;
}

/******************3维点或向量***************/
class Vector3D {
public:
	Vector3D() :x(0), y(0), z(0) {}
	Vector3D(FloatType x, FloatType y, FloatType z) :x(x), y(y), z(z) {}
	Vector3D(const Vector3D& v) : x(v.x), y(v.y), z(v.z) {}
	void Vector3DZero() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	Vector3D& operator =(const Vector3D &v) {
		if (this != &v) {
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}
	Cylindrical ConvertToCylindrical();
	Spherical3D ConvertToSpherical3D();
	~Vector3D() {}
	Vector3D Cross(const Vector3D &b)const{//叉乘
		Vector3D cross(y*b.z - b.y*z, b.x*z - x * b.z, x*b.y - b.x*y);
		return cross;
	}
	FloatType Length()const{//向量长度
		return sqrtf(x*x + y * y + z * z);
	}
	void Normalize(){//归一化
		auto length = Length();

		if (length < EPSILON_E5)
			return;
		auto inv = 1 / length;
		x *= inv;
		y *= inv;
		z *= inv;
	}	
	FloatType x, y, z;
};


inline Vector3D operator +(const Vector3D &a, const Vector3D &b) {
	Vector3D sum(a.x + b.x, a.y + b.y, a.z + b.z);
	return sum;
}
inline Vector3D operator -(const Vector3D &a, const Vector3D &b) {
	Vector3D sub(a.x - b.x, a.y - b.y, a.z - b.z);
	return sub;
}

inline FloatType operator *(const Vector3D &a, const Vector3D &b) {
	return(b.x*a.x + b.y*a.y + b.z*a.z);
}
inline Vector3D operator *(const FloatType k, const Vector3D &a) {
	Vector3D scale(a.x*k, a.y *k, a.z*k);
	return scale;
}
inline Vector3D operator *(const Vector3D &a, const FloatType k) {
	return (k*a);
}
inline Vector3D BuildVector3D(const Vector3D& init, const Vector3D& term) {
	Vector3D result;
	result.x = term.x - init.x;
	result.y = term.y - init.y;
	result.z = term.z - init.z;
	return result;
}
inline FloatType CosTh(const Vector3D& a, const Vector3D& b) {
	auto tmp = a * b / (a.Length() *b.Length());
	return tmp;
}
inline std::ostream & operator <<(std::ostream &out, const Vector3D &vec) {
	out << "x = " << vec.x << " y = " << vec.y << " z = " << vec.z << std::endl;
	return out;
}
/******************4维点或向量，用于齐次坐标***************/
class Vector4D{
public:
	Vector4D() :x(0), y(0), z(0),w(0) {}
	Vector4D(FloatType x, FloatType y, FloatType z, FloatType w) :x(x), y(y), z(z),w(w) {}
	Vector4D(const Vector4D& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	void Vector4DZero() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}
	Vector4D& operator =(const Vector4D &v) {
		if (this != &v) {
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}
		return *this;
	}
	~Vector4D() {}
	Vector4D Cross(const Vector4D &b)const {//叉乘
		Vector4D cross(y*b.z - b.y*z, b.x*z - x * b.z, x*b.y - b.x*y,1);
		return cross;
	}
	FloatType Length()const {//向量长度
		return sqrtf(x*x + y * y + z * z);
	}
	void Normalize() {//归一化
		auto length = Length();

		if (length < EPSILON_E5)
			return;
		auto inv = 1 / length;
		x *= inv;
		y *= inv;
		z *= inv;
		w = 1;
	}
	FloatType x, y, z,w;
};

inline Vector4D operator +(const Vector4D &a, const Vector4D &b) {
	Vector4D sum(a.x + b.x, a.y + b.y, a.z + b.z, 1);
	return sum;
}
inline Vector4D operator -(const Vector4D &a, const Vector4D &b) {
	Vector4D sub(a.x - b.x, a.y - b.y, a.z - b.z, 1);
	return sub;
}

inline FloatType operator *(const Vector4D &a, const Vector4D &b) {
	return(b.x*a.x + b.y*a.y + b.z*a.z);
}
inline Vector4D operator *(const FloatType k, const Vector4D &a) {
	Vector4D scale(a.x*k, a.y *k, a.z*k, 1);
	return scale;
}
inline Vector4D operator *(const Vector4D &a, const FloatType k) {
	return (k*a);
}
inline Vector4D BuildVector4D(const Vector4D& init, const Vector4D& term) {
	Vector4D result;
	result.x = term.x - init.x;
	result.y = term.y - init.y;
	result.z = term.z - init.z;
	result.w = 1;
	return result;
}
inline FloatType CosTh(const Vector4D& a, const Vector4D& b) {
	auto tmp = a * b / (a.Length() *b.Length());
	return tmp;
}
inline std::ostream & operator <<(std::ostream &out,const Vector4D &vec) {
	out << "x = " << vec.x << " y = " << vec.y << " z = " << vec.z << " w = " << vec.w << std::endl;
	return out;
}

/******************四元数***************/
class Quat {
public:
	Quat():w(0), x(0), y(0), z(0) {}
	Quat(FloatType w,FloatType x, FloatType y, FloatType z):w(w),x(x),y(y),z(z){}
	Quat(const Quat& v):w(v.w),x(v.x),y(v.y),z(v.z){}
	Quat(const Vector3D& v,FloatType theta){
		//方向向量v必须是单位向量，角度单位为弧度
		FloatType theta_div = 0.5 * theta;
		FloatType theta_sinf = sinf(theta_div);
		w = cosf(theta_div);
		x = theta_sinf * v.x;
		y = theta_sinf * v.y;
		z = theta_sinf * v.z;
	}
	Quat(const Vector4D& v,FloatType theta){
		//方向向量v必须是单位向量，角度单位为弧度
		FloatType theta_div = 0.5 * theta;
		FloatType theta_sinf = sinf(theta_div);
		w = cosf(theta_div);
		x = theta_sinf * v.x;
		y = theta_sinf * v.y;
		z = theta_sinf * v.z;
	}
	Quat(FloatType theta_z,FloatType theta_y,FloatType theta_x){
		//根据绕x、y、z旋转的角度，创建对应的四元数
		FloatType cos_z = 0.5 * cosf(theta_z);
		FloatType cos_y = 0.5 * cosf(theta_y);
		FloatType cos_x = 0.5 * cosf(theta_x);

		FloatType sin_z = 0.5* sinf(theta_z);
		FloatType sin_x = 0.5* sinf(theta_x);
		FloatType sin_y = 0.5* sinf(theta_y);

		w = cos_z*cos_y*cos_x + sin_z*sin_y*sin_x;
		x = cos_z*cos_y*sin_x - sin_z*sin_y*cos_x;
		y = cos_z*sin_y*cos_x + sin_z*cos_y*sin_x;
		z = sin_z*cos_y*cos_x - cos_z*sin_y*sin_x;
	}
	~Quat() {}
	Quat& operator =(const Quat &v) {
		if (this != &v) {
			w = v.w;
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}
	void QuatZero() {
		w = 0.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;		
	}
	void ConvToVector3DAndTheta(Vector3D &v,FloatType &theta) {
		//将单位四元数转换成单位向量和角度
		theta = acosf(w);
		FloatType sinf_theta_inv = 1.0 / sinf(theta);
		v.x = x * sinf_theta_inv;
		v.y = y * sinf_theta_inv;
		v.z = z * sinf_theta_inv;

		theta *= 2;
	}
	Quat Conjufate() {
		Quat q;
		q.w = w;
		q.x = -x;
		q.y = -y;
		q.z = -z;
		return q;
	}
	FloatType Length() {
		return sqrt(w*w + x * x + y * y + z * z);
	}
	FloatType Length2() {
		return (w*w + x * x + y * y + z * z);
	}
	void Normalize() {//归一化
		auto length = Length();

		if (length < EPSILON_E5)
			return;
		auto inv = 1 / length;
		w *= inv;
		x *= inv;
		y *= inv;
		z *= inv;		
	}
	Quat UnitInverse() {
		return Conjufate();
	}
	Quat Inverse() {
		auto inv = 1.0 / Length2();

		Quat q;
		q.w =  w * inv;
		q.x = -x * inv;
		q.y = -y * inv;
		q.z = -z * inv;
		return q;
	}
	FloatType w,x,y,z;
};

inline Quat operator +(const Quat &a, const Quat &b) {
	Quat sum(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);
	return sum;
}
inline Vector4D operator -(const Quat &a, const Quat &b) {
	Vector4D sub(a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z);
	return sub;
}

inline Quat operator *(const Quat &a, const Quat &b) {//a*b!=b*a 除非a,b为乘法单位数
	FloatType ret0 = (a.z - a.y) *(b.y - b.z);
	FloatType ret1 = (a.w + a.x) *(b.w + b.x);
	FloatType ret2 = (a.w - a.x) *(b.y + b.z);
	FloatType ret3 = (a.y + a.z) *(b.w - b.x);
	FloatType ret4 = (a.z - a.x) *(b.x - b.y);
	FloatType ret5 = (a.z + a.x) *(b.x + b.y);
	FloatType ret6 = (a.w + a.y) *(b.w - b.z);
	FloatType ret7 = (a.w - a.y) *(b.w + b.z);
	FloatType ret8 = ret5 + ret6 + ret7;
	FloatType ret9 = 0.5*(ret4 + ret8);

	Quat mul(ret0 + ret9 - ret5, ret1 + ret9 - ret8, ret2 + ret9 - ret7, ret3 + ret9 - ret6);
	return mul;

}
inline Quat operator *(const FloatType k, const Quat &a) {
	Quat scale(a.w*k, a.x*k, a.y *k, a.z*k);
	return scale;
}
inline Quat operator *(const Quat &a, const FloatType k) {
	return (k*a);
}
inline std::ostream & operator <<(std::ostream &out,const Quat &vec) {
	out << " w = " << vec.w << " x = " << vec.x << " y = " << vec.y << " z = " << vec.z << std::endl;
	return out;
}

/******************矩阵模板***************/
template<class T, int ROW, int COL> class Matrix {
public:
	Matrix() {//默认构造函数，初始化为0
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; ++j)
				M[i][j] = 0;
		}
	}
	Matrix(T(&m)[ROW*COL]) {
		try {
			for (int i = 0; i < ROW; ++i) {
				for (int j = 0; j < COL; ++j)
					M[i][j] = m[i * COL + j];
			}
		}
		catch (const char* msg) {
			std::cout << msg << std::endl;
		}
	}
	Matrix(const std::initializer_list<T> &args) {
		try {
			if (args.size() < ROW*COL) {
				throw "error:init data count less than ROW*COL";
			}
			auto beg = args.begin();
			for (int i = 0; i < ROW; ++i) {
				for (int j = 0; j < COL; j++) {
					M[i][j] = *beg;
					++beg;
				}

			}
		}
		catch (const char* msg) {
			std::cout << msg << std::endl;
		}
	}
	Matrix(const Matrix<T, ROW, COL>& m) {
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; j++)
				M[i][j] = m[i][j];
		}
	}
	Matrix(const T pm[][COL]) {
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; j++)
				M[i][j] = pm[i][j];
		}
	}
	Matrix<T, ROW, COL> operator +(const Matrix<T, ROW, COL> &m)const {
		T tmp[ROW][COL];
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; j++) {
				tmp[i][j] = M[i][j] + m[i][j];
			}
		}
		Matrix<T, ROW, COL> res(tmp);
		return res;
	}
	const T* operator [](int i)const {
		try {
			if (i >= ROW && i < 0) {
				throw "i out of range!";
			}
			auto p = M[i];
			return p;
		}
		catch (const char* msg) {
			std::cout << msg << std::endl;
		}
	}
	Matrix<T, ROW, COL>& operator =(const Matrix<T, ROW, COL> &v) {
		if (this != &v) {
			T ret[ROW][COL];
			v.GetRawDate(ret);
			for (int i = 0; i < ROW; ++i) {
				for (int j = 0; j < COL; ++j) {
					M[i][j] = ret[i][j];
				}
			}
		}
		
		return *this;
	}

	Matrix<T, COL, ROW> Transpose()const {
		T tmp[COL][ROW];
		for (int j = 0; j < COL; ++j) {
			for (int i = 0; i < ROW; ++i) {
				tmp[j][i] = M[i][j];
			}
		}
		Matrix<T, COL, ROW> res(tmp);
		return res;
	}

	void GetRawDate(T(&int_ref)[ROW][COL])const {
		try {
			for (int i = 0; i < ROW; ++i) {
				for (int j = 0; j < COL; ++j) {
					int_ref[i][j] = M[i][j];
				}
			}
		}
		catch (const char* msg) {
			std::cout << msg << std::endl;
		}
	}
	void SetRawDate(int i, int j, T val) {
		try {
			if (i >= ROW || j >= COL) {
				throw "ROW or column is higher";
			}
			M[i][j] = val;
		}
		catch (const char* msg) {
			std::cout << msg << std::endl;
		}
	}

private:
	T M[ROW][COL];
	static const int _row = ROW;
	static const int _col = COL;
};

//打印矩阵
template<typename T, int row, int col>
inline std::ostream & operator <<(std::ostream &out, const Matrix<T, row, col> &m) {
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			std::cout << m[i][j] << " ";
		}
		std::cout << std::endl;
	}
	return out;
}
//矩阵乘法，只要满足矩阵乘法条件的两个矩阵都可使用这个函数实现
template<class T, int nRow, int col, int nCol>
inline Matrix<T, nRow, nCol> operator *(const Matrix<T, nRow, col> &m1, const Matrix<T, col, nCol> &m2) {
	try {

		T tmp[nRow*nCol];

		for (int i = 0; i < nRow; ++i) {
			for (int j = 0; j < nCol; j++) {
				T sum = 0;
				for (int k = 0; k < col; ++k) {
					sum += (m1[i][k] * m2[k][j]);
				}
				tmp[i*nCol + j] = sum;
			}

		}
		Matrix<T, nRow, nCol> res(tmp);
		return res;
	}
	catch (const char* msg) {
		std::cout << msg;
	}

}

//矩阵乘法，相当于缩放
template<class T,int row,int col>
inline Matrix<T, row, col> operator *(const Matrix<T, row, col> &m1,float k) {
	try {

		T tmp[row][col];

		for (int i = 0; i < row; ++i) {
			for (int j = 0; j < col; j++) {
				tmp[i][j] = m1[i][j] * k;
			}

		}
		Matrix<T, row, col> res(tmp);
		return res;
	}
	catch (const char* msg) {
		std::cout << msg;
	}

}

//矩阵乘法，相当于缩放
template<class T,int row,int col>
inline Matrix<T, row, col> operator *(float k ,const Matrix<T, row, col> &m1) {
	return (m1 * k);
}

//计算行列式
template<int N>
inline FloatType MatrixDet(const Matrix<FloatType, N, N> &m) {

	FloatType ret[N][N];
	m.GetRawDate(ret);
	int maxLine = 0;
	//标记交换行的次数
	int sign = 0;
	for (int col = 0; col < N - 1; ++col) {
		maxLine = col;
		FloatType Max = ret[col][col];

		//判断每列的最大数，并将其放到主对角线上
		for (int row = col + 1; row < N; ++row) {
			FloatType value = ret[row][col];
			if (fabs(Max) < fabs(value)) {
				//当某一行大于k时将其赋值给k，并将其所在行标记下来
				Max = ret[row][col];
				maxLine = row;
			}

		}
		//判断矩阵是否为奇异阵
		if (fabs(Max) < 0.000001) {
			printf("行列式的值是: 0 ");
			break;
		}

		//当maxLine不等于col时交换这两行的所有元素
		if (maxLine != col) {
			++sign;
			for (int j = col; j < N; ++j) {
				Swap(ret[maxLine][j], ret[col][j]);
			}
		}
		//消去过程，化为上三角的形式
		for (int row = col + 1; row < N; row++) {
			//
			auto tmp = ret[row][col] / ret[col][col];
			FloatType val;
			for (int k = col; k < N; k++)
				ret[row][k] = ret[row][k] - ret[col][k] * tmp;
		}
	}
	//求化简后行列式的值
	FloatType tmp = ret[0][0];
	for (int i = 1; i < N; ++i)
		tmp *= ret[i][i];
	if (sign % 2 != 0)
		tmp = -tmp;
	return tmp;
}

//矩阵求逆，采用LU分解方法
template<int N>
inline bool InverseMatrix(const Matrix<FloatType, N, N> &m, Matrix<FloatType, N, N>& result) {
	FloatType ret[N][N];
	m.GetRawDate(ret);
	FloatType u[N][N];
	FloatType l[N][N];
	// 计算L第1列，对角线都为1，不计算，用于存在U矩阵的对角线
	for (int row = 1; row < N; ++row)
		ret[row][0] = ret[row][0] / ret[0][0];
	//LU分解
	for (int row = 1; row < N; ++row) {
		//计算U矩阵
		for (int col = row; col < N; ++col) {
			FloatType sum = 0;
			for (int t = 0; t < row; ++t) {
				sum += ret[row][t] * ret[t][col];
			}

			ret[row][col] = ret[row][col] - sum;

		}
		//计算L矩阵
		for (int rowl = row + 1; rowl < N; ++rowl) {
			FloatType sum = 0;
			for (int t = 0; t < row; ++t) {
				sum += ret[rowl][t] * ret[t][row];
			}
			ret[rowl][row] = (ret[rowl][row] - sum) / ret[row][row];
		}
	}
	for (int t = 0; t < N; ++t) {
		if (fabs(ret[t][t]) < EPSILON_E4) {
			return false;
		}
	}
	for (int row = 0; row < N; ++row) {
		for (int col = 0; col < N; ++col) {
			u[row][col] = 0;
			l[row][col] = 0;
		}
	}
	//u矩阵求逆
	for (int col = 0; col < N; ++col) {
		u[col][col] = 1 / ret[col][col];

		for (int row = col - 1; row >= 0; --row) {
			FloatType sum = 0;
			for (int t = row + 1; t <= col; ++t) {
				sum += ret[row][t] * u[t][col];
			}
			u[row][col] = -sum / u[row][row];
		}
	}
	//l矩阵求逆
	for (int col = 0; col < N; ++col) {
		l[col][col] = 1;
		for (int row = col + 1; row < N; ++row) {
			FloatType sum = 0;
			for (int t = col; t < row; ++t) {
				l[row][col] = l[row][col] - ret[row][t] * l[t][col];
			}
		}
	}
	Matrix<FloatType, N, N> U(u);
	Matrix<FloatType, N, N> L(l);
	std::cout << "U:" << std::endl;
	std::cout << U;
	std::cout << "L:" << std::endl;
	std::cout << L;

	result = U * L;
	std::cout << "result:" << std::endl;
	std::cout << result;

	return true;
}

//2*2矩阵求逆
inline bool InverseMatrix2X2(const Matrix<FloatType, 2, 2> &m, Matrix<FloatType, 2, 2>& result) {
	auto det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	if (det < EPSILON_E4) {
		return false;
	}
	else {
		result.SetRawDate(0, 0, m[1][1] / det);
		result.SetRawDate(0, 1, -m[0][1] / det);
		result.SetRawDate(1, 0, -m[1][0] / det);
		result.SetRawDate(1, 1, m[0][0] / det);
		return true;
	}
}

//3*3矩阵求逆，使用余子式、代数余子式、伴随矩阵求逆
inline bool InverseMatrix3X3(const Matrix<FloatType, 3, 3> &m, Matrix<FloatType, 3, 3>& result) {
	auto det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
		- m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2])
		+ m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]);
	if (det < EPSILON_E4) {
		return false;
	}
	else {
		auto det_inv = 1 / det;
		result.SetRawDate(0, 0, det_inv*(m[1][1] * m[2][2] - m[2][1] * m[1][2]));
		result.SetRawDate(1, 0, -det_inv * (m[1][0] * m[2][2] - m[2][0] * m[1][2]));
		result.SetRawDate(2, 0, det_inv*(m[1][0] * m[2][1] - m[2][0] * m[1][1]));

		result.SetRawDate(0, 1, -det_inv * (m[0][1] * m[2][2] - m[2][1] * m[0][2]));
		result.SetRawDate(1, 1, det_inv*(m[0][0] * m[2][2] - m[2][0] * m[0][2]));
		result.SetRawDate(2, 1, -det_inv * (m[0][0] * m[2][1] - m[2][0] * m[0][1]));

		result.SetRawDate(0, 2, det_inv*(m[0][1] * m[1][2] - m[1][1] * m[0][2]));
		result.SetRawDate(1, 2, -det_inv * (m[0][0] * m[1][2] - m[1][0] * m[0][2]));
		result.SetRawDate(2, 2, det_inv*(m[0][0] * m[1][1] - m[1][0] * m[0][1]));

		return true;
	}
}

//4*4矩阵求逆，默认最后一列是【0，0，0，1】
inline bool InverseMatrix4X4(const Matrix<FloatType, 4, 4> &m, Matrix<FloatType, 4, 4>& result) {
	auto det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
		- m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2])
		+ m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]);
	if (det < EPSILON_E4) {
		return false;
	}
	else {
		auto det_inv = 1 / det;
		result.SetRawDate(0, 0, det_inv * (m[1][1] * m[2][2] - m[2][1] * m[1][2]));
		result.SetRawDate(0, 1, -det_inv * (m[0][1] * m[2][2] - m[2][1] * m[0][2]));
		result.SetRawDate(0, 2, det_inv*(m[0][1] * m[1][2] - m[1][1] * m[0][2]));
		result.SetRawDate(0, 3, 0.0f);

		result.SetRawDate(1, 0, -det_inv * (m[1][0] * m[2][2] - m[2][0] * m[1][2]));
		result.SetRawDate(1, 1, det_inv*(m[0][0] * m[2][2] - m[2][0] * m[0][2]));
		result.SetRawDate(1, 2, -det_inv * (m[0][0] * m[1][2] - m[1][0] * m[0][2]));
		result.SetRawDate(1, 3, 0.0f);

		result.SetRawDate(2, 0, det_inv*(m[1][0] * m[2][1] - m[2][0] * m[1][1]));
		result.SetRawDate(2, 1, -det_inv * (m[0][0] * m[2][1] - m[2][0] * m[0][1]));
		result.SetRawDate(2, 2, det_inv*(m[0][0] * m[1][1] - m[1][0] * m[0][1]));
		result.SetRawDate(2, 3, 0.0f);

		result.SetRawDate(3, 0, -(m[3][0] * m[0][0] + m[3][1] * m[1][0] + m[3][2] * m[2][0]));
		result.SetRawDate(3, 1, -(m[3][0] * m[0][1] + m[3][1] * m[1][1] + m[3][2] * m[2][1]));
		result.SetRawDate(3, 2, -(m[3][0] * m[0][2] + m[3][1] * m[1][2] + m[3][2] * m[2][2]));
		result.SetRawDate(3, 3, 1.0f);
		return true;
	}
}

//3d向量与4*4矩阵相乘,对向量进行平移、旋转等操作
inline Vector3D operator * (Vector3D &v, Matrix<FloatType, 4, 4> &m) {
	Vector3D tmp;
	tmp.x = v.x * m[0][0] + v.y * m[1][0] + v.z* m[2][0] + m[3][0];
	tmp.y = v.x * m[0][1] + v.y * m[1][1] + v.z* m[2][1] + m[3][1];
	tmp.z = v.x * m[0][2] + v.y * m[1][2] + v.z* m[2][2] + m[3][2];
	return tmp;
}
//3d向量与4*4矩阵相乘
inline Vector3D operator * (Matrix<FloatType, 4, 4> &m, Vector3D &v) {
	return v * m;
}

//3d向量与4*3矩阵相乘，默认最后一列是【0，0，0，1】
inline Vector3D operator * (Vector3D &v, Matrix<FloatType, 4, 3> &m) {
	Vector3D tmp;
	tmp.x = v.x * m[0][0] + v.y * m[1][0] + v.z* m[2][0] + m[3][0];
	tmp.y = v.x * m[0][1] + v.y * m[1][1] + v.z* m[2][1] + m[3][1];
	tmp.z = v.x * m[0][2] + v.y * m[1][2] + v.z* m[2][2] + m[3][2];
	return tmp;
}
inline Vector3D operator * (Matrix<FloatType, 4, 3> &m,Vector3D &v) {
	return (v * m);
}

/***************参数化直线******************/

using Point2D = Vector2D;
using Point3D = Vector3D;
//极坐标
class Polar2D {
public:
	Polar2D();
	Polar2D(FloatType r, FloatType theta);
	Polar2D(Polar2D&);
	~Polar2D();
	void ConvertToPoint2D(Point2D&);
	FloatType r;
	FloatType theta;//弧度
};
//柱面坐标
class Cylindrical {
public:
	Cylindrical();
	Cylindrical(FloatType r, FloatType theta, FloatType z);
	Cylindrical(Cylindrical&);
	~Cylindrical();
	void ConvertToPoint3D(Point3D&);
	FloatType r;
	FloatType theta;
	FloatType z;
};
//球坐标
class Spherical3D {
public:
	Spherical3D();
	Spherical3D(FloatType p, FloatType theta, FloatType phi);
	Spherical3D(Spherical3D&);
	~Spherical3D();
	void ConvertToPoint3D(Point3D&);
	FloatType p;
	FloatType theta;
	FloatType phi;
};



//定点数
class Fix {//有错误，先不管
public:	
	static const int MAX = INT_MAX;
	static const int MIN = INT_MIN;
	static const size_t NUM_BIT = sizeof(int)*8;
	static const size_t NUM_BIT_OVER_2 = sizeof(int)*4;
	static const size_t ONE = 1<< NUM_BIT_OVER_2;
	static const size_t TEN = 10<< NUM_BIT_OVER_2;
	static const size_t HALF = 1<< (NUM_BIT_OVER_2-1);
	static const int LOW_MASK = 0X0000FFFF;
	static const int HIGH_MASK = 0XFFFF0000;
	Fix(int v) :val(v){}
	Fix(double v) {
		val =int (v * ONE);
	}
	Fix(const Fix& f):val(f.val){}
	Fix(const float v) {
		val =int (v * ONE);
	}
	Fix operator +(const Fix& y) {
		Fix sum(val + y.val);
		return sum;
	}
	Fix operator *(const Fix& y) {
		auto low = (unsigned int)(val&LOW_MASK);
		auto high = val >> NUM_BIT_OVER_2;
		auto yLow = (unsigned int)(y.val&LOW_MASK);
		auto yHigh = y.val >> NUM_BIT_OVER_2;

		auto low2 = low * yLow;
		auto lowHigh =(int) low * yHigh;
		auto highLow = high *(int) yLow;
		auto high2 = high * yHigh;

		auto lowReault = low2 >> NUM_BIT_OVER_2;
		auto highResult1 = high2 << NUM_BIT_OVER_2;

		auto sum = (int)lowReault + lowHigh + highLow + highResult1;
		Fix res(sum);
		return res;
	}
	operator float() const {
		return (float)val / ONE;
	}
	int GetValue() {
		return val;
	}
private:
	int val;
};

/*
using Point2D = Vector2D;
using Point3D = Vector3D;
class Line2D {
public:
	Point2D p0;
	Point2D p1;
	Vector2D v;
};
class Line3D {
public:
	Point3D p0;
	Point3D p1;
	Vector3D v;
};
class Plane3D {
public:
	Point3D P0;
	Vector3D n;
};*/



class PointInt {
public:
	PointInt(int x, int y);
	PointInt(PointInt& v);
	~PointInt();
	void operator =(const PointInt &v) {
		x = v.x;
		y = v.y;
	}
	inline void PointIntZero(PointInt &v) {
		v.x = 0;
		v.y = 0;
	}
	int x, y;
};