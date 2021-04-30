#pragma once 

class VECTOR3;
#include <initializer_list>
#include <ostream>
#include <string>
#include "..\\include\\vector.h"

using namespace std;

struct LMATRIX{
    double* line[3];
    LMATRIX(double*);
    double& operator[](int);
};

class MATRIX3{
    friend class VECTOR3;
private:
    double matrix[3][3];
public:

    MATRIX3();
    MATRIX3(initializer_list<initializer_list<double>>);
    MATRIX3(const MATRIX3&);

    LMATRIX operator[](int) const;
    VECTOR3 operator*(const VECTOR3&) const;
    MATRIX3 operator*(const MATRIX3&) const;
};
