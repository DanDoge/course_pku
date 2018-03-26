#include <stdio.h>
#include <stdlib.h>

#include <cuda_runtime.h>

#define SIZE 1024
#define bool int
#define false 0
#define true  1

int matrix[SIZE][SIZE] = {};
int result[SIZE][SIZE]    = {};

void random_generator(int** mat, int size){
    for(int i = 0; i < size; i += 1){
        for(int j = 0; j < size; j += 1){
            mat[i][j] = rand() % 1024;
        }
    }
}

bool verification(int** mat, int** res, int size){
    for(int i = 0; i < size; i += 1){
        for(int j = 0; j < size; j += 1){
            if(mat[i][j] != res[j][i]){
                return false;
            }
        }
    }
    return true;
}

bool InitCUDA(){
    int count = 0;
    cudaGetDeviceCount(&count);

    if(count == 0){
        fprintf(stderr, "no device found!\n");
        return false;
    }

    for(int i = 0; i < count; i += 1){
        cudaDeviceProp prop;
        if(cudaGetDeviceProperties(&prop, i) == cudaSuccess){
            if(prop.major >= 1){
                break;
            }
        }
    }

    if(i == count){
        fprintf(stderr, "no device supporting CUDA!\n");
        return false;
    }

    cudaSetDevice(i);

    return true;
}

__global__ static void trans(int** mat, int** res, int size){
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int idx = bid * THREAD_NUM + tid;

    const int tx = idx / size;
    const int ty = idx % size;

    res[ty * size + tx] = mat[tx * size + ty];
}

int main(){
    if(!InitCUDA()){
        return 0;
    }
    random_generator(matrix, SIZE);

    int *gpumatrix, *gpuresult;
    cudaMalloc((void**)&gpumatrix, sizeof(int) * SIZE * SIZE);
    cudaMalloc((void**)*gpuresult, sizeof(int) * SIZE * SIZE);

    cudaMemcpy(gpumatrix, matrix, sizeof(int) * SIZE * SIZE, cudaMemcpyHostToDevice);
    cudaMemcpy(gpuresult, result, sizeof(int) * SIZE * SIZE, cudaMemcpyHostToDevice);

    trans<<<SIZE, SIZE, 0>>>(gpumatrix, gpuresult, SIZE);

    cudaMemcpy(result, gpuresult, sizeof(int) * SIZE * SIZE, cudaMemcpyDeviceToHost);

    cudaFree(gpumatrix);
    cudaFree(gpuresult);

    if(!verification(matrix, result, SIZE)){
        fprintf(stderr, "wrong!\n");
    }
    return 0;
}
