#include "..\\include\\vector.h"
#include "..\\include\\matrix.h"
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <ostream>
#include <string>
#include <cmath>

#include <initializer_list>

using namespace std;

static void printErrorMessageAndExit(string message)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
    cout << "ERROR: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << message << "\n";
    exit(EXIT_FAILURE);
}

ostream& operator<<(ostream& out, const VECTOR3& a)
{
    out << fixed << setprecision(2) << "" << a.x << " " << a.y << " " << a.z << "";
    return out;
}

double VECTOR3::operator*(const VECTOR3& a) const
{
    double aux = a.x * x + a.x * y + a.x * z + a.y * x + a.y * y + a.y * z + a.z * x + a.z * y + a.z * z;
    return aux;
}

VECTOR3 VECTOR3::operator*(const MATRIX3& mat) const
{
    VECTOR3 ans;
    ans.x = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z;
    ans.y = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z;
    ans.z = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z;

    return ans;
}

void VECTOR3::normalize()
{
    double len = sqrt(x * x + y * y + z * z);
    x /= len;
    y /= len;
    z /= len;
}

double& VECTOR3:: operator[](int index)
{
    if ( index < 0 || index > 3 )
        printErrorMessageAndExit("TRYING TO ACCESS MEMORY OVER THE BOUNDARY");
    if ( index == 0 )
        return x;
    if ( index == 1 )
        return y;
    return z;
}
