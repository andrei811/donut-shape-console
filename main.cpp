#include <bits/stdc++.h>
#include <windows.h>
#include "vector.h"
#include "matrix.h"
#define SPACE_DIM 100
#define X_DIM 41
#define Y_DIM 41
#define Z_DIM 41
#define R 10
#define r 4
#define X_BIAS -20
#define Y_BIAS -20
#define Z_BIAS -20
#define ROTATION 4

using namespace std;

void printErrorMessageAndExit(string message)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
    cout << "ERROR: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << message << "\n";
    exit(EXIT_FAILURE);
}

VECTOR3 Normals[SPACE_DIM][SPACE_DIM][SPACE_DIM], rotNormals[SPACE_DIM][SPACE_DIM][SPACE_DIM], light;

char gray_scale[] = ".,-~:;=!*#$@";
bool space[SPACE_DIM][SPACE_DIM][SPACE_DIM], rotMat[SPACE_DIM][SPACE_DIM][SPACE_DIM];
char displayMat[SPACE_DIM][SPACE_DIM];

char findLum(double val)
{
    if ( val < -300 )
        return gray_scale[0];
    if ( val < -250 )
        return gray_scale[1];
    if ( val < -200 )
        return gray_scale[2];
    if ( val < -150 )
        return gray_scale[3];
    if ( val < -100 )
        return gray_scale[4];
    if ( val < -50 )
        return gray_scale[5];
    if ( val < 0 )
        return gray_scale[6];
    if ( val < 50 )
        return gray_scale[7];
    if ( val < 100 )
        return gray_scale[8];
    if ( val < 150 )
        return gray_scale[9];
    if ( val < 200 )
        return gray_scale[10];
    return gray_scale[11];
}

VECTOR3 NormVECTOR3(double x, double y, double z)
{
    double aux = sqrt(x * x + y * y);

    if ( aux == 0 )
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        cout << "ERROR: ";
        // system("Color 07");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        cout << "DIVIDING BY 0 IN 'NormVECTOR3' FUNCTION\n";
        exit(EXIT_FAILURE);
    }

    VECTOR3 vax = { 2.0 * x * (R - aux) / (aux), 2.0 * y * (R - aux) / (aux), 1 };

    vax.normalize();

    return vax;
}

int roundDouble(double x)
{
    if ( (x - ( int )(x)) <= 0.5 )
        return ( int )x;
    return ( int )x + 1;
}

inline double formula(double x, double y, double z)
{
    double aux = (R - sqrt(x * x + y * y));
    return 1.0 * aux * aux + z * z;
}

void createTorus()
{
    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            for ( int k = 0; k <= Z_DIM; k++ )
                if ( roundDouble(formula(i + X_BIAS, j + Y_BIAS, k + Z_BIAS)) <= (1.0 * r * r) )
                {
                    space[i][j][k] = 1;
                    Normals[i][j][k] = NormVECTOR3(i + X_BIAS, j + Y_BIAS, k + Z_BIAS);
                }
                else
                {
                    space[i][j][k] = 0;
                    Normals[i][j][k] = { -100, -100, -100 };
                }
}

inline bool insideSpace(VECTOR3 v)
{
    return !(v[0] < 0 || v[1] < 0 || v[2] < 0 || v[0] > X_DIM || v[1] > Y_DIM || v[2] > Z_DIM);
}

/*
@param axis 1-x, 2-y, 3-z, 4-XYZ
*/
void rotate(double radAngle, int axis)
{
    double s = sin(radAngle), c = cos(radAngle);
    MATRIX3 InvRot, NormRot;

    if ( axis == 3 ) // rotatie pe x
    {
        InvRot = MATRIX3({ {1, 0, 0}, { 0, c, s}, { 0, -s, c} });
        NormRot = MATRIX3({ { 1, 0, 0}, { 0, c, -s}, { 0, s, c} });
    }
    else if ( axis == 2 ) // rotatie pe y
    {
        InvRot = MATRIX3({ { c, 0, -s}, { 0, 1, 0}, { s, 0, c} });
        NormRot = MATRIX3({ { c, 0, s}, { 0, 1, 0}, { -s, 0, c} });
    }
    else if ( axis == 1 ) // rotatie pe z
    {
        InvRot = MATRIX3({ { c, s, 0}, { -s, c, 0}, { 0, 0, 1} });
        NormRot = MATRIX3({ { c, -s, 0}, { s, c, 0}, { 0, 0, 1} });
    }
    else if ( axis == 4 ) // rotatie pe xyz
    {
        InvRot = MATRIX3({ { c * c, c * s, -s}, { c * s * s - c * s, c * c + s * s * s, c * s}, { c * c * s + s * s, s * s * c - s * c, c * c} });
        NormRot = MATRIX3({ { c * c, -s * c, s}, {c * s * s + c * s, c * c - s * s * s, -c * s}, { s * s - c * c * s, c * s + c * s * s , c * c} });
    }
    else
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        cout << "ERROR: ";
        // system("Color 07");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        cout << "INVALID ROTATION AXIS";
        exit(EXIT_FAILURE);
    }

    VECTOR3 v;

    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            for ( int k = 0; k <= Z_DIM; k++ )
            {
                // create the vector
                v = VECTOR3(i + X_BIAS, j + Y_BIAS, k + Z_BIAS);

                // find the initial position in space for the v vector
                v = v * InvRot;

                // subtract the biases
                v[0] = v[0] - X_BIAS;
                v[1] = v[1] - Y_BIAS;
                v[2] = v[2] - Z_BIAS;

                // init rotMat and rotNormals
                rotMat[i][j][k] = 0;
                rotNormals[i][j][k] = { -100,-100,-100 };

                // if the vector is inside the working space
                if ( !insideSpace(v) )
                    continue;

                rotMat[i][j][k] = space[roundDouble(v[0])][roundDouble(v[1])][roundDouble(v[2])];

                // find the initial normal for the point on the surface...
                rotNormals[i][j][k] = Normals[roundDouble(v[0])][roundDouble(v[1])][roundDouble(v[2])];

                rotNormals[i][j][k] = NormRot * rotNormals[i][j][k]; // rotate the normal vector by multiplying by the rotation matrix

                rotNormals[i][j][k].normalize();
            }
}

void transform_2d()
{
    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            displayMat[i][j] = ' ';

    for ( int k = Z_DIM; k >= 0; k-- )
        for ( int i = 0; i <= X_DIM; i++ )
            for ( int j = 0; j <= Y_DIM; j++ )
                if ( displayMat[i][j] == ' ' && rotMat[i][j][k] )
                {
                    // displayMat[i][j] = rotMat[i][j][k];
                    displayMat[i][j] = findLum(( double )(rotNormals[i][j][k] * light));
                }
}

void clearscreen()
{
    HANDLE hOut;
    COORD Position;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    Position.X = 0;
    Position.Y = 0;
    SetConsoleCursorPosition(hOut, Position);
}

void update()
{
    clearscreen();
    transform_2d();

    // top border
    cout << "||";
    for ( int j = 0; j <= 2 * Y_DIM + 2; j++ )
        cout << "-";
    cout << "||\n";

    for ( int i = 0; i <= X_DIM; i++ )
    {
        cout << "|| ";
        for ( int j = 0; j <= Y_DIM; j++ )
            cout << displayMat[i][j] << displayMat[i][j];
        cout << "|| ";
        cout << '\n';
    }

    // bottom border
    cout << "||";
    for ( int j = 0; j <= 2 * Y_DIM + 2; j++ )
        cout << "-";
    cout << "||";
}

int main()
{
    // cout << VECTOR3(1, 2, 3) * MATRIX3({ {1, 1, 2}, {1, 2, 2}, {0, 2, 3} });
    // return 0;
    light = { 0, 100, 100 };

    clearscreen();
    for ( int i = 1; i <= 30; i++ )
        cout << "                                                                         \n";
    clearscreen();
    createTorus();

    // rotate(0, 1);
    // transform_2d();
    // for ( int i = 0; i <= X_DIM; i++ )
    // {
    //     // cout << "{";
    //     for ( int j = 0; j <= Y_DIM; j++ )
    //         // for ( int k = 0; k <= Y_DIM; k++ )
    //         cout << rotMat[i][j][20] << rotMat[i][j][20];
    //     // cout << Normals[i][j][5] << endl;
    //     cout << endl;
    // }
    // cout << endl;
    // return 0;

    double angle = 0;

    while ( 1 )
    {
        if ( angle >= 6.28 )
            angle = 0;
        rotate(angle, ROTATION);
        update();
        angle += 0.1;
        // Sleep(1000);
    }
}