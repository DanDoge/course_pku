#include <iostream>
#include <cmath>
#include <memory.h>
#include <iomanip>
using namespace std;

int n, m;
double eps = 1e-10;
double INF = 1e7;

struct Point{
    double x, y;
    Point(double xx = 0.0, double yy = 0.0){
        x = xx;
        y = yy;
    }
}points[128], p[128], tmp[128];

struct Line{
    Point s, e;
}line[128];

typedef Point Vector;

Vector operator+(Vector a, Vector b){
    return Vector(a.x + b.x, a.y + b.y);
}

Vector operator-(Vector a, Vector b){
    return Vector(a.x - b.x, a.y - b.y);
}

Vector operator*(Vector a, double b){
    return Vector(a.x * b, a.y * b);
}

Vector operator/(Vector a, double b){
    return Vector(a.x / b, a.y / b);
}

Vector rotate(Vector a){
    return Vector(-a.y, a.x);
}

double len(Vector v){
    return sqrt(v.x * v.x + v.y * v.y);
}

int cmp_double(double x){
    return (x > eps) - (x < eps);
}

double cross(Vector a, Vector b){
    return a.x * b.y - a.y * b.x;
}

Point glt(Point a, Point b, Point c, Point d){
    Vector v = b - a;
    Vector u = d - c;
    Vector w = a - c;
    double t = cross(u, w) / cross(v, u);
    return a + v * t;
}

void cut(Vector a, Vector b){
    int cnt = 0;
    memset(tmp, 0, sizeof(tmp));
    for(int i = 0; i < m; i += 1){
        double aa = cross(b - a, p[i] - a);
        double bb = cross(b - a, p[(i + 1) % m] - a);
        if(cmp_double(aa) <= 0){
            tmp[cnt++] = p[i];
        }
        if(cmp_double(aa) * cmp_double(bb) < 0){
            tmp[cnt++] = glt(a, b, p[i], p[(i + 1) % m]);
        }
    }
    m = cnt;
    for(int i = 0; i < cnt; i += 1){
        p[i] = tmp[i];
    }
}

int has_point(double mid){
    for(int i = 2; i <= n + 1; i += 1){
        Vector l = points[i] - points[i - 1];
        Vector normal = rotate(l) / len(l);
        Vector move = normal * mid;
        line[i - 1].s = l + move + points[i - 1];
        line[i - 1].e = line[i - 1].s - l;
    }
    m = 0;
    p[m++] = Point(INF, INF);
    p[m++] = Point(-INF, INF);
    p[m++] = Point(-INF, -INF);
    p[m++] = Point(INF, -INF);

    for(int i = 1; i <= n; i += 1){
        cut(line[i].s, line[i].e);
        if(!m){
            return 1;
        }
    }
    if(m == 1){
        return 1;
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    while(cin >> n && n){
        for(int i = 1; i <= n; i += 1){
            cin >> points[i].x >> points[i].y;
        }
        points[n + 1] = points[1];
        double left = 0.0, right = 10000.0, res = -1.0;
        while(right - left > eps){
            double mid = (left + right) / 2;
            //cout << mid << endl;
            if(has_point(mid)){
                res = mid;
                right = mid - eps;
            }else{
                left = mid + eps;
            }
        }
        cout << fixed << setprecision(6) << res << endl;
    }
    return 0;
}
