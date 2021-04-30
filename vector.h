#pragma once

class MATRIX3;

#include "matrix.h"
#include <initializer_list>
#include <string>
#include <ostream>

using namespace std;

class VECTOR3{
private:
    double x, y, z;

public:
    VECTOR3(double x, double y, double z): x(x), y(y), z(z){}
    VECTOR3(): x(0), y(0), z(0){}
    VECTOR3(const VECTOR3& _aux): x(_aux.x), y(_aux.y), z(_aux.z){}

    friend ostream& operator<<(ostream& out, const VECTOR3& a);

    double operator*(const VECTOR3& a)const;
    VECTOR3 operator*(const MATRIX3& mat)const;
    void normalize();
    double& operator[](int index);
};
