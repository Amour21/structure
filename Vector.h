#pragma once
#include <iostream>
const size_t Mul = 2;

class Vector {
	double* data = nullptr;
	size_t sizev = 0;
	size_t cap = 0;
public:
	Vector() {};
	Vector(size_t n, double value = 0);
	Vector(const Vector& v);
	Vector(double* start, double* end);
	~Vector();
	Vector& operator= (const Vector& v);
	double& operator[] (size_t index) {
		return data[index];
	}
	double& operator[] (size_t index) const {
		return data[index];
	}
	double& at (size_t index) {
		if (index >= 0 && index < sizev)
			return data[index];
		throw "Error: incorrect index";
	}
	double& at(size_t index) const {
		if (index >= 0 && index < sizev)
			return data[index];
		throw "Error: incorrect index";
	}
	bool empty() const {
		return sizev == 0;
	}
	void resize(size_t newSize);
	size_t insert(size_t pos, double value);
	void push_back(double value);
	void pop_back();
	void erase(size_t pos);
	void swap(Vector& other);
	void clear();
	void print();
	void delVector(Vector& v);
	void reserve(size_t newCap);
};