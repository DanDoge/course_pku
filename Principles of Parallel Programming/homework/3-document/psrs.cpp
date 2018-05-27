// ml:std = c++14
#include <iostream>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <mpi.h>
using namespace std;

#define tovoid(x) (void*)&(x)[0] // convert std::vector<int> ==> void*
#define toint(x)  (int*)&(x)[0]  // likely, cvt std::vector<int> to int*

void merge(vector<int> &a, vector<int> &b, int l, int m, int r);
void mergeSort(vector<int> &a, vector<int> &b, int l, int r);

auto constexpr n = 100'000'000;

#define PRINT_ARRAY 1

template <class T>
void print(std::vector<T> const& a)
{
#if PRINT_ARRAY
    std::copy(
        std::begin(a),
        std::end(a),
        std::ostream_iterator<T>{std::cout, " "}
    );
    std::cout << "\n";
#endif
}

template<class T>
void verification(std::vector<T>& a){
    for(int i = 0; i < a.size() - 1; i += 1){
        if(a[i] > a[i + 1]){
            std::cout << "wrong!" << std::endl;
            return ;
        }
    }
    std::cout << "right!" << std::endl;
    return ;
}

int main(int argc, char* argv[])
{
    std::mt19937 gen(4);
    std::uniform_int_distribution<> dis{};

    std::vector<int> v(n);
    std::generate(std::begin(v), std::end(v), [&](){ return dis(gen); });

    auto start = std::chrono::high_resolution_clock::now();

	int rank;
	int size_of_threads;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size_of_threads);
	int size = n / size_of_threads; // suppose size_of_threads | n

// scatter original array to each thread

    vector<int> this_array(size);
	MPI_Scatter(tovoid(v), size, MPI_INT, tovoid(this_array), size, MPI_INT, 0, MPI_COMM_WORLD);

    // and each thread sort its own part
    std::sort(this_array.begin(), this_array.end());

// choose pivot!

    vector<int> pivot(size_of_threads);
    vector<int> pivot_tot(size_of_threads * size_of_threads);
    for(int i = 0; i < size_of_threads; i += 1){
        pivot[i] = this_array[i * size / size_of_threads];
    }
    MPI_Gather(tovoid(this_array), size_of_threads, MPI_INT, tovoid(pivot_tot), size_of_threads, MPI_INT, 0, MPI_COMM_WORLD);


// select pivot!

    if(rank == 0){
        std::sort(pivot_tot.begin(), pivot_tot.end());
        for(int i = 0; i < size_of_threads - 1; i += 1){
            pivot[i] = pivot_tot[(i + 1) * size_of_threads];
        }
    }
    MPI_Bcast(tovoid(pivot), size_of_threads - 1, MPI_INT, 0, MPI_COMM_WORLD);

// class partition

    vector<int> class_start(size_of_threads);
    vector<int> class_length(size_of_threads);
    int idx = 0;
    for(int class_idx = 0; class_idx < size_of_threads - 1; class_idx += 1){
        class_start[class_idx] = idx;
        class_length[class_idx] = 0;
        while(idx < size && this_array[idx] < pivot[class_idx]){
            class_length[class_idx] += 1;
            idx += 1;
        }
    }
    class_start[size_of_threads - 1] = idx;
    class_length[size_of_threads - 1] = size - idx;

// sort each part again!

    vector<int> received;
    vector<int> received_length(size_of_threads);
    vector<int> received_start(size_of_threads);

    for(int thread = 0; thread < size_of_threads; thread += 1){
        MPI_Gather(&class_length[thread], 1, MPI_INT, tovoid(received_length), 1, MPI_INT, thread, MPI_COMM_WORLD);
        if(rank == thread){
            received_start[0] = 0;
            for(int i = 1; i < size_of_threads; i += 1){
                received_start[i] = received_start[i - 1] + received_length[i - 1];
            }
            received.resize(received_start[size_of_threads - 1] + received_length[size_of_threads - 1]);
        }
        MPI_Gatherv(&this_array[class_start[thread]], class_length[thread], MPI_INT, tovoid(received), toint(received_length), toint(received_start), MPI_INT, thread, MPI_COMM_WORLD);
    }

    std::sort(received.begin(), received.end());

    int this_send_length = received_start[size_of_threads - 1] + received_length[size_of_threads - 1];

// then collect them!

    vector<int> send_length(size_of_threads);
    vector<int> send_starts(size_of_threads);

    MPI_Gather(&this_send_length, 1, MPI_INT, tovoid(send_length), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0){
        send_starts[0] = 0;
        for(int i = 1; i < size_of_threads; i += 1){
            send_starts[i] = send_starts[i - 1] + send_length[i - 1];
        }
    }


// collect parts, sort all

    MPI_Gatherv(tovoid(received), this_send_length, MPI_INT, tovoid(v), toint(send_length), toint(send_starts), MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to sort a array of "
        << n << " ints : " << diff.count() << " s\n";

    verification(v);
    return 0;
}
