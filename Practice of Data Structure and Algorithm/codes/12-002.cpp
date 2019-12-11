#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

struct Point{
    double x, y;
    Point(double xx=0.0, double yy=0.0){
        x = xx;
        y = yy;
    }
};

Point p[1024];
int stack[1024], top;

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

double norm(Vector a){
    return sqrt(a.x * a.x + a.y * a.y);
}

bool cmp(Point a, Point b){
    double tmp = cross_product(a - p[0], b - p[0]);
    if(tmp > 0){
        return true;
    }else if(tmp == 0 && norm(a - p[0]) < norm(b - p[0])){
        return true;
    }
    return false;
}

void init(int n){
    Point tmp;
    cin >> p[0].x >> p[0].y;
    tmp.x = p[0].x;
    tmp.y = p[0].y;
    int smallest_idx = 0;
    for(int i = 1; i < n; i += 1){
        cin >> p[i].x >> p[i].y;
        if(p[i].y < tmp.y || (p[i].y == tmp.y && p[i].x < tmp.x)){
            tmp.x = p[i].x;
            tmp.y = p[i].y;
            smallest_idx = i;
        }
    }
    p[smallest_idx].x = p[0].x;
    p[smallest_idx].y = p[0].y;
    p[0].x = tmp.x;
    p[0].y = tmp.y;

    sort(p + 1, p + n, cmp);
}

void graham(int n){
    for(int i = 0; i <= 1; i += 1){
        stack[i] = i;
    }
    top = 1;
    for(int i = 2; i < n; i += 1){
        while(top > 0 && cross_product(p[stack[top]] - p[stack[top - 1]], p[i] - p[stack[top - 1]]) < 0){
            top -= 1;
        }
        top += 1;
        stack[top] = i;
    }
}

int main(int argc, char const *argv[]) {
    int num_cases;
    cin >> num_cases;
    while(num_cases--){
        int n;
        cin >> n;
        init(n);
        if(n < 6){
            cout << "NO" << endl;
            continue;
        }
        graham(n);
        int flag = true;
        for(int i = 1; i < top; i += 1){
            if(cross_product(p[stack[i + 1]] - p[stack[i - 1]], p[stack[i]] - p[stack[i - 1]])
                && cross_product(p[stack[i + 2]] - p[stack[i]], p[stack[i + 1]] - p[stack[i]])
            ){
                flag = false;
            }
        }
        cout << (flag? "YES" : "NO") << endl;
    }
    return 0;
}
