# 第三次作业

###### 元培学院 黄道吉 1600017857

### 前期准备

#### 概述PSRS算法

Parallel Sorting by Regular Sampling(以下简称PSRS)算法, 首先将输入数据平均分配给各个核, 在各个核中排序, 收集各自局部的枢轴值(pivot), 再取出枢轴值的枢轴值(全局的pivot), 按全局pivot将各个核的数组分类, 每一个核处理两个枢轴值之间的数字(这里的实现取[pivot[i - 1], pivot[i]), 即左闭右开的区间划分), 最后收集数组.

具体的算法参考, [这一篇论文](https://pdfs.semanticscholar.org/f796/9a5351dea0374e985190356505ba4f4f26b1.pdf), 代码实现上参考了[这里的实现](http://csweb.cs.wfu.edu/bigiron/LittleFE-PSRS/build/html/PSRSimplementation.html)

#### MPI用法

关于MPI函数的用法和参数, 参考了[这里的文档](https://www.mpich.org/static/docs/v3.1/www3/).

### 代码实现

#### 计时部分&随机数

完全按照助教的示例代码, 计时的时间段是从MPI_Init()到MPI_Finalize().
