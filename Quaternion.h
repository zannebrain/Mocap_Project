#ifndef ___QUATERNION___
#define ___QUATERNION___

#pragma once

#include <cmath>
#include <ostream>
#include "MVector.h"

#define PI (4.0*atan(1.0))

template <typename T>
struct Quaternion{

	T x, y, z, w;

	Quaternion( T theta, vec3d& axis){
		
		T phi = theta * ( (T)PI / (T)180.0);

		vec3d ax = axis.normalize();

		x = ax[0] * sin( phi / (T)2.0);
		y = ax[1] * sin( phi / (T)2.0);
		z = ax[2] * sin( phi / (T)2.0);
		w =			cos( phi / (T)2.0);
	}

	Quaternion( const vec3d& vec): w(0.0){
		x = vec[0];
		y = vec[1];
		z = vec[2];
	}

	Quaternion( T w, T x, T y, T z){
	
		this->w = w;
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Quaternion(): x(0.0), y(0.0), z(0.0), w(0.0) {}

	T length() const;
	Quaternion<T> normalize() const;
	Quaternion<T> conjugate();

	Quaternion<T> operator-( );
};

template <typename T>
T Quaternion<T>::length() const
{  
	return sqrt(x * x + y * y + z * z + w * w); 
} 

template <typename T>
Quaternion<T> Quaternion<T>::normalize() const
{   
	T L = length();   

	Quaternion<T> q = *this;
	q.x /= L;   
	q.y /= L;   
	q.z /= L;   
	q.w /= L;
	return q;
} 

template <typename T>
Quaternion<T> Quaternion<T>::conjugate()
{
	Quaternion<T> q = *this;
	q.x = -x;   
	q.y = -y;   
	q.z = -z;
	return q;
}

template <typename T>
Quaternion<T> mult(const Quaternion<T>& A, const Quaternion<T>& B) {
	Quaternion<T> C;  
	C.x = A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y;   
	C.y = A.w*B.y - A.x*B.z + A.y*B.w + A.z*B.x;   
	C.z = A.w*B.z + A.x*B.y - A.y*B.x + A.z*B.w;   
	C.w = A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z;   
	return C;
}

template <typename T>
Quaternion<T> operator*(const Quaternion<T>& A, const Quaternion<T>& B) {
	Quaternion<T> C;  
	C.x = A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y;   
	C.y = A.w*B.y - A.x*B.z + A.y*B.w + A.z*B.x;   
	C.z = A.w*B.z + A.x*B.y - A.y*B.x + A.z*B.w;   
	C.w = A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z;   
	return C;
}

template <typename T>
Quaternion<T> operator*( T s, const Quaternion<T>& A) {
	
	return Quaternion<T>(s*A.w, s*A.x, s*A.y, s*A.z );
}

template <typename T>
Quaternion<T> operator+(const Quaternion<T>& A, const Quaternion<T>& B) {

	return Quaternion<T>( A.w + B.w,  A.x + B.x, A.y + B.y, A.z + B.z);
}

template <typename T>
Quaternion<T> Quaternion<T>::operator-( ) {

	return Quaternion<T>( -w, -x, -y, -z );
}

template <typename T>
void rotate( Quaternion<T>& Q, vec3d& P)
{
	Quaternion<T> V( P );

	V = mult(mult(Q, V), Q.conjugate());

	P[0] = V.x;
	P[1] = V.y;
	P[2] = V.z;
}

template <typename T>
void rotate(T theta, vec3d& axis, vec3d& P){

	Quaternion<T> V(P);
	Quaternion<T> R(theta, axis);

	V = R*V*R.conjugate();

	P[0] = V.x;
	P[1] = V.y;
	P[2] = V.z;
}

template <typename T>
Quaternion<T> slerp( Quaternion<T> q1, Quaternion<T> q2, T t){

	Quaternion<T> q3;
	q1 = q1.normalize();
	q2 = q2.normalize();
	T cosOmega = q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;

	//if negative dot, negate one of the input
	//quaternions to take the shorter 4D "arc"

	if(cosOmega < (T)0.0){
	
		cosOmega = -cosOmega;
		q3 = -q2;
	}else{
	
		q3 = q2;
	}

	T k0, k1;
	if( fabs(cosOmega) < ((T)1.0) - ((T)5e-3)){
	
		T sinOmega = sqrt( ((T)1.0) - (cosOmega*cosOmega) );
		T omega = atan2(sinOmega, cosOmega);

		k0 = sin( ((T)1.0) - t )*(omega / sinOmega);
		k1 = sin(t*omega) / sinOmega;
		
		return ((k0*q1) + (k1*q3)).normalize();
	}else{
	
		Quaternion<T> q4 = ( ((T)1.0) - t)*q1 +t*q3;
		return q4.normalize();
	}
}

template< typename T >
vec3f toEuler( const Quaternion<T>& q ){
	
	T p(0.), h(0.), b(0.);
	p = -2.0*( q.y*q.z - q.w*q.x);

	if( fabs(p) > 5e-4){

		p = 1.570796f*p;
		h = atan2( (T)(-(q.x*q.z) + (q.w*q.y) ), (T)(.5 - (q.y*q.y)- (q.z*q.z)) );
		b = 0.0f;
	}else{
		
		p = asin(p);
		h = atan2( (T)( (q.x*q.z) + (q.w*q.y) ), (T)(.5 - (q.x*q.x) - (q.y*q.y) ) );
		b = atan2( (T)( (q.x*q.y) + (q.w*q.z) ), (T)(.5 - (q.x*q.x) - (q.z*q.z) ) );
	}
		
	return vec3f(p, h, b);
}

template< typename T >
ostream& operator<<(ostream& stream, const Quaternion< T>& q)
{
	stream << "[ " << q.x << " " << q.y << " " << q.z << " | " << q.w << " ]";

	return stream;
}

#endif