/*****
* Author : Utsab Singha Roy.
* T&C : Do whatever you want with it(copy, share, hack, discuss etc) as nothing much
*       except gaining knowledge can be accomplished with it. 
*       If using please cite source, don't plagiarize and don't blame me for anything.
* 14th Sept, 2019
* 
* This is to investigate whether replacing a recursive function with a non recursive
* version with explicit stack can be beneficial for performance.
* The premise is that, since the size of the recursion depth is not known, any
* implicit stack would use an heap based container whereas the recursive implementation
* uses the faster system stack and the function call overhead is much less than the
* dynamic memory allocation overhead.
* 
* The simple recursive function , takes an input devised it by 2 can recursively calls
* itself. On each recursive call a counter is incremented. The function is kept simple
* purposefully so that only the effects of dynamic memory of containers can be measured. 
* 
* We are going to test the recursive version against four non-recursive explicit stack 
* based version which use different containers deque, vector, list and a statically 
* allocated array as underlying container of the std::stack.
* 
******/

#include<iostream>
#include<stack>
#include <benchmark/benchmark.h>
#include<vector>
#include<list>

/*recursive implementation of counting function*/
void recursive(int num, int& result) {
    if(num == 0)
        return;
    ++result;
    recursive(num/2, result);
    recursive(num/2, result);
}

/*Recursion is replaced by explicit std::stack
  which uses deque as underlying container. */
void nonrecursive_deque(int num, int& result) {
    std::stack<int> S;
    S.push(num);
    while(! S.empty() ) {
        int curr = S.top();
        S.pop();
        if(curr == 0)
            continue;
        ++result;
        S.push(curr/2);
        S.push(curr/2);
    }
}

/*Recursion is replaced by explicit std::stack
  which uses vector as underlying container. */
void nonrecursive_vector(int num, int& result) {
    std::stack<int, std::vector<int>> S;
    S.push(num);
    while(! S.empty() ) {
        int curr = S.top();
        S.pop();
        if(curr == 0)
            continue;
        ++result;
        S.push(curr/2);
        S.push(curr/2);
    }
}

/*Recursion is replaced by explicit std::stack
  which uses list as underlying container. */
void nonrecursive_list(int num, int& result) {
    std::stack<int, std::list<int>> S;
    S.push(num);
    while(! S.empty() ) {
        int curr = S.top();
        S.pop();
        if(curr == 0)
            continue;
        ++result;
        S.push(curr/2);
        S.push(curr/2);
    }
}

/*An implementation of fixed size array which has 
  all the functions needed by std::stack. 
  Implementation Assumes container size is enough 
  to hold the recursion stack and does not perform
  bound check. 
  How was the bound obtained?
  Max input for this test is 1024*1024. It was run with
  that input and checked whats max size of the stack.
  The formula is ceil(lg(num+1))+1. With num=1024*1024
  the value is 22. 30 as taken as array size to be on
  the safe side*/
constexpr static size_t RECURSION_STACK_MAX_SIZE = 100;
template<typename T>
struct myarray {
    typedef T value_type ;
    typedef  T& reference;
    typedef  const T& const_reference;
    typedef  std::size_t size_type;
    T arr_[RECURSION_STACK_MAX_SIZE];
    size_t indx_ = 0;
    void push_back(T&& item) { arr_[indx_++] = item;}
    void push_back(const T& item) { arr_[indx_++] = item;}
    void pop_back() { --indx_; }
    bool empty() const  { return indx_ == 0;}
    constexpr std::size_t size() const{ return indx_;}
    reference back() { return (arr_[indx_-1]);}
    const_reference back() const { return arr_[indx_-1];}
};
void nonrecursive_array(int num, int& result) {
    std::stack<int, myarray<int>> S;
    S.push(num);
    while(! S.empty() ) {
        int curr = S.top();
        S.pop();
        if(curr == 0)
            continue;
        ++result;
        S.push(curr/2);
        S.push(curr/2);
    }
}

/*This is similar as above, only using a vector and
  its size is reserved during creation*/
template<typename T>
struct myvector : public std::vector<T> {
    myvector() { this->reserve(RECURSION_STACK_MAX_SIZE);}  
};
void nonrecursive_vector_reserved(int num, int& result) {
    std::stack<int, myvector<int>> S;
    S.push(num);
    while(! S.empty() ) {
        int curr = S.top();
        S.pop();
        if(curr == 0)
            continue;
        ++result;
        S.push(curr/2);
        S.push(curr/2);
    }
}

static void BM_recursive(benchmark::State& state) {
    int num = state.range(0);
    int result;
    for(auto _ : state) {
        result = 0;
        recursive(num, result);
        benchmark::DoNotOptimize(result);
    }
}
static void BM_nonrecursive_deque(benchmark::State& state) {
    int num = state.range(0);
    int result;
    for(auto _ : state) {
        result = 0;
        nonrecursive_deque(num, result);
        benchmark::DoNotOptimize(result);
    }
}
static void BM_nonrecursive_vector(benchmark::State& state) {
    int num = state.range(0);
    int result;
    for(auto _ : state) {
        result = 0;
        nonrecursive_vector(num, result);
        benchmark::DoNotOptimize(result);
    }
}
static void BM_nonrecursive_array(benchmark::State& state) {
    int num = state.range(0);
    int result;
    for(auto _ : state) {
        result = 0;
        nonrecursive_array(num, result);
        benchmark::DoNotOptimize(result);
    }
}
static void BM_nonrecursive_list(benchmark::State& state) {
    int num = state.range(0);
    int result;
    for(auto _ : state) {
        result = 0;
        nonrecursive_list(num, result);
        benchmark::DoNotOptimize(result);
    }
}
static void BM_nonrecursive_vector_reserved(benchmark::State& state) {
    int num = state.range(0);
    int result;
    for(auto _ : state) {
        result = 0;
        nonrecursive_vector_reserved(num, result);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_recursive)
    ->Arg(1024*1024)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_nonrecursive_deque)
    ->Arg(1024*1024)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_nonrecursive_vector)
    ->Arg(1024*1024)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_nonrecursive_list)
    ->Arg(1024*1024)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_nonrecursive_array)
    ->Arg(1024*1024)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_nonrecursive_vector_reserved)
    ->Arg(1024*1024)
    ->Unit(benchmark::kMicrosecond);
BENCHMARK_MAIN();

/************************************************************************************************************
* Linux 4.15.0-46-generic #49~16.04.1-Ubuntu SMP x86_64 x86_64 x86_64 GNU/Linux
* Run on (8 X 3900 MHz CPU s)
* CPU Caches:
*   L1 Data 32K (x4)
*   L1 Instruction 32K (x4)
*   L2 Unified 256K (x4)
*   L3 Unified 8192K (x1)
* Load Average: 0.67, 0.49, 0.63
* Results : 
* g++ RecursionVsNonRecursive.cpp -pthread -I ../../benchmark/include/  \
*  -std=c++14 -L ../../benchmark/build/src/ -lbenchmark -O3
* BM_recursive/1048576                          1864 us         1864 us          379
* BM_nonrecursive_deque/1048576                 7270 us         7270 us           93
* BM_nonrecursive_vector/1048576                6607 us         6607 us          102
* BM_nonrecursive_list/1048576                 99013 us        99009 us            7
* BM_nonrecursive_array/1048576                 6113 us         6112 us          114
* BM_nonrecursive_vector_reserved/1048576       7894 us         7894 us           86
* 
* g++ RecursionVsNonRecursive.cpp -pthread -I ../../benchmark/include/ \
*     -std=c++14 -L ../../benchmark/build/src/ -lbenchmark -Os
* BM_recursive/1048576                         10490 us        10489 us           65
* BM_nonrecursive_deque/1048576                20630 us        20630 us           34
* BM_nonrecursive_vector/1048576               16675 us        16674 us           42
* BM_nonrecursive_list/1048576                112060 us       112057 us            6
* BM_nonrecursive_array/1048576                 9908 us         9908 us           71
* BM_nonrecursive_vector_reserved/1048576      16756 us        16756 us           42
* 
* clang++ RecursionVsNonRecursive.cpp -pthread -I ../../benchmark/include/ \
*      -std=c++14 -L ../../benchmark/build/src/ -lbenchmark -O3
* BM_recursive/1048576                          4037 us         4037 us          174
* BM_nonrecursive_deque/1048576                 7936 us         7936 us           86
* BM_nonrecursive_vector/1048576                4039 us         4039 us          171
* BM_nonrecursive_list/1048576                 98227 us        98228 us            7
* BM_nonrecursive_array/1048576                 4046 us         4046 us          174
* BM_nonrecursive_vector_reserved/1048576       3987 us         3987 us          172
* 
* clang++ RecursionVsNonRecursive.cpp -pthread -I ../../benchmark/include/ \
*      -std=c++14 -L ../../benchmark/build/src/ -lbenchmark -Os
* BM_recursive/1048576                          4191 us         4191 us          164
* BM_nonrecursive_deque/1048576                 7836 us         7835 us           88
* BM_nonrecursive_vector/1048576                7131 us         7131 us           97
* BM_nonrecursive_list/1048576                 96947 us        96949 us            7
* BM_nonrecursive_array/1048576                 6782 us         6782 us           98
* BM_nonrecursive_vector_reserved/1048576       7548 us         7548 us           91
* 
* Observations :
* 1) Performance-wise : 
*    recursive > array-stack > vector-stack > deque-stack > list-stack
*    Irrespective of optimization or compiler, recursive version beats others.
* 2) Vector based stack has comparable performance as deque based stack. 
* 3) If maximum stack size is known, a pre-allocated array based stack can be better
*    than std::stack if -Os is used. Otherwise, default deque based std::stack gives 
*    competitive performance.
*   
* How does it prove the effect is due to stack vs heap memory access?
* In this test all of the functions perform same operations, except the recursive call 
* has function call overhead whereas the non-recursive versions perform empty-check and 
* executes loop-branching which kind of cancel each other. ( This argument is very naive 
* as compiler performs much more optimizations.)The point is these extraneous factors may
* not be significant as each has comparable overhead. The only part that is different 
* is memory access. Also, this pattern perfectly matches with the conventional wisdom
* that array is more performant whereas list is least performant.
* 
* Why there is significant performance difference between recursive vs array based
* stack implementation in g++ with -O3 but not with -Os ?
* Looking at generated assembly , gcc aggressively optimized the recursive function with
* -O3 and the output assembly of the recursive function was quite large. But with -Os,
* it only applied tail call optimization. The advanced options made the recursive call
* faster. For example the with -O3 IPC is 2.40 whereas with -Os its 1.24 as measured by
* perf.
* 
* The curious case of reserved vector:
* The stack based on reserved vector did not quite perform as expected.It turned out 
* to be same as normal vector. With the present input the stack has to hold 22 
* integers (88 bytes). A normal vector took 6 allocations where as the reserved vector
* took 1 allocation. 6 allocations may be quite a small number to prove the other's
* efficacy.   
* Nevertheless, the reserved vector solution can use an approximate max size of
* the stack where as array solution needs the array size to be greater than or equal
* to max stack size.
* 
* Calculating approximate max size of the stack:
* Suppose the algorithm recursively calls itself b times. The call tree can be 
* visualized as a tree with branching factor b. Each a element is popped off
* the stack b new items are pushed thus augmenting stack size by (b-1). If the 
* depth of recursion tree is d , the max number of items simultaneously residing
* in the stack is d(b-1) + 1. For example d is O(lg(input_size)) if input is halved
* on each call. Having said that, if the recursive algorithm calls itself only once
* (eg. binary search) from the formula above the stack size is 1 as the branching 
* factor is 1. In such case, replacing the recursion with iteration is definitely
* more performant.
***********************************/
