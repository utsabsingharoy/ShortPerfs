
/*****
* Author : Utsab Singha Roy.
* T&C : Do whatever you want with it(copy, share, hack, discuss etc) as nothing much
*       except gaining knowledge can be accomplished with it. 
*       If using please cite source, don't plagiarize and don't blame me for anything.
* 14th Sept, 2019

This is a validation of Matt Austern's paper "Why you shouldn't use set (and what you
should use instead)" 
http://lafstern.org/matt/col1.pdf 
The real life scenario is data is collected and stored in a container. Only after data
storing is complete it will be used. So data insertion in the container and data find
are done in separate phases and they are not interspersed. The complexity of the find
step must be O(log(n))

The alternatives are :
1) Store the data in a vector as it comes. Before the find step sort it and remove
   duplicate. 
2) Store the data in a vector keeping sorted order.
3) Put the data in a std::set

In all cases the find will be O(log(n)). So find part is not measured.
In the setup phase some random numbers are created and stored in a vector ( this step
does not contribute to perf measurement). We are going to measure the three ways
of storing the data in a container.
*******/

#include <benchmark/benchmark.h>
#include <vector>
#include <set>
#include <algorithm>

static void BM_vector_insert_sort(benchmark::State& state) {
    std::vector<int> vorig;
    vorig.resize(state.range(0));
    std::generate(vorig.begin(), vorig.end(), rand);
    for (auto _ : state) {
        std::vector<int> v;
        for(auto i : vorig)
            v.push_back(i);
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
        benchmark::DoNotOptimize(v);
    }
}

static void BM_vector_insert_keep_sorted(benchmark::State& state) {
    std::vector<int> vorig;
    vorig.resize(state.range(0));
    std::generate(vorig.begin(), vorig.end(), rand);
    for (auto _ : state) {
        std::vector<int> v;
        for(auto i : vorig) {
            if(!std::binary_search(v.begin(), v.end(), i)) {
                v.push_back(i);
                auto i_pos = v.rbegin();
                for(auto rit = v.rbegin()+1; rit != v.rend(); ++rit, ++i_pos) {
                    if(*i_pos <  *rit)  
                        std::iter_swap(i_pos, rit); 
                    else
                        break;
                }
            }
        }
        benchmark::DoNotOptimize(v);
    }
}

static void BM_set_insert(benchmark::State& state) {
    std::vector<int> v;
    v.resize(state.range(0));
    std::generate(v.begin(), v.end(), rand);
    for (auto _ : state) {
        std::set<int> s;
        for(auto i : v)
            s.insert(i);
        benchmark::DoNotOptimize(s);
    }
}

BENCHMARK(BM_vector_insert_sort)
    ->Range(8, 1024*1024)
    ->Unit(benchmark::kMillisecond);
BENCHMARK(BM_vector_insert_keep_sorted)
    ->Range(8, 5000)
    ->Unit(benchmark::kMillisecond);
BENCHMARK(BM_set_insert)
    ->Range(8, 1024*1024)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();

/*****
g++ IMP_RESULT/Containter_VectorSet_1.cpp  -pthread -I ../../benchmark/include/  \
        -std=c++14 -L ../../benchmark/build/src/ -lbenchmark -O3
Run on (8 X 3900 MHz CPU s)
CPU Caches:
  L1 Data 32K (x4)
  L1 Instruction 32K (x4)
  L2 Unified 256K (x4)
  L3 Unified 8192K (x1)
-------------------------------------------------------------------------
Benchmark                               Time             CPU   Iterations
-------------------------------------------------------------------------
vector_insert_sort/8                0.000 ms        0.000 ms      6041712
vector_insert_sort/64               0.001 ms        0.001 ms      1270132
vector_insert_sort/512              0.005 ms        0.005 ms       137256
vector_insert_sort/4096             0.166 ms        0.166 ms         4339
vector_insert_sort/32768             1.79 ms         1.79 ms          391
vector_insert_sort/262144            17.0 ms         17.0 ms           41
vector_insert_sort/1048576           72.7 ms         72.7 ms           10
vector_insert_keep_sorted/8         0.000 ms        0.000 ms      5299593
vector_insert_keep_sorted/64        0.001 ms        0.001 ms       626255
vector_insert_keep_sorted/512       0.051 ms        0.051 ms        12826
vector_insert_keep_sorted/4096       2.41 ms         2.41 ms          282
vector_insert_keep_sorted/5000       3.75 ms         3.75 ms          191
set_insert/8                        0.000 ms        0.000 ms      3195775
set_insert/64                       0.002 ms        0.002 ms       357345
set_insert/512                      0.031 ms        0.031 ms        23893
set_insert/4096                     0.450 ms        0.450 ms         1575
set_insert/32768                     6.18 ms         6.18 ms          105
set_insert/262144                     104 ms          104 ms            7
set_insert/1048576                    668 ms          668 ms            1

In the case when data insertion and find stage are not interspersed, keeping
the data in a vector and then sorting it before use (vector_insert_sort)
is clearly more performant than using a set (set_insert). Find stage of 
both the cases is O(log(n)).

On top of that the vector solution uses much less space as each node of
std::set can cost 32 bytes of meta data (left, right, parent pointers and
color). If we are storing integers which is generally 4 bytes, we are 
having 8 times more memory overhead.

When the data insertions and finds are interspersed the vector can be kept
sorted while inserting (vector_insert_keep_sorted). But due to O(n) nature
of insertion its very slow and should be avoided, unless the number of 
items is very small ( for integers, its less than 512 items and should be
smaller for larger objects). For small number of items vector_insert_keep_sorted
is still a valid alternative as it has low memory consumption and vector has
best locality of reference so find should be faster. For higher number 
of items the algorithmic complexity dominates. So, for interspersed access in
general std::set should be used. 
*****/
