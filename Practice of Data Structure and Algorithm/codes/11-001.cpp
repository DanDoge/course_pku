#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

double eps = 1e-5;

struct point{
    double x, y;
    point(double xx = 0.0, double yy = 0.0){
        x = xx;
        y = yy;
    }
};

typedef point Vector;

Vector operator -(point a, point b){
    return Vector(a.x - b.x, a.y - b.y);
}

Vector operator +(point a, point b){
    return Vector(a.x + b.x, a.y + b.y);
}

Vector operator *(Vector a, double b){
    return Vector(a.x * b, a.y * b);
}

double intersect(Vector a, Vector b){
    return a.x * b.y - a.y * b.x;
}

double length(Vector a){
    return sqrt(a.x * a.x + a.y * a.y);
}

point get_intersection(point a, Vector aa, point b, Vector bb){
    Vector tmp = a - b;
    double tmpp = intersect(bb, tmp) / intersect(aa, bb);
    return a + aa * tmpp;
}

double distance(point p, point a, point b){
    Vector v1 = b - a, v2 = p - a;
    return fabs(intersect(v1, v2)) / length(v1);
}

point a, b, c, d;

int main(int argc, char const *argv[]) {
    cout << "INTERSECTING LINES OUTPUT" << endl;
    int num_cases;
    cin >> num_cases;
    while(num_cases--){
        cin >> a.x >> a.y >> b.x >> b.y;
        cin >> c.x >> c.y >> d.x >> d.y;
        Vector v1 = b - a;
        Vector v2 = d - c;
        if(intersect(v1, v2)){
            point tmp = get_intersection(a, v1, c, v2);
            cout << "POINT " << fixed << setprecision(2) << tmp.x << " "
                 << fixed << setprecision(2) << tmp.y << endl;
        }else if(distance(c, a, b) > eps){
            cout << "NONE" << endl;
        }else{
            cout << "LINE" << endl;
        }
    }
    cout << "END OF OUTPUT" << endl;
    return 0;
}
