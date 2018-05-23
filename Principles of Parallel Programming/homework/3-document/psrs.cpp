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

void merge(vector<int> &a, vector<int> &b, int l, int m, int r);
void mergeSort(vector<int> &a, vector<int> &b, int l, int r);

auto constexpr n = 1'000'000;

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
	MPI_Scatter(tovoid(v), size, MPI_INT, this_array, size, MPI_INT, 0, MPI_COMM_WORLD);

    // and each thread sort its own part
    vector<int> tmp_array(size);
	mergeSort(this_array, tmp_array, 0, (size - 1));

// choose pivot!

// select pivot!

// sort each part again!

// then collect them!


    // collect parts, sort all
	vector<int> sorted;
	if(rank == 0) {
		sorted.resize(n);
	}

	MPI_Gather(tovoid(this_array), size, MPI_INT, tovoid(sorted), size, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank == 0) {
        vector<int> other_array(n);
		mergeSort(sorted, other_array, 0, (n - 1));
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to sort a array of "
        << n << " ints : " << diff.count() << " s\n";

    print(v);
}





/********** Merge Function **********/
void merge(vector<int> &a, vector<int> &b, int l, int m, int r) {

	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;

	while((h <= m) && (j <= r)) {

		if(a[h] <= a[j]) {

			b[i] = a[h];
			h++;

			}

		else {

			b[i] = a[j];
			j++;

			}

		i++;

		}

	if(m < h) {

		for(k = j; k <= r; k++) {

			b[i] = a[k];
			i++;

			}

		}

	else {

		for(k = h; k <= m; k++) {

			b[i] = a[k];
			i++;

			}

		}

	for(k = l; k <= r; k++) {

		a[k] = b[k];

		}

	}

/********** Recursive Merge Function **********/
void mergeSort(vector<int> &a, vector<int> &b, int l, int r) {

	int m;

	if(l < r) {

		m = (l + r)/2;

		mergeSort(a, b, l, m);
		mergeSort(a, b, (m + 1), r);
		merge(a, b, l, m, r);

		}

	}
