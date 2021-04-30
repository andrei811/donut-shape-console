#include <iostream>
#include <cmath>
#include <windows.h>
#include "..\\include\\vector.h"
#include "..\\include\\matrix.h"
#define SPACE_DIM 50
#define X_DIM 41
#define Y_DIM 41
#define Z_DIM 41
#define R 10
#define r 4
#define X_BIAS -20
#define Y_BIAS -20
#define Z_BIAS -20

using namespace std;

void printErrorMessageAndExit(string message)
{
    // set color red
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
    cout << "ERROR: ";
    // set color white
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << message << "\n";
    exit(EXIT_FAILURE);
}

VECTOR3 Normals[SPACE_DIM][SPACE_DIM][SPACE_DIM], rotNormals[SPACE_DIM][SPACE_DIM][SPACE_DIM], light;

char grayScale[] = ".,-~:;=!*#$@";
bool space[SPACE_DIM][SPACE_DIM][SPACE_DIM], rotMat[SPACE_DIM][SPACE_DIM][SPACE_DIM];
char displayMat[SPACE_DIM][SPACE_DIM];

/*
@brief find the luminance on the surface of the torus
@param val dot product of the light vector and normal vector
*/
char findLum(double val)
{
    if ( val < -300 )
        return grayScale[0];
    if ( val < -250 )
        return grayScale[1];
    if ( val < -200 )
        return grayScale[2];
    if ( val < -150 )
        return grayScale[3];
    if ( val < -100 )
        return grayScale[4];
    if ( val < -50 )
        return grayScale[5];
    if ( val < 0 )
        return grayScale[6];
    if ( val < 50 )
        return grayScale[7];
    if ( val < 100 )
        return grayScale[8];
    if ( val < 150 )
        return grayScale[9];
    if ( val < 200 )
        return grayScale[10];
    return grayScale[11];
}

/*
@brief Finding the normal surface vector
@param x x-coordonate of point on the surface whose normal vector will be found
@param y y-coordonate of point on the surface whose normal vector will be found
@param z z-coordonate of point on the surface whose normal vector will be found
*/
VECTOR3 FindSurfaceNormal(double x, double y, double z)
{
    double aux = sqrt(x * x + y * y);
    if ( aux == 0 )
        printErrorMessageAndExit("DIVIDING BY 0 IN 'FindSurfaceNormal' FUNCTION");

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

/*
@breif torus formula; result <= r^2 if the point is inside torus
*/
inline double formula(double x, double y, double z)
{
    double aux = (R - sqrt(x * x + y * y));
    return 1.0 * aux * aux + z * z;
}

/*
@brief Create torus in `space`
*/
void createTorus()
{
    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            for ( int k = 0; k <= Z_DIM; k++ )
                if ( roundDouble(formula(i + X_BIAS, j + Y_BIAS, k + Z_BIAS)) <= (1.0 * r * r) )
                {
                    space[i][j][k] = 1;
                    Normals[i][j][k] = FindSurfaceNormal(i + X_BIAS, j + Y_BIAS, k + Z_BIAS);
                }
                else
                {
                    space[i][j][k] = 0;
                    Normals[i][j][k] = { -100, -100, -100 };
                }
}

/*
@brief return true if the point v is inside the usable space
*/
inline bool insideSpace(VECTOR3 v)
{
    return !(v[0] < 0 || v[1] < 0 || v[2] < 0 || v[0] > X_DIM || v[1] > Y_DIM || v[2] > Z_DIM);
}

/*
@brief Rotate the space on a specific axis
*/
void rotate(double radAngle, int axis)
{
    double s = sin(radAngle), c = cos(radAngle);
    MATRIX3 InvRot, NormRot;

    if ( axis == 3 ) // x rotation
    {
        InvRot = MATRIX3({ {1, 0, 0}, { 0, c, s}, { 0, -s, c} });
        NormRot = MATRIX3({ { 1, 0, 0}, { 0, c, -s}, { 0, s, c} });
    }
    else if ( axis == 2 ) // y rotation
    {
        InvRot = MATRIX3({ { c, 0, -s}, { 0, 1, 0}, { s, 0, c} });
        NormRot = MATRIX3({ { c, 0, s}, { 0, 1, 0}, { -s, 0, c} });
    }
    else if ( axis == 1 ) // z rotation
    {
        InvRot = MATRIX3({ { c, s, 0}, { -s, c, 0}, { 0, 0, 1} });
        NormRot = MATRIX3({ { c, -s, 0}, { s, c, 0}, { 0, 0, 1} });
    }
    else if ( axis == 4 ) // xyz rotation
    {
        InvRot = MATRIX3({ { c * c, c * s, -s}, { c * s * s - c * s, c * c + s * s * s, c * s}, { c * c * s + s * s, s * s * c - s * c, c * c} });
        NormRot = MATRIX3({ { c * c, -s * c, s}, {c * s * s + c * s, c * c - s * s * s, -c * s}, { s * s - c * c * s, c * s + c * s * s , c * c} });
    }
    else
        printErrorMessageAndExit("INVALID ROTATION AXIS");

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

                // rotate the normal vector by multiplying by the rotation matrix
                rotNormals[i][j][k] = NormRot * rotNormals[i][j][k];

                rotNormals[i][j][k].normalize();
            }
}

/*
@brief Transform the 3D space into a screen-printable 2D space
*/
void transform_2d()
{
    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            displayMat[i][j] = ' ';

    for ( int k = Z_DIM; k >= 0; k-- )
        for ( int i = 0; i <= X_DIM; i++ )
            for ( int j = 0; j <= Y_DIM; j++ )
                if ( displayMat[i][j] == ' ' && rotMat[i][j][k] )
                    displayMat[i][j] = findLum(( double )(rotNormals[i][j][k] * light));
}

/*
@brief Clear the screen by moving the cursor to (0,0)
*/
void clearscreen()
{
    HANDLE hOut;
    COORD Position;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    Position.X = 0;
    Position.Y = 0;
    SetConsoleCursorPosition(hOut, Position);
}

/*
@brief Prints the 2D space in cosole
*/
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

/*
@brief Transforms the upper case into lower cases
*/
void lower(char* s)
{
    for ( int i = 0; s[i]; i++ )
        if ( s[i] >= 'A' && s[i] <= 'Z' )
            s[i] = 'a' + (s[i] - 'A');
}

int main(int argc, char** argv)
{
    int ROTATION = 4;

    for ( int i = 0; i < argc; i++ )
    {
        lower(argv[i]);
        if ( i + 1 < argc && strcmp("-a", argv[i]) == 0 )
        {
            lower(argv[i + 1]);
            if ( strcmp("x", argv[i + 1]) == 0 )
                ROTATION = 1;
            else if ( strcmp("y", argv[i + 1]) == 0 )
                ROTATION = 2;
            else if ( strcmp("z", argv[i + 1]) == 0 )
                ROTATION = 3;
            else if ( strcmp("xyz", argv[i + 1]) == 0 )
                ROTATION = 4;
        }
    }

    light = { 0, 100, 100 };

    clearscreen();

    for ( int i = 1; i <= 50; i++ )
        cout << "                                                                                               \n";

    clearscreen();
    createTorus();

    double angle = 0;

    while ( 1 )
    {
        if ( angle >= 6.28 )
            angle = 0;
        rotate(angle, ROTATION);
        update();
        angle += 0.1;
    }
}