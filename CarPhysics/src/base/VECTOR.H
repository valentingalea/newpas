/*
 * Copyright (c) 2012 Valentin Galea
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cstdio>
#include <cmath>

template<typename T> static inline T RAD(T x) { return x * T(0.017453292519943296); }
template<typename T> static inline T DEG(T x) { return x * T(57.2957795130823209); }

#define dot %
#define cross *

//
// Vector Type Template
//
template<class real>
class vector_t_t
{
public:
	//
	// De/Construction
	//
	inline vector_t_t()
	{
	}

	inline vector_t_t(real x, real y, real z)
	{
		data[0] = x;
		data[1] = y;
		data[2] = z;
	}

	inline vector_t_t(const char *string)
	{
		sscanf(string,
			(sizeof(real) == sizeof(float)) ? "%f %f %f" : "%lf %lf %lf",
			&data[0], &data[1], &data[2]);
	}

#if 0
private:
	// avoid pass by value
	inline vector_t_t(const vector_t_t &other)
	{
		*this = other; //call operator= (compiler's default is safe)
		//printf("copy-constr self=%p(%s) other=%p\n", this, print(), &other);
	}

public:
#endif
	//
	// Accesors & Mutators
	//
	inline const real operator[](int offset) const //read-only
	{
		return data[offset];
	}

	inline real& operator[](int offset) //full-access
	{
		return data[offset];
	}

	inline real const * getData(void) const //read-only
	{
		return data;
	}

	//
	// Unary Operators
	//
#define FRIEND
#define TYPE const vector_t_t
#define PARAM void
#define CODE(op) return vector_t_t(op##data[0], op##data[1], op##data[2]);

#define MAKE_OP(op) \
	FRIEND inline TYPE operator op ( PARAM ) \
	{ \
		CODE(op) \
	}

	MAKE_OP(-)
	MAKE_OP(+)

 #undef TYPE
#define TYPE const vector_t_t&
 #undef PARAM
#define PARAM const real factor
 #undef CODE
#define CODE(op) \
		data[0] op factor; data[1] op factor; data[2] op factor; \
		return *this;

	MAKE_OP(=)
	MAKE_OP(*=)
	MAKE_OP(/=)
	MAKE_OP(+=)
	MAKE_OP(-=)

 #undef PARAM
#define PARAM const vector_t_t &right
 #undef CODE
#define CODE(op) \
		data[0] op right[0]; data[1] op right[1]; data[2] op right[2]; \
		return *this;

	//MAKE_OP(=) the compiler's default is better !
	//MAKE_OP(*=) should be cross-prod !
	//MAKE_OP(/=) is it really usefull ?
	MAKE_OP(+=)
	MAKE_OP(-=)

	//
	// Binary Operators
	//
 #undef FRIEND
#define FRIEND friend
 #undef TYPE
#define TYPE const vector_t_t
 #undef PARAM
#define PARAM const real factor, const vector_t_t &v
 #undef CODE
#define CODE(op) return vector_t_t(v.data[0] op factor, v.data[1] op factor, v.data[2] op factor);

	MAKE_OP(+)
	MAKE_OP(*)

 #undef PARAM
#define PARAM const vector_t_t &v, const real factor

	MAKE_OP(+)
	MAKE_OP(*)
	MAKE_OP(-)
	MAKE_OP(/)

 #undef PARAM
#define PARAM const vector_t_t &left, const vector_t_t &right
 #undef CODE
#define CODE(op) return vector_t_t(left.data[0] op right.data[0], left.data[1] op right.data[1], left.data[2] op right.data[2]);

	MAKE_OP(+)
	MAKE_OP(-)
	//MAKE_OP(*) see below...
	//MAKE_OP(/)

	//									[   i	 j    k  ]
	//	cross/vector-product = det (	[ a[0] a[1] a[2] ]	)
	//									[ b[0] b[1] b[2] ]
 #undef CODE
#define CODE(op) \
		return vector_t_t( \
			left.data[1]*right.data[2] - left.data[2]*right.data[1], \
			left.data[2]*right.data[0] - left.data[0]*right.data[2], \
			left.data[0]*right.data[1] - left.data[1]*right.data[0]  );
	MAKE_OP( cross )

	//
	//	dot/scalar-product = || a || * || b || * cos( a, b )
	//
 #undef TYPE
#define TYPE real
 #undef CODE
#define CODE(op) \
	 return (left.data[0] * right.data[0] + left.data[1] * right.data[1] + left.data[2] * right.data[2]);
	 MAKE_OP( dot )

#undef FRIEND
#undef TYPE
#undef PARAM
#undef CODE
#undef MAKE_OP

	//
	// Methods
	//
	inline const real length() const
	{
		return (real)sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
	}

	inline void normalize()
	{
		real i = real(1) / length();

		data[0] *= i;
		data[1] *= i;
		data[2] *= i;
	}

	void rotateAround(const vector_t_t &w, real angle)
	{
		real _cos, _sin, tmp;
		vector_t_t f1, f2, f3;

		_cos = real(cos(angle));
		_sin = real(sin(angle));
#if 1
		tmp = *this dot w;
		f1[0] = w[0] * tmp;
		f1[1] = w[1] * tmp;
		f1[2] = w[2] * tmp;
		f2[0] = f1[0] + (data[0] - f1[0]) * _cos;
		f2[1] = f1[1] + (data[1] - f1[1]) * _cos;
		f2[2] = f1[2] + (data[2] - f1[2]) * _cos;
		f3 = *this cross w;
		data[0] = f2[0] + f3[0] * _sin;
		data[1] = f2[1] + f3[1] * _sin;
		data[2] = f2[2] + f3[2] * _sin;
#else
		f1 = w * (*this dot w);
		f2 = f1 + (*this - f1) * _cos;
		f3 = *this cross w;
		*this = f2 + (f3 * _sin);
#endif
	}

	//
	// Actual Data
	//
private:
	real	data[3];
};

//
// Quaternion Type Template
//
template<class real>
class quaternion_t_t
{
public:
	//
	// De/Construction
	//
	inline quaternion_t_t()
	{
	}

	inline quaternion_t_t(real a, real b, real c, real d)
	{
		w = a, x = b, y = c, z = d;
	}

	// axis 2 rot. quat
	inline quaternion_t_t(real angle, const vector_t_t<real>& axis)
	{
		//axis.normalize();
		real a = angle / real(2);
		real c = real(cos(a));
		real s = real(sin(a));
		w = c;
		x = axis[0]*s;
		y = axis[1]*s;
		z = axis[2]*s;
	}

#if 0
private:
	// avoid pass by value
	inline quaternion_t_t(const quaternion_t_t& other)
	{
		*this = other; //call operator= (below)
		//printf("quat cp construct at %p\n", this);
	}
#endif

public:
	/*// compiler's default is safe
	inline quaternion_t_t& operator= (const quaternion_t_t& other)
	{
		w = other.w, etc...
		return *this;
	}
	*/

	//
	// Unary Operators
	//
	inline const quaternion_t_t operator- (void)
	{
		return quaternion_t_t(w, -x, -y, -z); //or just negate 'w'
	}

	//
	// Binary Operators
	//
	friend inline const quaternion_t_t operator+ (const quaternion_t_t& a, const quaternion_t_t& b)
	{
		return quaternion_t_t(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);
	}

	friend inline const quaternion_t_t operator* (const quaternion_t_t& a, const quaternion_t_t& b)
	{
		return quaternion_t_t(
			a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
			a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
			a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
			a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
		);
	}

	friend inline const quaternion_t_t operator* (real r, const quaternion_t_t& a)
	{
		return quaternion_t_t(a.w * r, a.x * r, a.y * r, a.z * r);
	}

	//
	// Methods
	//
	void toRotation(const quaternion_t_t& other) // ordinary quat 2 rot. quat
	{
		real a = real(acos(other.w) * 2);
		real s = real(sin(a / 2));
		if ( s == real(0) )
			x = other.x, y = other.y, z = other.z;
		else
			x = other.x / s, y = other.y / s, z = other.z / s;
		w = DEG<real>(a);
	}

	void normalize()
	{
		real r = real(sqrt(w*w + x*x + y*y + z*z));
		if ( r == real(0) )
			w = 1, x = 0, y = 0, z = 0;
		else
		{
			real n = real(1) / r;
			w *= n, x *= n, y *= n, z *= n;
		}
	}

	//
	// Actual Data
	//
public:
	real w, x, y, z;
};

//
// Matrix Type Template
//
template<class real>
class matrix_t_t
{
public:

	//
	// De/Construction
	//
	inline matrix_t_t()
	{
	}

	inline matrix_t_t
	(
		real rc00, real rc01, real rc02,
		real rc10, real rc11, real rc12,
		real rc20, real rc21, real rc22
	)
	{
		m[0][0] = rc00; m[0][1] = rc01; m[0][2] = rc02;
		m[1][0] = rc10; m[1][1] = rc11; m[1][2] = rc12;
		m[2][0] = rc20; m[2][1] = rc21; m[2][2] = rc22;
	}

	matrix_t_t(const quaternion_t_t<real>& q)
	{
		real s = q.w, vx = q.x, vy = q.y, vz = q.z;

		m[0][0] = 1-2*vy*vy-2*vz*vz;
		m[0][1] = 2*vx*vy-2*s*vz;
		m[0][2] = 2*vx*vz+2*s*vy;

		m[1][0] = 2*vx*vy+2*s*vz;
		m[1][1] = 1-2*vx*vx-2*vz*vz;
		m[1][2] = 2*vy*vz-2*s*vx;

		m[2][0] = 2*vx*vz-2*s*vy;
		m[2][1] = 2*vy*vz+2*s*vx;
		m[2][2] = 1-2*vx*vx-2*vy*vy;
	}

private:
	// avoid pass by value
	inline matrix_t_t(const matrix_t_t &other)
	{
		*this = other; //call operator= (below)
		//printf("cp construct at %p\n", this);
	}

public:
	// compilers's default is ok
	//inline matrix_t_t& operator =(const matrix_t_t &other);

	//
	// Binary Operators
	//
	friend const matrix_t_t operator* (const matrix_t_t& left, const matrix_t_t& right)
	{
		const data_t &a = left.m;
		const data_t &b = right.m;
		#define M(r, c) a[r][0] * b[0][c] + a[r][1] * b[1][c] + a[r][2] * b[2][c]
		return matrix_t(
			M(0, 0), M(0, 1), M(0, 2),
			M(1, 0), M(1, 1), M(1, 2),
			M(2, 0), M(2, 1), M(2, 2)
		);
		#undef M
	}

	friend const vector_t_t<real> operator* (const vector_t_t<real>& left, const matrix_t_t& right)
	{
		return vector_t_t<real>(
			right.m[0][0] * left[0] + right.m[0][1] * left[1] + right.m[0][2] * left[2],
			right.m[1][0] * left[0] + right.m[1][1] * left[1] + right.m[1][2] * left[2],
			right.m[2][0] * left[0] + right.m[2][1] * left[1] + right.m[2][2] * left[2]
		);
	}

	//
	// Methods
	//
	const matrix_t_t transpose()
	{
		return matrix_t_t(
			m[0][0], m[1][0], m[2][0],
			m[0][1], m[1][1], m[2][1],
			m[0][2], m[1][2], m[2][2]
		);
	}

	const matrix_t_t inverse()
	{
		real det =
			 m[0][0] * (m[1][1]*m[2][2] - m[1][2]*m[2][1]) +
			 m[0][1] * (m[1][0]*m[2][2] - m[1][2]*m[2][0]) +
			 m[0][2] * (m[1][0]*m[2][1] - m[1][1]*m[2][0]) ;

		if ( det == real(0) )
			return matrix_t_t(
				1, 0, 0,
				0, 1, 0,
				0, 0, 1
			);
		else
		{
			real n = real(1) / det;
			return matrix_t_t(
				n * (m[1][1]*m[2][2] - m[1][2]*m[2][1]) * real(+1),
				n * (m[1][0]*m[2][2] - m[1][2]*m[2][0]) * real(-1),
				n * (m[1][0]*m[2][1] - m[1][1]*m[2][0]) * real(+1),

				n * (m[0][1]*m[2][2] - m[0][2]*m[2][1]) * real(-1),
				n * (m[0][0]*m[2][2] - m[0][2]*m[2][0]) * real(+1),
				n * (m[0][0]*m[2][1] - m[0][1]*m[2][0]) * real(-1),

				n * (m[0][1]*m[1][2] - m[0][2]*m[1][1]) * real(+1),
				n * (m[0][0]*m[1][2] - m[0][2]*m[1][0]) * real(-1),
				n * (m[0][0]*m[1][1] - m[0][1]*m[1][0]) * real(+1)
			);
		}
	}

	//
	// Actual Data
	//
private:
	typedef real data_t[3][3];
	data_t m;
};

typedef double real_t;
typedef vector_t_t<real_t> vector_t;
typedef quaternion_t_t<real_t> quaternion_t;

#endif //__VECTOR_H__
