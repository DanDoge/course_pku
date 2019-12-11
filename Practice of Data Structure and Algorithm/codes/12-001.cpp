#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
using namespace std;

struct Point{
    double x, y;
    Point(double xx=0.0, double yy=0.0){
        x = xx;
        y = yy;
    }
};

typedef Point Vector;

Vector operator +(Vector a, Vector b){
    return Vector(a.x + b.x, a.y + b.y);
}

Vector operator -(Vector a, Vector b){
    return Vector(a.x - b.x, a.y - b.y);
}

Vector operator *(Vector a, double b){
    return Vector(a.x * b, a.y * b);
}

Vector operator /(Vector a, double b){
    return Vector(a.x / b, a.y / b);
}

int cmp_zero(double x){
    return (x > 1e-9) - (x < -1e-9);
}

double cross_product(Vector a, Vector b){
    return a.x * b.y - a.y * b.x;
}

bool intersect_segment(Point a1, Point a2, Point b1, Point b2){
    double cross_product1 = cross_product(b1 - b2, a1 - b2);
    double cross_product2 = cross_product(b1 - b2, a2 - b2);
    return cmp_zero(cross_product1) * cmp_zero(cross_product2) <= 0;
}

bool intersect(Point a1, Point a2, Point b1, Point b2, Point c1, Point c2){
    double ac1 = cross_product(c1 - c2, a1 - c2);
    double ac2 = cross_product(c1 - c2, a2 - c2);
    double bc1 = cross_product(c1 - c2, b1 - c2);
    double bc2 = cross_product(c1 - c2, b2 - c2);

    if(cmp_zero(ac1) * cmp_zero(ac2) < 0 || cmp_zero(bc1) * cmp_zero(bc2) < 0){
        return 1;
    }
    if(cmp_zero(ac1) <= 0 && cmp_zero(ac2) <= 0 && cmp_zero(bc1) <= 0 && cmp_zero(bc2) <= 0){
        return 1;
    }
    if(cmp_zero(ac1) >= 0 && cmp_zero(ac2) >= 0 && cmp_zero(bc1) >= 0 && cmp_zero(bc2) >= 0){
        return 1;
    }
    return 0;
}

void intersect_point(Point a1, Point a2, Point b1, Point b2, Point &res){
    double cross_product1 = cross_product(a1 - b1, b2 - b1);
    double cross_product2 = cross_product(a2 - b1, b2 - b1);
    res = (a2 * cross_product1 - a1 * cross_product2) / (cross_product1 - cross_product2);
}


double res = -1e9;
int n;

Point p[32][2];

void solve(Point a, Point b){
    int i;
    for(i = 1; i < n; i += 1){
        if(intersect(p[i][0], p[i + 1][0], p[i][1], p[i + 1][1], a, b)){
            break;
        }
    }
    if(i == n){
        res = 1e9;
        return ;
    }
    if(p[i][0].x < a.x){
        return ;
    }
    Point tmp;
    if(intersect_segment(p[i][0], p[i + 1][0], a, b)){
        intersect_point(p[i][0], p[i + 1][0], a, b, tmp);
        res = max(res, tmp.x);
    }
    if(intersect_segment(p[i][1], p[i + 1][1], a, b)){
        intersect_point(p[i][1], p[i + 1][1], a, b, tmp);
        res = max(res, tmp.x);
    }
}

int main(int argc, char const *argv[]) {
    while(cin >> n && n > 0){
        for(int i = 1; i <= n; i += 1){
            cin >> p[i][0].x >> p[i][0].y;
            p[i][1].x = p[i][0].x;
            p[i][1].y = p[i][0].y - 1;
        }
        res = -1e9;
        for(int i = 1; i <= n; i += 1){
            for(int j = i + 1; j <= n; j += 1){
                solve(p[i][0], p[j][1]);
                solve(p[i][1], p[j][0]);
            }
        }
        if(cmp_zero(res - 1e9) == 0){
            cout << "Through all the pipe." << endl;
        }else{
            cout << fixed << setprecision(2) << res << endl;
        }
    }
    return 0;
}
