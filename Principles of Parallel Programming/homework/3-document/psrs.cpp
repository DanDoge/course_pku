// ml:std = c++14
#include <iostream>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <queue>
#include <mpi.h>
using namespace std;

// used in multi_merge()
struct node{
    int idx_start;
    int idx_in_array;
    int val;
    node(int i_s, int i_i_a, int v){
        idx_start = i_s;
        idx_in_array = i_i_a;
        val = v;
    }
    friend bool operator<(const node& obj1, const node& obj2){
        // used in std::priority_queue
        return obj1.val > obj2.val;
    }
};


// convert std::vector to pointer, although vec.data() is introduced in c++11
#define tovoid(x) (void*)&(x)[0] // convert std::vector<int> ==> void*
#define toint(x)  (int*)&(x)[0]  // likely, cvt std::vector<int> to int*

void multi_merge(int* start[], int length[], int size, int result[], int result_length);

auto constexpr n = 100'000'000;

#define PRINT_ARRAY 0

template <class T>
void print(std::vector<T> const& a)
{
// amazing code....
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
    // what's my rank...
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // and how many peer threads(include me) do i have?
	MPI_Comm_size(MPI_COMM_WORLD, &size_of_threads);
    // suppose size_of_threads | n
	int size = n / size_of_threads;

// scatter original array to each thread

    vector<int> this_array(size);
	MPI_Scatter(tovoid(v), size, MPI_INT, tovoid(this_array), size, MPI_INT, 0, MPI_COMM_WORLD);

    // and each thread sort its own part, use std::sort here because it is fastest
    std::sort(this_array.begin(), this_array.end());

// choose pivot!

    vector<int> pivot(size_of_threads);
    vector<int> pivot_tot(size_of_threads * size_of_threads);
    for(int i = 0; i < size_of_threads; i += 1){
        pivot[i] = this_array[i * size / size_of_threads];
    }
    // and gather them
    MPI_Gather(tovoid(this_array), size_of_threads, MPI_INT, tovoid(pivot_tot), size_of_threads, MPI_INT, 0, MPI_COMM_WORLD);


// select pivot!

    if(rank == 0){
        vector<int*> s(size_of_threads);
        vector<int> l(size_of_threads);
        for(int i = 0; i < size_of_threads; i += 1){
            s[i] = &pivot_tot[i * size_of_threads];
            l[i] = size_of_threads;
        }
        vector<int> pivot_sorted(size_of_threads * size_of_threads);
        // seems to be faster, i do not choose loser tree for its conplexity
        multi_merge(&s[0], &l[0], size_of_threads, toint(pivot_sorted), size_of_threads * size_of_threads);

        for(int i = 0; i < size_of_threads - 1; i += 1){
            pivot[i] = pivot_sorted[(i + 1) * size_of_threads];
        }
    }
    // and send global pivot to all processes
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
    // the last class
    class_start[size_of_threads - 1] = idx;
    class_length[size_of_threads - 1] = size - idx;

// sort each part again!

    vector<int> received;
    vector<int> received_length(size_of_threads);
    vector<int> received_start(size_of_threads);

    for(int thread = 0; thread < size_of_threads; thread += 1){
        // how long should i send?
        MPI_Gather(&class_length[thread], 1, MPI_INT, tovoid(received_length), 1, MPI_INT, thread, MPI_COMM_WORLD);
        if(rank == thread){
            received_start[0] = 0;
            for(int i = 1; i < size_of_threads; i += 1){
                received_start[i] = received_start[i - 1] + received_length[i - 1];
            }
            // how long should i receive?
            received.resize(received_start[size_of_threads - 1] + received_length[size_of_threads - 1]);
        }
        // send & receive them!
        MPI_Gatherv(&this_array[class_start[thread]], class_length[thread], MPI_INT, tovoid(received), toint(received_length), toint(received_start), MPI_INT, thread, MPI_COMM_WORLD);
    }

    vector<int*> s(size_of_threads);
    for(int i = 0; i < size_of_threads; i += 1){
        s[i] = toint(received) + received_start[i];
    }
    // sort my part
    multi_merge(&s[0], toint(received_length), size_of_threads, toint(v), n);

    // how long should i send to root process?
    int this_send_length = received_start[size_of_threads - 1] + received_length[size_of_threads - 1];

// then collect them!

    vector<int> send_length(size_of_threads);
    vector<int> send_starts(size_of_threads);

    // tell root process how long i will send
    MPI_Gather(&this_send_length, 1, MPI_INT, tovoid(send_length), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0){
        send_starts[0] = 0;
        for(int i = 1; i < size_of_threads; i += 1){
            send_starts[i] = send_starts[i - 1] + send_length[i - 1];
        }
    }


// collect parts, now the original array is already sorted

    MPI_Gatherv(tovoid(v), this_send_length, MPI_INT, tovoid(v), toint(send_length), toint(send_starts), MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    // am i faster than std::sort()? --> usually yes...but that's under -O2!
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    // every process print its time
    std::cout << "Time to sort a array of "
        << n << " ints : " << diff.count() << " s\n";

    // root process check if i am right
    if(rank == 0){
        verification(v);
    }
    return 0;
}


// multi_merge, use std::priority_queue(binary-tree), loser-tree seems to be too conplicated..
void multi_merge(int* start[], int length[], int size, int result[], int result_length){
    priority_queue<node> queue;
    for(int i = 0; i < size; i += 1){
        if(length[i]){
            // heads of arrays
            queue.push(node(i, 0, start[i][0]));
        }
    }

    int idx_res = 0;
    while(!queue.empty() && idx_res < result_length){
        node top = queue.top();
        queue.pop();

        result[idx_res] = start[top.idx_start][top.idx_in_array];
        idx_res += 1;

        if(top.idx_in_array + 1 < length[top.idx_start]){
            // what's the next element, if any?
            queue.push(node(top.idx_start, top.idx_in_array + 1, start[top.idx_start][top.idx_in_array + 1]));
        }
    }
}
