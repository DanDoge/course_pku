#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

int main(){
    for(int i = 3; i <= 12; i += 1){
        cout << 1200 * log(i) / log(2) << endl;
    }
    return 0;
}
