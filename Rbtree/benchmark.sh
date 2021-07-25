#! /bin/bash

for dir in "original" "std_multiset" "std_multiset_emplacehint" "boost_container_multiset" "boost_intrusive_multiset"; do
    cd $dir
    make

    echo "Current: $dir"

    ptmalloc=$(perf stat --repeat 5 ./rbtree 2>&1 | grep "time elapsed" | awk '{ print $1 }')
    echo "ptmalloc: $ptmalloc"

    jemalloc=$(LD_PRELOAD=/usr/lib/libjemalloc.so perf stat --repeat 5 ./rbtree 2>&1 | grep "time elapsed" | awk '{ print $1 }')
    echo "jemalloc: $jemalloc"

    tcmalloc=$(LD_PRELOAD=/usr/lib/libtcmalloc.so perf stat --repeat 5 ./rbtree 2>&1 | grep "time elapsed" | awk '{ print $1 }')
    echo "tcmalloc: $tcmalloc"

    mimalloc=$(LD_PRELOAD=/usr/lib/libmimalloc.so perf stat --repeat 5 ./rbtree 2>&1 | grep "time elapsed" | awk '{ print $1 }')
    echo "mimalloc: $mimalloc"

    hoard=$(LD_PRELOAD=/usr/lib/libhoard.so perf stat --repeat 5 ./rbtree 2>&1 | grep "time elapsed" | awk '{ print $1 }')
    echo "hoard: $hoard"


    rm rbtree
    cd ..
    printf "$dir\t$ptmalloc\t$jemalloc\t$tcmalloc\t$mimalloc\t$hoard\n" >> result.txt
done