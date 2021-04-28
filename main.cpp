#include <bits/stdc++.h>
#include <windows.h>
#define SPACE_DIM 100
#define X_DIM 41
#define Y_DIM 41
#define Z_DIM 12
#define R 8
#define r 4
#define X_BIAS -20
#define Y_BIAS -20
#define Z_BIAS -5

using namespace std;

struct vect{
    double x, y, z;
    friend ostream& operator<<(ostream& out, const vect& a)
    {
        out << fixed << setprecision(2) << "{" << a.x << "," << a.y << ", " << a.z << "}";
        return out;
    }
    double operator*(const vect& a)
    {
        double aux = a.x * x + a.x * y + a.x * z + a.y * x + a.y * y + a.y * z + a.z * x + a.z * y + a.z * z;
        return aux;
    }
}Normals[SPACE_DIM][SPACE_DIM][SPACE_DIM], light, rotNormals[SPACE_DIM][SPACE_DIM][SPACE_DIM];

// ostream& operator<<(ofstream& out, vect a)

char gray_scale[] = ".:*#@";
char space[SPACE_DIM][SPACE_DIM][SPACE_DIM];
char rotMat[SPACE_DIM][SPACE_DIM][SPACE_DIM], displayMat[SPACE_DIM][SPACE_DIM];

char findLum(double val)
{
    if ( val < 0 )
        return gray_scale[0];
    if ( val < 1 )
        return gray_scale[1];
    if ( val < 3 )
        return gray_scale[2];
    if ( val < 5 )
        return gray_scale[3];
    return gray_scale[4];
}

vect NormVect(double x, double y, double z)
{
    double aux = sqrt(x * x + y * y), len = sqrt(x * x + y * y + z * z);

    if ( aux == 0 )
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        cout << "ERROR: ";
        // system("Color 07");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        cout << "DIVIDING BY 0 IN 'NormVect' FUNCTION\n";
        exit(EXIT_FAILURE);
    }

    return { 2.0 * x * (R - aux) / (aux * len), 2.0 * y * (R - aux) / (aux * len), 1 / len };
}

int roundDouble(double x)
{
    if ( x <= 0.5 )
        return ( int )x;
    return ( int )x + 1;
}

void copyMatrix(double A[4][4], double B[4][4])
{
    for ( int i = 1; i <= 3; i++ )
        for ( int j = 1;j <= 3; j++ )
            A[i][j] = B[i][j];
}

void matMult(double A[4][4], double B[4][4])
{
    double C[4][4] = { {0, 0, 0, 0},{0, 0, 0, 0},{0, 0, 0, 0},{0, 0, 0, 0} };

    for ( int i = 1; i <= 3; i++ )
        for ( int j = 1; j <= 3; j++ )
            for ( int k = 1; k <= 3; k++ )
                C[i][j] += A[i][k] * B[k][j];

    copyMatrix(A, C);
}

void M_INV_X(double A[4][4], double radAngle)
{
    double s = sin(radAngle), c = cos(radAngle);
    double B[4][4] = { {0, 0, 0, 0},{0, 1, 0, 0}, {0, 0, c, s}, {0, 0, -s, c} };
    copyMatrix(A, B);
}

void M_INV_Y(double A[4][4], double radAngle)
{
    double s = sin(radAngle), c = cos(radAngle);
    double B[4][4] = { {0, 0, 0, 0},{0, c, 0, -s}, {0, 0, 1, 0}, {0, s, 0, c} };
    copyMatrix(A, B);
}

void M_INV_Z(double A[4][4], double radAngle)
{
    double s = sin(radAngle), c = cos(radAngle);
    double B[4][4] = { {0, 0, 0, 0},{0, c, -s, 0}, {0, s, c, 0}, {0, 0, 0, 1} };
    copyMatrix(A, B);
}

void M_INV_XY(double A[4][4], double radAngle)
{
    double c = cos(radAngle), s = sin(radAngle);
    double B[4][4] = { {0, 0, 0, 0},{0, c, 0, -s},{0, s * s, c, s * c},{0, s * c, -s, c * c} };

    copyMatrix(A, B);
}

void M_INV_XYZ(double A[4][4], double radAngle)
{
    double c = cos(radAngle), s = sin(radAngle);
    double B[4][4] = { {0, 0, 0, 0}, {0, c * c, -(c * s), -s}, {0, c * s + c * s * s, c * c - s * s * s, c * s}, {0, c * c * s - s * s, -(c * s) - c * s * s, c * c} };

    copyMatrix(A, B);
}

inline double formula(double x, double y, double z)
{
    double aux = (7.0 - sqrt(x * x + y * y));
    return 1.0 * aux * aux + z * z;
}

void matVectMult(double v[4], double A[4][4])
{
    double C[4] = { 0, 0, 0, 0 };

    for ( int i = 1; i <= 3; i++ )
        for ( int j = 1; j <= 3; j++ )
            C[i] += 1.0 * v[j] * A[i][j];

    for ( int i = 1; i <= 3; i++ )
        v[i] = C[i];
}

void createTorus()
{
    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            for ( int k = 0; k <= Z_DIM; k++ )
                if ( round(formula(i + X_BIAS, j + Y_BIAS, k + Z_BIAS)) <= (1.0 * r * r) )
                {
                    space[i][j][k] = gray_scale[0];
                    Normals[i][j][k] = NormVect(i + X_BIAS, j + Y_BIAS, k + Z_BIAS);
                }
                else
                    space[i][j][k] = ' ';
}

void FindSurfaceNormal(int x, int y, int z)
{
    int nudge = 0.25;
    int newx, newy;
    int sign = 1;

    if ( formula(x + nudge, y, z) > 1.0 * r * r )
        sign = -1;


}

inline bool inside_space(double v[4])
{
    return !(v[1] < 0 || v[2] < 0 || v[3] < 0 || v[1] > X_DIM || v[2] > Y_DIM || v[3] > Z_DIM);
}

/*
@param axis 1-x, 2-y, 3-z, 4-XY
*/
void rotate(double radAngle, int axis)
{
    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            for ( int k = 0; k <= Z_DIM; k++ )
                rotMat[i][j][k] = ' ';

    double v[4] = { 0, 0, 0, 0 };
    double Rotation[4][4] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} };

    if ( axis == 3 )
        M_INV_X(Rotation, radAngle);
    else if ( axis == 2 )
        M_INV_Y(Rotation, radAngle);
    else if ( axis == 1 )
        M_INV_Z(Rotation, radAngle);
    else if ( axis == 4 )
        M_INV_XY(Rotation, radAngle);
    else if ( axis == 5 )
        M_INV_XYZ(Rotation, radAngle);
    else
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        cout << "ERROR: ";
        // system("Color 07");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        cout << "INVALID ROTATION AXIS";
        exit(EXIT_FAILURE);
    }


    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            for ( int k = 0; k <= Z_DIM; k++ )
            {
                v[1] = i + X_BIAS;
                v[2] = j + Y_BIAS;
                v[3] = k + Z_BIAS;

                matVectMult(v, Rotation);

                v[1] -= X_BIAS;
                v[2] -= Y_BIAS;
                v[3] -= Z_BIAS;

                // cout << i << " " << j << " " << k << "       " << v[1] << " " << v[2] << " " << v[3] << "\n";
                if ( !inside_space(v) )
                    continue;
                rotMat[i][j][k] = space[roundDouble(v[1])][roundDouble(v[2])][roundDouble(v[3])];
                rotNormals[i][j][k] = Normals[roundDouble(v[1])][roundDouble(v[2])][roundDouble(v[3])];

                v[1] = rotNormals[i][j][k].x;
                v[2] = rotNormals[i][j][k].y;
                v[3] = rotNormals[i][j][k].z;

                matVectMult(v, Rotation);
                rotNormals[i][j][k].x = v[1];
                rotNormals[i][j][k].y = v[2];
                rotNormals[i][j][k].z = v[3];
            }
}

void transform_2d()
{
    for ( int i = 0; i <= X_DIM; i++ )
        for ( int j = 0; j <= Y_DIM; j++ )
            displayMat[i][j] = ' ';

    for ( int k = 0; k <= Z_DIM; k++ )
        for ( int i = 0; i <= X_DIM; i++ )
            for ( int j = 0; j <= Y_DIM; j++ )
                if ( displayMat[i][j] == ' ' && rotMat[i][j][k] != ' ' )
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
    light.x = light.y = 3;
    createTorus();

    clearscreen();
    for ( int i = 1; i <= 30; i++ )
        cout << "                                                                         \n";
    clearscreen();

    // rotate(0, 1);
    // transform_2d();
    // for ( int i = 0; i <= X_DIM; i++ )
    // {
    //     for ( int j = 0; j <= Y_DIM; j++ )
    //         // for ( int k = 0; k <= Y_DIM; k++ )
    //         cout << space[i][j][5];
    //     cout << "\n";
    // }
    // cout << endl;
    // cout << endl;
    // double minn = INT_MAX;

    // for ( int i = 0; i <= X_DIM; i++ )
    // {
    //     for ( int j = 0; j <= Y_DIM; j++ )
    //         for ( int k = 0; k <= Y_DIM; k++ )
    //             minn = min(minn, ( double )(Normals[i][j][k] * light));
    // }
    // cout << minn;
    // return 0;

    double angle = 0;

    while ( 1 )
    {
        if ( angle >= 6.28 )
            angle = 0;
        rotate(angle, 1);
        update();
        angle += 0.1;
        // Sleep(1000);
    }
}