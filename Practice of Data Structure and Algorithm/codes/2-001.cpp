#include <iostream>
#include <vector>
using namespace std;

int size;
int step = 1;

void dfs(int pos, vector<vector<int> >& tree, vector<int>& start, vector<int>& end){
    start[pos] = step++;
    int num_child = tree[pos].size();
    for(int i = 0; i < num_child; i += 1){
        dfs(tree[pos][i], tree, start, end);
    }
    end[pos] = step++;
    return ;
}

int lowbit(int n){
    return n & (-n);
}

void update(int pos, int value, vector<int>& c){
    while(pos <= size * 2){
        c[pos] += value;
        pos += lowbit(pos);
    }
    return ;
}

int query(int pos, vector<int>& c){
    int sum = 0;
    while(pos > 0){
        sum += c[pos];
        pos -= lowbit(pos);
    }
    return sum;
}

int main(int argc, char const *argv[]) {
    std::ios::sync_with_stdio(false);
    cin >> size;
    vector<vector<int> > tree(size + 1);
    vector<int> start(size + 1, 0);
    vector<int> end(size + 1, 0);
    vector<int> a(size + 1, 1);
    vector<int> c(2 * size + 1, 0);
    for(int i = 1; i < size; i += 1){
        int parent, child;
        cin >> parent >> child;
        tree[parent].push_back(child);
    }
    dfs(1, tree, start, end);
    for(int i = 1; i <= 2 * size; i += 1){
        update(i, 1, c);
    }
    int num_instruct;
    cin >> num_instruct;
    while(num_instruct--){
        char type;
        int pos;
        cin >> type >> pos;
        if(type == 'C'){
            if(a[pos] == 1){
                update(start[pos], -1, c);
                update(end[pos], -1, c);
                a[pos] = 0;
            }else{
                update(start[pos], 1, c);
                update(end[pos], 1, c);
                a[pos] = 1;
            }
        }else{
            cout << (query(end[pos], c) - query(start[pos] - 1, c)) / 2 << endl;
        }
    }
    return 0;
}
