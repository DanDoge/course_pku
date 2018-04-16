/*sigmoid function
* Author    : Huang Daoji
* StudentID : 1600017857
* Date      : 2018-04-16
*/

// header files
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cuda_runtime.h>

#define M_PI_2        1.57079632679489661923	/* pi/2 */
#define M_PI_2_INV    (1.0/M_PI_2)
#define M_2_SQRTPI    1.12837916709551257390    /* 2/sqrt(pi) */
#define ERF_COEF      (1.0/M_2_SQRTPI)
#define threhold      100000000


void verification(){
    double ans = 0.0;
    for(double i = 1.0; i <= 100000000.0; i += 1.0){
        ans += 1 / (1 + exp(-i));
    }
    printf("result on cpu: %10.10f. \n", ans);
}

// print some basic parameters
void printDeviceProp(const cudaDeviceProp &prop) {
	printf("Device Name : %s.\n", prop.name);
	printf("totalGlobalMem : %d.\n", prop.totalGlobalMem);
	printf("sharedMemPerBlock : %d.\n", prop.sharedMemPerBlock);
	printf("regsPerBlock : %d.\n", prop.regsPerBlock);
	printf("warpSize : %d.\n", prop.warpSize);
	printf("memPitch : %d.\n", prop.memPitch);
	printf("maxThreadsPerBlock : %d.\n", prop.maxThreadsPerBlock);
	printf("maxThreadsDim[0 - 2] : %d %d %d.\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
	printf("maxGridSize[0 - 2] : %d %d %d.\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
	printf("totalConstMem : %d.\n", prop.totalConstMem);
	printf("major.minor : %d.%d.\n", prop.major, prop.minor);
	printf("clockRate : %d.\n", prop.clockRate);
	printf("textureAlignment : %d.\n", prop.textureAlignment);
	printf("deviceOverlap : %d.\n", prop.deviceOverlap);
	printf("multiProcessorCount : %d.\n", prop.multiProcessorCount);
}

// find a device, and quit
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


// kernel function here
__global__ static void calc(double* gpuans) {
	__shared__ double tmp[100];
	int bid = blockIdx.x * 100 + blockIdx.y;
	int tid = threadIdx.x * 10 + threadIdx.y;

	double idx = bid * 10000 + tid * 100;
	double ans = 0.0;
	if (idx < threhold) {
		for (double i = 1.0; i <= 100.0; i += 1.0) {
			ans += 1 / (1 + exp(-idx - i));
		}
	}
	else {
		for (double i = 1.0; i <= 100.0; i += 1.0) {
			ans += M_PI_2_INV * atan(M_PI_2 * (i + idx));
		}
	}
	tmp[tid] = ans;
    __syncthreads();
    /*
	int offset = 1, mask = 1;
	while (offset < 100) {
		if ((tid & mask) == 0 && tid + mask < 100) {
			tmp[tid] += tmp[tid + offset];
		}
		offset = offset << 1;
		mask = mask + offset;
		__syncthreads();
	}*/
    int i = 50;
    while(i != 0){
        if(tid < i){
            tmp[tid] = tmp[tid + i] + tmp[tid];
        }
        __syncthreads();
        i /= 2;
    }
	if (tid == 0) {
        /*
        double res = 0.0;
        for(int i = 0; i < 100; i += 1){
            res += tmp[i];
        }*/
		gpuans[bid] = tmp[0];
	}
}

int main() {
	if (!InitCUDA()) {
		return 0;
	}

	// warmup
	double* warmup = (double*)malloc(sizeof(double) * 1024 * 1024);
	double* gpuwarmup;
	cudaMalloc((void**)&gpuwarmup, sizeof(double) * 1024 * 1024);
	cudaMemcpy(gpuwarmup, warmup, sizeof(double) * 1024 * 1024, cudaMemcpyHostToDevice);

	//
	dim3 dimBlock(10, 10);
	dim3 dimGrid(100, 100);

	clock_t start, stop;
	start = clock();
	double* ans = (double*)calloc(10000, sizeof(double));
	double* gpuans;
	cudaMalloc((void**)&gpuans, sizeof(double) * 10000);
	cudaMemcpy(gpuans, ans, sizeof(double) * 10000, cudaMemcpyHostToDevice);
	calc<<<dimGrid, dimBlock >>>(gpuans);
	cudaMemcpy(ans, gpuans, sizeof(double) * 10000, cudaMemcpyDeviceToHost);

	double res = 0.0;
	for (int i = 0; i < 10000; i += 1) {
		res += ans[i];
	}
	stop = clock();
	double t_ns = (stop - start) / (double)(CLOCKS_PER_SEC);
	printf("%10.10f s\n", t_ns);
	printf("result is: %10.10f. \n", res);
    verification();
	return 0;
}

/* end */
