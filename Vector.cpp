#include "Vector.h"


Vector::Vector(size_t n, double value) {
	resize(n);
	for (size_t i = 0; i < n; i++) {
		data[i] = value;
	}
	cap = n;
}
Vector::Vector(const Vector& v) {
	resize(v.sizev);
	for (size_t i = 0; i < sizev; i++) {
		data[i] = v.data[i];
	}
	cap = v.cap;
}
void Vector::resize(size_t newSize) {
	if (cap >= newSize) {
		sizev = newSize;
		return;
	}
	reserve(Mul * newSize);
	sizev = newSize;
}
Vector::Vector(double* start, double* end) {
	if (end >= start) throw "Error start bigger than end";
	sizev = end - start;
	resize(sizev);
	for (size_t i = 0; i < sizev; i++) {
		data[i] = start[i];
	}
	cap = sizev;
}
Vector::~Vector() {	clear();}
void Vector::clear() {
	sizev = 0;
	delete[] data;
	data = nullptr;
}
Vector& Vector::operator= (const Vector& v) {
	if (&v != this) {
		resize(v.sizev);
		for (size_t i = 0; i < sizev; i++) {
			data[i] = v.data[i];
		}
	}
	return* this;
}
void Vector::swap(Vector& v) {
	size_t tmp1 = sizev;
	sizev = v.sizev;
	v.sizev = tmp1;
	double* tmp2 = data;
	data = v.data;
	v.data = tmp2;
}
void Vector::delVector(Vector& v) {
	Vector v2;
	v.swap(v2);
}
size_t Vector::insert(size_t pos, double value) {
	resize(sizev + 1);
	for (size_t i = sizev - 1; i > pos; i--) {
		data[i] = data[i - 1];
	}
	data[pos] = value;
	return pos;
}
void Vector::erase(size_t pos) {
	if (pos > sizev) throw "Error erase pos";
	for (size_t i = pos; i < sizev - 1; i++) {
		data[i] = data[i + 1];
	}
	resize(sizev - 1);
}
void Vector::push_back(double value) {
	resize(sizev + 1);
	data[sizev - 1] = value;
}
void Vector::pop_back() {
	sizev--;
}
void Vector::print() {
	for (size_t i = 0; i < sizev; i++) {
		std::cout << data[i] << " ";
	}
	std::cout << "\n";
}
void Vector::reserve(size_t newCap) {
	if (cap >= newCap) return;
	double* newData = new double[newCap];
	for (size_t i = 0; i < sizev; i++) 
		newData[i] = data[i];
	delete[] data;
	data = newData;
	cap = newCap;
}