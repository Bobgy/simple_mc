#ifndef VEC_H
#define VEC_H
namespace vec{

	template <class T> inline T sqr(T x){ return x*x; }
	
	template < class T >
	class Vec3{

	private:

		T x[3];

	public:

		Vec3(T v[3]){
			x[0] = v[0];
			x[1] = v[1];
			x[2] = v[2];
		}
		Vec3(T a, T b, T c){
			x[0] = a;
			x[1] = b;
			x[2] = c;
		}
		Vec3(){}
		T *operator&(){
			return x;
		}
		const T *operator&() const {
			return x;
		}
		T &operator[](size_t id){
			return x[id];
		}
		T operator[](size_t id) const {
			return x[id];
		}
		bool operator < (Vec3 r) const {
			if (x[0] != r[0]) return x[0] < r[0];
			if (x[1] != r[1]) return x[1] < r[1];
			return x[2] < r[2];
		}
		Vec3<T> operator + (Vec3 r) const {
			return Vec3(x[0] + r[0], x[1] + r[1], x[2] + r[2]);
		}
		Vec3<T> operator - (Vec3 r) const {
			return Vec3(x[0] - r[0], x[1] - r[1], x[2] - r[2]);
		}
		friend Vec3<T> operator * (Vec3<T> l, T k) {
			return Vec3(l[0] * k, l[1] * k, l[2] * k);
		}
		friend Vec3<T> operator * (T k, Vec3<T> r){
			return Vec3(r[0] * k, r[1] * k, r[2] * k);
		}
		T operator * (Vec3<T> r) const { //dot product
			return x[0] * r[0] + x[1] * r[1] + x[2] * r[2];
		}
		Vec3<T> operator ^ (Vec3<T> r) const { //cross product
			return Vec3<T>(x[1] * r[2] - x[2] * r[1], x[2] * r[0] - x[0] * r[2], x[0] * r[1] - x[1] * r[0]);
		}
		T operator !() const {
			return sqrt(sqr(x[0] + sqr(x[1]) + sqr(x[2])));
		}

	};
}
#endif