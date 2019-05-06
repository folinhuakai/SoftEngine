#pragma once
#include <initializer_list>
#include "Math.h"
#include "VectorXD.h"
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
	//加法
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
	//减法
	Matrix<T, ROW, COL> operator -(const Matrix<T, ROW, COL> &m)const {
		T tmp[ROW][COL];
		for (int i = 0; i < ROW; ++i) {
			for (int j = 0; j < COL; j++) {
				tmp[i][j] = M[i][j] - m[i][j];
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
template<class T, int row, int col>
inline Matrix<T, row, col> operator *(const Matrix<T, row, col> &m1, float k) {
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
template<class T, int row, int col>
inline Matrix<T, row, col> operator *(float k, const Matrix<T, row, col> &m1) {
	return (m1 * k);
}

//计算行列式
template<int N>
inline float MatrixDet(const Matrix<float, N, N> &m) {

	float ret[N][N];
	m.GetRawDate(ret);
	int maxLine = 0;
	//标记交换行的次数
	int sign = 0;
	for (int col = 0; col < N - 1; ++col) {
		maxLine = col;
		float Max = ret[col][col];

		//判断每列的最大数，并将其放到主对角线上
		for (int row = col + 1; row < N; ++row) {
			float value = ret[row][col];
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
			float val;
			for (int k = col; k < N; k++)
				ret[row][k] = ret[row][k] - ret[col][k] * tmp;
		}
	}
	//求化简后行列式的值
	float tmp = ret[0][0];
	for (int i = 1; i < N; ++i)
		tmp *= ret[i][i];
	if (sign % 2 != 0)
		tmp = -tmp;
	return tmp;
}

//矩阵求逆，采用LU分解方法
template<int N>
inline bool InverseMatrix(const Matrix<float, N, N> &m, Matrix<float, N, N>& result) {
	float ret[N][N];
	m.GetRawDate(ret);
	float u[N][N];
	float l[N][N];
	// 计算L第1列，对角线都为1，不计算，用于存在U矩阵的对角线
	for (int row = 1; row < N; ++row)
		ret[row][0] = ret[row][0] / ret[0][0];
	//LU分解
	for (int row = 1; row < N; ++row) {
		//计算U矩阵
		for (int col = row; col < N; ++col) {
			float sum = 0;
			for (int t = 0; t < row; ++t) {
				sum += ret[row][t] * ret[t][col];
			}

			ret[row][col] = ret[row][col] - sum;

		}
		//计算L矩阵
		for (int rowl = row + 1; rowl < N; ++rowl) {
			float sum = 0;
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
			float sum = 0;
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
			float sum = 0;
			for (int t = col; t < row; ++t) {
				l[row][col] = l[row][col] - ret[row][t] * l[t][col];
			}
		}
	}
	Matrix<float, N, N> U(u);
	Matrix<float, N, N> L(l);
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
inline bool InverseMatrix2X2(const Matrix<float, 2, 2> &m, Matrix<float, 2, 2>& result) {
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
inline bool InverseMatrix3X3(const Matrix<float, 3, 3> &m, Matrix<float, 3, 3>& result) {
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
inline bool InverseMatrix4X4(const Matrix<float, 4, 4> &m, Matrix<float, 4, 4>& result) {
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
inline Vector3D operator * (Vector3D &v, Matrix<float, 4, 4> &m) {
	Vector3D tmp;
	tmp.x = v.x * m[0][0] + v.y * m[1][0] + v.z* m[2][0] + m[3][0];
	tmp.y = v.x * m[0][1] + v.y * m[1][1] + v.z* m[2][1] + m[3][1];
	tmp.z = v.x * m[0][2] + v.y * m[1][2] + v.z* m[2][2] + m[3][2];
	return tmp;
}
//3d向量与4*4矩阵相乘
inline Vector3D operator * (Matrix<float, 4, 4> &m, Vector3D &v) {
	return v * m;
}

//3d向量与4*3矩阵相乘，默认最后一列是【0，0，0，1】
inline Vector3D operator * (Vector3D &v, Matrix<float, 4, 3> &m) {
	Vector3D tmp;
	tmp.x = v.x * m[0][0] + v.y * m[1][0] + v.z* m[2][0] + m[3][0];
	tmp.y = v.x * m[0][1] + v.y * m[1][1] + v.z* m[2][1] + m[3][1];
	tmp.z = v.x * m[0][2] + v.y * m[1][2] + v.z* m[2][2] + m[3][2];
	return tmp;
}
inline Vector3D operator * (Matrix<float, 4, 3> &m, Vector3D &v) {
	return (v * m);
}

//交换矩阵对应列
template<class T>
inline void Swap(Matrix<T, 2, 2> &m, Vector2D &v, int col) {
	m[0][col] = v.x;
	m[1][col] = v.y;
}

template<class T>
inline void Swap(Matrix<T, 3, 3> &m, Vector3D &v, int col) {
	m[0][col] = v.x;
	m[1][col] = v.y;
	m[2][col] = v.z;
}

template<class T>
inline void Swap(Matrix<T, 4, 3> &m, Vector4D &v, int col) {
	m[0][col] = v.x;
	m[1][col] = v.y;
	m[2][col] = v.z;
	m[2][col] = v.w;
}

template<class T>
inline void Swap(Matrix<T, 4, 4> &m, Vector4D &v, int col) {
	m[0][col] = v.x;
	m[1][col] = v.y;
	m[2][col] = v.z;
	m[2][col] = v.w;
}

template<class T, int ROW,int COL>
bool operator ==(const Matrix<T, ROW, COL> &a,const Matrix<T, ROW, COL> &b) {
	bool flag = true;
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			if (fabs(a[i][j] - b[i][j]) > EPSILON_E5) {
				flag = false;
				break;
			}
		}
	}
	return flag;
}
//方程求解函数
inline int SolveSystem2_2(const Matrix<float,2,2> &a, Matrix<float, 2, 1> &x,const Matrix<float, 2, 1> &b) {
	Matrix<float, 2, 2> ret;
	if (InverseMatrix2X2(a, ret)) {
		//存在逆矩阵
		x = ret * b;
		return 1;
	}
	else {
		return 0;
	}
}

inline int SolveSystem3_3(const Matrix<float,3,3> &a, Matrix<float, 3, 1> &x,const Matrix<float, 3, 1> &b) {
	Matrix<float, 3, 3> ret;
	if (InverseMatrix3X3(a, ret)) {
		//存在逆矩阵
		x = ret * b;
		return 1;
	}
	else {
		return 0;
	}
}