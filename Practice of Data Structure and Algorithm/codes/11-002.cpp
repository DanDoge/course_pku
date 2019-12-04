#include <iostream>
#include <cmath>
using namespace std;

struct point{
    double x, y;
    point(double xx = 0.0, double yy = 0.0){
        x = xx;
        y = yy;
    }
};

point points[16];
char name[16][4];

double get_area(point a, point b, point c){
    return 0.5 * fabs((c.y - a.y) * (b.x - a.x) - (b.y - a.y) * (c.x - a.x));
}

int main(int argc, char const *argv[]) {
    int num_points = 0;
    while(cin >> num_points && num_points){
        for(int i = 0; i < num_points; i += 1){
            cin >> name[i] >> points[i].x >> points[i].y;
        }
        int max_idx[3] = {};
        double max_area = 0.0;
        for(int i = 0; i < num_points; i += 1){
            for(int j = i + 1; j < num_points; j += 1){
                for(int k = j + 1; k < num_points; k += 1){
                    double this_area = get_area(points[i], points[j], points[k]);
                    if(this_area < max_area){
                        continue;
                    }
                    int all_out = 1;
                    for(int l = 0; l < num_points; l += 1){
                        if(l == i || l == j || l == k){
                            continue;
                        }
                        double area1 = get_area(points[l], points[j], points[k]);
                        double area2 = get_area(points[i], points[l], points[k]);
                        double area3 = get_area(points[i], points[j], points[l]);
                        if(area1 + area2 + area3 <= this_area){
                            all_out = 0;
                            break;
                        }
                    }
                    if(all_out){
                        max_idx[0] = i;
                        max_idx[1] = j;
                        max_idx[2] = k;
                        max_area = this_area;
                    }
                }
            }
        }
        cout << name[max_idx[0]] << name[max_idx[1]] << name[max_idx[2]] << endl;
    }
    return 0;
}
