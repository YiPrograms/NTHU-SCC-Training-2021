# HW: Boost RBTree Report
109062302 郭品毅

## Environmental Setup

### Hardware stack
- Host: HP Pavilion Power Laptop 15-cb0xx
- CPU: Intel i7-7700HQ , 4 Cores 8 Threads @ 3.800GHz
- Memory: 16 GB

### Software stack
- OS: Arch Linux
- Kernel: 5.10.16-arch1-1
- g++ (GCC) 10.2.0
- clang 11.0.1

## Boosting Methods

> All the timing results are from `perf stat --repeat 5 ./rbtree`

### Changing compiler
By experiments, `g++` produces faster binaries than `clang++` in this program, though the time difference is very small (4.57 and 4.59 seconds)

### Changing compile args
- Changing from `-O2` to `-O3` gives a slightly speed improvement (from 4.5744+-0.0252 to 4.438+-0.124 seconds)
- Adding the `-match=native` flag gives gives a speed improvement as well (4.1553+-0.0690 seconds)

From the experiments, changing compile args does boost the performance a bit.
So all the experiments below will use the compile args:
```
g++ -O3 -march=native rbtree.cpp -o rbtree
```

### Changing RBTree Implementations

- Original: 4.11s in average

- `std::multiset`: 3.98s in average
    - The RBTree in STL is a little bit faster than the provided one, but not significant. From my teammate, lawrence's perspective, it's because the implementation in STL accesses memory less frequently than the provided one, so the performance is better.

- `std::multiset` with `emplace_hint`: 2.41s in average
    - If `emplace_hint` is used to insert elements, it will start searching where to insert at the hint we given, and because the inserting sequence is mostly increasing, so we can record the last insertion position and give it as the hint for the next insertion, so the target can be inserted immediately after the hint. This changes the insertion complexity from O(logn) to amortized O(1).

- `boost::container::multiset` with `emplace_hint`: 4.974s in average
    - I'm not sure why the multiset in boost libs is slower in our case. Mabye the implementation is not suitable in our use case.

- `boost::intrusive::multiset`: 5.31s in average
    - According to boost docs, intrusive templates are optimized for performance, the main difference with STL is that it uses hooks so elements do not need to have another copy in the container, thus the big performance improvement, however, we have to store all the elements in another container without destroying it, and we need to implement a warpper class for it to work, so in our case, it became even slower.

In conclusion, `std::multiset` with `emplace_hint` is the fastest among my list.

![](https://i.imgur.com/zBc8Qq8.png)



### Changing `malloc()` implementations

Creating a RBTree needs to allocate memory many times, and allocating memory is a costly operation.
There are different `malloc` and `new` implementatons that are faster than the `malloc` in libc.
We can use them by linking different `malloc` using `LD_PRELOAD` environment variable

Here are some widely used `malloc` implementations:

- ptmalloc
    - this is the default `malloc` implementation used in `libc`
- [jemalloc](https://github.com/jemalloc/jemalloc)
    - jemalloc is a common alternative memory allocator that is used in Redis and Firefox
- [tcmalloc](https://github.com/google/tcmalloc)
    - tcmalloc is developed by Google and is optimized for multi-threadding memory allocations
- [mimalloc](https://github.com/microsoft/mimalloc)
    - mimalloc is developed by Microsoft and is used in various Microsoft services such as Bing and Azure
- [Hoard](https://github.com/emeryberger/Hoard)
    - Hoard is an alternative of malloc that is used by AOL, Cisco and other companies

We can use these memory allocators by providing `LD_PRELOAD` environment variable to link the `malloc` function from our library before the linker loads it from libc

## Performance Chart

![](https://i.imgur.com/xVsKWHf.png)

From the chart, we can see that `tcmalloc` and `mimalloc` are the best memory allocators among the list, but `tcmalloc` has better performance on STL libraries, and `mimalloc` has better performance on Boost libraries.

## Evaluation

In conclusion, the best result is using `std::multiset` with `emplace_hint` and preload the memory allocator with `tcmalloc`, and using the compiler flag `-O3` and `-march=native`, with 1.38 seconds

It has a 231% increase in performance, and is 3.31x faster compared to the original one (4.57s).


