#ifndef VEC_HPP
#define VEC_HPP

#include <type_traits>
#include "stdtypes.hpp"

namespace ltd {

template <typename T, size_t N>
class vec {
public:
	
	// some arguments are either (const T &) or (T) depending on what T is
	typedef typename std::conditional<
		std::is_arithmetic<T>::value, T, const T &>::type ConstRefT;
	
	// constructor
	vec() = default;
	template <typename... Args> vec(Args... args);
	
	// assignment
	vec<T, N> & operator =  (vec<T, N> const &rhs);
	
	// access
	T         & operator [] (size_t i);
	ConstRefT   operator [] (size_t i) const;
	
	// comparison
	bool        operator == (vec<T, N> const &rhs) const;
	bool        operator != (vec<T, N> const &rhs) const;
	
	// negating
	vec<T, N>   operator +  () const;
	vec<T, N>   operator -  () const;
	
	// addition
	vec<T, N> & operator += (vec<T, N> const &rhs);
	vec<T, N> & operator -= (vec<T, N> const &rhs);
	vec<T, N>   operator +  (vec<T, N> const &rhs) const;
	vec<T, N>   operator -  (vec<T, N> const &rhs) const;
	
	// multiplication
	vec<T, N> & operator *= (ConstRefT);
	vec<T, N> & operator /= (ConstRefT);
	vec<T, N>   operator *  (ConstRefT) const;
	vec<T, N>   operator /  (ConstRefT) const;
	
	// scalar product
	T           operator *  (vec<T, N> const &rhs) const;
	
	// query
	T norm2() const;
	
private:
	T _t[N];
};

template <typename T, size_t N>
template <typename... Args>
vec<T, N>::vec(Args... args)
		: _t({args...})
{
	static_assert(sizeof...(Args) == N,
			"Incorrect number of arguments passed to vec constructor");
}

template <typename T, size_t N>
vec<T, N> & vec<T, N>::operator = (vec<T, N> const &rhs)
{
	for (size_t i = 0; i < N; ++i) {
		this->_t[i] = rhs._t[i];
	}
	return *this;
}

template <typename T, size_t N>
T & vec<T, N>::operator [] (size_t i)
{
	return _t[i];
}

template <typename T, size_t N>
typename vec<T, N>::ConstRefT vec<T, N>::operator [] (size_t i) const
{
	return _t[i];
}

template <typename T, size_t N>
bool vec<T, N>::operator == (vec<T, N> const &rhs) const
{
	for (size_t i = 0; i < N; ++i)
		if (this->_t[i] != rhs._t[i]) return false;
	return true;
}

template <typename T, size_t N>
bool vec<T, N>::operator != (vec<T, N> const &rhs) const
{
	return !this->operator==(rhs);
}

template <typename T, size_t N>
vec<T, N> vec<T, N>::operator + () const
{
	return *this;
}

template <typename T, size_t N>
vec<T, N> vec<T, N>::operator - () const
{
	vec<T, N> result;
	for (size_t i = 0; i < N; ++i)
		result._t[i] = -this->_t[i];
	return result;
}

template <typename T, size_t N>
vec<T, N> & vec<T, N>::operator += (vec<T, N> const &rhs)
{
	for (size_t i = 0; i < N; ++i)
		this->_t[i] += rhs._t[i];
	return *this;
}

template <typename T, size_t N>
vec<T, N> & vec<T, N>::operator -= (vec<T, N> const &rhs)
{
	for (size_t i = 0; i < N; ++i)
		this->_t[i] -= rhs._t[i];
	return *this;
}

template <typename T, size_t N>
vec<T, N> vec<T, N>::operator + (vec<T, N> const &rhs) const
{
	vec<T, N> result;
	for (size_t i = 0; i < N; ++i)
		result._t[i] = this->_t[i] + rhs._t[i];
	return result;
}

template <typename T, size_t N>
vec<T, N> vec<T, N>::operator - (vec<T, N> const &rhs) const
{
	vec<T, N> result;
	for (size_t i = 0; i < N; ++i)
		result._t[i] = this->_t[i] - rhs._t[i];
	return result;
}

template <typename T, size_t N>
vec<T, N> & vec<T, N>::operator *= (ConstRefT rhs)
{
	for (size_t i = 0; i < N; ++i)
		this->_t[i] *= rhs;
	return *this;
}

template <typename T, size_t N>
vec<T, N> & vec<T, N>::operator /= (ConstRefT rhs)
{
	for (size_t i = 0; i < N; ++i)
		this->_t[i] /= rhs;
	return *this;
}

template <typename T, size_t N>
vec<T, N> vec<T, N>::operator * (ConstRefT rhs) const
{
	vec<T, N> result;
	for (size_t i = 0; i < N; ++i)
		result._t[i] = this->_t[i] * rhs;
	return result;
}

template <typename T, size_t N>
vec<T, N> vec<T, N>::operator / (ConstRefT rhs) const
{
	vec<T, N> result;
	for (size_t i = 0; i < N; ++i)
		result._t[i] = this->_t[i] / rhs;
	return result;
}

template <typename T, size_t N>
T vec<T, N>::operator * (vec<T, N> const &rhs) const
{
	T result = 0;
	
	for (size_t i = 0; i < N; ++i)
		result += this->_t[i] * rhs._t[i];
	
	return result;
}

template <typename T, size_t N>
T vec<T, N>::norm2() const
{
	return this->operator*(*this);
}

extern template class vec<uint8, 2>;
extern template class vec<uint8, 3>;
extern template class vec<int8, 2>;
extern template class vec<int8, 3>;
extern template class vec<int, 2>;
extern template class vec<int, 3>;
extern template class vec<size_t, 2>;
extern template class vec<size_t, 3>;
extern template class vec<float, 2>;
extern template class vec<float, 3>;
extern template class vec<double, 2>;
extern template class vec<double, 3>;

} // namespace ltd

#endif // VEC_HPP
