/*****
* Author : Utsab Singha Roy.
* T&C : Do whatever you want with it(copy, share, hack, discuss etc) as nothing much
*       except gaining knowledge can be accomplished with it. 
*       If using please cite source, don't plagiarize and don't blame me for anything.
* 14th Sept, 2019

* In GoingNative 2013, Sean Parent had shown an implementation of insertion sort 
* using std::rotate.
* Here we are measuring relative performance between that vs traditional insertion
* sort implementation to sort vector. The rotate version can work with forward 
* iterators whereas the traditional version uses bidirectional iterator.
* We are going use a vector of 20 or 50 elements which can be random, sorted or
* reverse sorted.
******/

#include <vector>
#include <benchmark/benchmark.h>

enum InputType {SMALL_RANDOM = 0, SMALL_SORTED = 1, SMALL_REVSORTED = 2,
                LARGE_RANDOM = 3, LARGE_SORTED = 4, LARGE_REVSORTED = 5 }; 

std::vector<int> get_vector(InputType type) {
    std::vector<int> vec;
    if(type == InputType::SMALL_RANDOM) {
        vec = {91,15,45,59,80,76,81,12,99,71,83,44,46,52,34,53,62,67,40,29};
    }
    else if(type == InputType::SMALL_SORTED) {
        vec = {12,15,29,34,40,44,45,46,52,53,59,62,67,71,76,80,81,83,91,99};
    }
    else if(type == InputType::SMALL_REVSORTED) {
        vec = {99,91,83,81,80,76,71,67,62,59,53,52,46,45,44,40,34,29,15,12};
    }
    else if(type == InputType::LARGE_RANDOM) {
        vec = {10,82,70,14,31,84,57,75,73,60,95,74,27,63,66,24,25,6,65,36,26,16,54,45,\
            99,18,19,59,44,30,85,3,15,67,61,93,98,97,35,71,88,76,32,64,9,39,83,34,40,79};
    }
    else if(type == InputType::LARGE_SORTED) {
        vec = {3,6,9,10,14,15,16,18,19,24,25,26,27,30,31,32,34,35,36,39,40,44,45,54,57,\
            59,60,61,63,64,65,66,67,70,71,73,74,75,76,79,82,83,84,85,88,93,95,97,98,99};
    }
    else {
        vec = {99,98,97,95,93,88,85,84,83,82,79,76,75,74,73,71,70,67,66,65,64,63,61,60,\
            59,57,54,45,44,40,39,36,35,34,32,31,30,27,26,25,24,19,18,16,15,14,10,9,6,3};
    }
    return vec;
}

template<typename FI, typename COMP>
void insertion_sort_fwd_iter_rotate(FI begin, FI end, const COMP& comp) {
    for(auto it = std::next(begin); it != end; ++it) {
        //upper bound works on half open range [first, last), ie last is not part
        //search space. In Insertion sort , we are searching for a location for 
        // *it, in (begin, it-1) ie, in half open range (begin, it]. 
        //auto pos = std::upper_bound(begin, it, *it, comp);
        auto pos = std::find_if(begin , it, [&comp, &it](const auto& i){ return comp(*it, i); });
        // Similarly rotate works on half open range [pos, std::next(it)).
        std::rotate(pos, it, std::next(it));
    }
}

template<typename BI, typename COMP>
void insertion_sort_bidir_iter( BI begin, BI end, const COMP& comp) {
    for(auto it = std::next(begin); it != end; ++it) {
        auto it1 = it;
        while(comp(*it1, *std::prev(it1))) {
            std::iter_swap(it1, std::prev(it1));
            --it1;
            if(it == begin) break;
        }
    }
}

static void BM_insertion_sort_fwd_iter(benchmark::State& state) {
    for(auto _ : state) {
        std::vector<int> v = get_vector(static_cast<InputType>(state.range(0)));
        benchmark::DoNotOptimize(v);
        insertion_sort_fwd_iter_rotate(v.begin(), v.end(), std::less<int>());
    }
}

static void BM_insertion_sort_bidir_iter(benchmark::State& state) {
    for(auto _ : state) {
        std::vector<int> v = get_vector(static_cast<InputType>(state.range(0)));
        benchmark::DoNotOptimize(v);
        insertion_sort_bidir_iter(v.begin(), v.end(), std::less<int>());
    }
}

BENCHMARK(BM_insertion_sort_fwd_iter)
    ->Arg(0)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_insertion_sort_bidir_iter)
    ->Arg(0)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(5)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK_MAIN();

/*****
* g++ Algorithm_InsertionSortImpl.cpp  -pthread -I ../../benchmark/include/  \
*     -std=c++14 -L ../../benchmark/build/src/ -lbenchmark -O3
* Run on (8 X 3900 MHz CPU s)
* CPU Caches:
*   L1 Data 32K (x4)
*   L1 Instruction 32K (x4)
*   L2 Unified 256K (x4)
*   L3 Unified 8192K (x1)
* Load Average: 0.53, 0.35, 0.41
* -------------------------------------------------------------------------
* Benchmark                               Time             CPU   Iterations
* -------------------------------------------------------------------------
* BM_insertion_sort_fwd_iter/0        0.154 us        0.154 us      4496487
* BM_insertion_sort_fwd_iter/1        0.114 us        0.114 us      6125300
* BM_insertion_sort_fwd_iter/2        0.158 us        0.158 us      4158298
* BM_insertion_sort_fwd_iter/3        0.473 us        0.473 us      1479566
* BM_insertion_sort_fwd_iter/4        0.381 us        0.381 us      1852707
* BM_insertion_sort_fwd_iter/5        0.457 us        0.457 us      1540695
* BM_insertion_sort_bidir_iter/0      0.114 us        0.114 us      6243115
* BM_insertion_sort_bidir_iter/1      0.037 us        0.037 us     19380921
* BM_insertion_sort_bidir_iter/2      0.174 us        0.174 us      3997608
* BM_insertion_sort_bidir_iter/3      0.525 us        0.525 us      1309856
* BM_insertion_sort_bidir_iter/4      0.083 us        0.083 us      7840301
* BM_insertion_sort_bidir_iter/5       1.57 us         1.57 us       451557
* 
* Since in practice insertion sort is used on very small arrays case 0,1,2
* above are more important than case 3,4,5 which has been added just to
* observe its behaviour.
* 
* std::rotation based sort is faster if the array is reverse sorted. Otherwise
* traditional insertion sort is faster specially if the array is sorted.
* But given that very small running time of both cases as the input is so small
* any one of the version is perfect for almost all situations.
* In super performance critical cases, bidirection iterator based traditional
* insertion sort should be used as its better in 2 out of 3 cases.
*****/
