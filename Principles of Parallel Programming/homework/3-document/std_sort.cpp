// ml:std = c++14
#include <iostream>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>

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

int main(int argc, char* argv[])
{
    std::mt19937 gen(4);
    std::uniform_int_distribution<> dis{};

    std::vector<int> v(n);
    std::generate(std::begin(v), std::end(v), [&](){ return dis(gen); });

    auto start = std::chrono::high_resolution_clock::now();

	std::sort(v.start(), v.end());

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to sort a array of "
        << n << " ints : " << diff.count() << " s\n";

    return 0;
}
