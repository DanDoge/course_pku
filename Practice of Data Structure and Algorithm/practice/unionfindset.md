### 并查集

初始化

```c++
int init(int size){
    for(int i = 1; i <= size; i += 1){
        prec[i] = i;
    }
}
```

查找

```c++
int get_prec(int idx){
    return prec[idx] == idx ? idx : prec[idx] = get_prec(prec[idx]);
}
```

合并

```c++
int merge(int from, int to){
    if(get_prec(from) == get_prec(to)){
        return ;
    }
    prec[get_prec(from)] = get_prec(to);
}
```

离线并查集: 寻找不同类的东西 -> 先处理完同类的再找错误

食物链，bugs -> 离线不适用，捕食关系强于不同类
- 维护三个并查集: x x_eat eat_x
- 每次对三个元素归并

维护集合大小?
- 每个节点维护两个变量：和父节点之间的距离(包括父节点，不包括自己，init=0)(e.g. 银河英雄传说) and (作为根节点时)集合大小
