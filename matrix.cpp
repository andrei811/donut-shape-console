#include "matrix.h"
#include "vector.h"
#include <windows.h>
#include <initializer_list>
#include <ostream>
#include <string>
#include <iostream>
using namespace std;

static void printErrorMessageAndExit(string message)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
    cout << "ERROR: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << message << "\n";
    exit(EXIT_FAILURE);
}

LMATRIX::LMATRIX(double* l)
{
    for ( int i = 0; i < 3; i++ )
        line[i] = (l + i);
}

double& LMATRIX::operator[](int index)
{
    if ( index > 2 || index < 0 )
        printErrorMessageAndExit("TRYING TO ACCESS MEMORY OVER THE BOUNDARY");
    return *(line[index]);
}

MATRIX3::MATRIX3()
{
    for ( int i = 0; i < 3; i++ )
        for ( int j = 0; j < 3; j++ )
            matrix[i][j] = 0;
}

MATRIX3::MATRIX3(initializer_list<initializer_list<double>> A)
{
    if ( A.size() != 3 )
        printErrorMessageAndExit("BAD MATRIX INITALIZATION");
    int i = 0, j = 0;

    for ( auto it = A.begin(); it != A.end();it++ )
    {
        if ( it->size() != 3 )
            printErrorMessageAndExit("BAD MATRIX INITALIZATION");

        j = 0;
        for ( auto iter = it->begin();iter != it->end(); iter++ )
            matrix[i][j] = *iter, j++;
        i++;
    }
}

MATRIX3::MATRIX3(const MATRIX3& mat)
{
    for ( int i = 0; i < 3; i++ )
        for ( int j = 0; j < 3; j++ )
            matrix[i][j] = mat.matrix[i][j];
}

LMATRIX MATRIX3:: operator[](int index)const
{
    if ( index > 2 || index < 0 )
        printErrorMessageAndExit("TRYING TO ACCESS MEMORY OVER THE BOUNDARY");
    return LMATRIX(( double* )matrix[index]);
}

VECTOR3 MATRIX3:: operator*(const VECTOR3& A) const
{
    return A * (*(this));
}
MATRIX3 MATRIX3:: operator*(const MATRIX3& A)const
{
    MATRIX3 C;

    for ( int i = 0; i < 3; i++ )
        for ( int j = 0; j < 3; j++ )
            for ( int k = 0; k < 3; k++ )
                C[i][j] += matrix[i][k] * A.matrix[k][j];

    return C;
}

