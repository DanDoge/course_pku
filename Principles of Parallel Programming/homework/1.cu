#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <cuda_runtime.h>

#define bool int
#define false 0
#define true  1

void random_generator(int* mat, int size) {
	for (int i = 0; i < size; i += 1) {
		for (int j = 0; j < size; j += 1) {
			mat[i * size + j] = rand() % 1024;
		}
	}
}

bool verification(int* mat, int* res, int size) {
	for (int i = 0; i < size; i += 1) {
		for (int j = 0; j < size; j += 1) {
			if (mat[i * size + j] != res[j * size + i]) {
				return false;
			}
		}
	}
	return true;
}

void printDeviceProp(const cudaDeviceProp &prop) {
	printf("Device Name : %s.\n", prop.name);
	printf("totalGlobalMem : %d.\n", prop.totalGlobalMem);
	printf("sharedMemPerBlock : %d.\n", prop.sharedMemPerBlock);
	printf("regsPerBlock : %d.\n", prop.regsPerBlock);
	printf("warpSize : %d.\n", prop.warpSize);
	printf("memPitch : %d.\n", prop.memPitch);
	printf("maxThreadsPerBlock : %d.\n", prop.maxThreadsPerBlock);
	printf("maxThreadsDim[0 - 2] : %d %d %d.\n"
           , prop.maxThreadsDim[0]
           , prop.maxThreadsDim[1]
           , prop.maxThreadsDim[2]);
	printf("maxGridSize[0 - 2] : %d %d %d.\n"
           , prop.maxGridSize[0]
           , prop.maxGridSize[1]
           , prop.maxGridSize[2]);
	printf("totalConstMem : %d.\n", prop.totalConstMem);
	printf("major.minor : %d.%d.\n", prop.major, prop.minor);
	printf("clockRate : %d.\n", prop.clockRate);
	printf("textureAlignment : %d.\n", prop.textureAlignment);
	printf("deviceOverlap : %d.\n", prop.deviceOverlap);
	printf("multiProcessorCount : %d.\n", prop.multiProcessorCount);
}

bool InitCUDA()
{
	int count;
	cudaGetDeviceCount(&count);

	if (count == 0) {
		fprintf(stderr, "There is no device.\n");
		return false;
	}

	int i;
	for (i = 0; i < count; i++) {
		cudaDeviceProp prop;
		cudaGetDeviceProperties(&prop, i);
		printDeviceProp(prop);
		if (cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
			if (prop.major >= 1) {
				break;
			}
		}
	}

	if (i == count) {
		fprintf(stderr, "There is no device supporting CUDA 1.x.\n");
		return false;
	}
	cudaSetDevice(i);
	return true;
}

__global__ static void trans(int* mat, int* res, int size, clock_t* time) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	int j = blockIdx.y * blockDim.y + threadIdx.y;

	clock_t start_time;
	if (threadIdx.x == 0 && threadIdx.y == 0) {
		time[blockIdx.x * (size / blockDim.x) + blockIdx.y] = clock();
	}

	if(i < size && j < size){
		res[i * size + j] = mat[j * size + i];
	}

	if (threadIdx.x == 0 && threadIdx.y == 0) {
		time[blockIdx.x * (size / blockDim.x)
             + blockIdx.y
             + size * size / (blockDim.x * blockDim.y)]
        = clock();
	}
}

int main() {
	if (!InitCUDA()) {
		return 0;
	}

	int SIZE = 0;
	scanf("%d", &SIZE);

	int* matrix = (int*)malloc(sizeof(int) * SIZE * SIZE);
	int* result = (int*)malloc(sizeof(int) * SIZE * SIZE);

	random_generator(matrix, SIZE);

	int *gpumatrix, *gpuresult;


	for (int blocksize = 1; blocksize <= 32; blocksize += 1) {
		cudaMalloc((void**)&gpumatrix, sizeof(int) * SIZE * SIZE);
		cudaMalloc((void**)&gpuresult, sizeof(int) * SIZE * SIZE);
		cudaMemcpy(gpumatrix, matrix
                   , sizeof(int) * SIZE * SIZE
                   , cudaMemcpyHostToDevice);
		int blocksum = ceil((double)SIZE / blocksize)
                       * ceil((double)SIZE / blocksize);
		dim3 dimBlock(blocksize, blocksize);
		dim3 dimGrid(ceil((double)SIZE / dimBlock.x)
                     , ceil((double)SIZE / dimBlock.y));

		clock_t* cputime = (clock_t*)malloc(sizeof(clock_t) * blocksum * 2);
		clock_t* gputime;
		cudaMalloc((void**)&gputime, sizeof(clock_t) * blocksum * 2);

		trans <<<dimGrid, dimBlock, 0 >>> (gpumatrix, gpuresult, SIZE, gputime);

		cudaMemcpy(result, gpuresult
                   , sizeof(int) * SIZE * SIZE
                   , cudaMemcpyDeviceToHost);
		cudaMemcpy(cputime, gputime
                   , sizeof(clock_t) * blocksum * 2
                   , cudaMemcpyDeviceToHost);

		cudaFree(gpumatrix);
		cudaFree(gpuresult);
		cudaFree(gputime);

		if (!verification(matrix, result, SIZE)) {
			fprintf(stderr, "wrong!\n");
		}
		else {
			fprintf(stdout, "right!\n");
		}

		clock_t min_start = cputime[0], max_end = cputime[blocksum];
		for (int i = 1; i < blocksum; i += 1) {
			if (min_start > cputime[i]) {
				min_start = cputime[i];
			}
			if (max_end < cputime[i + blocksum]) {
				max_end = cputime[i + blocksum];
			}
		}

		fprintf(stdout
                , "total time is %f, blocksize is %d\n"
                , log10((double)(max_end - min_start))
                , blocksize);
		free(cputime);
	}


	free(matrix);
	free(result);
	scanf("%d", &SIZE);
	return 0;
}
