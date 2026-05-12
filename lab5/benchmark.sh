#!/bin/bash

# Компиляция
g++ -O3 z_build.cpp -o zfunc.out
g++ -O3 main.cpp -o tree.out
g++ -O3 naive_build.cpp -o naive.out
g++ gen.cpp -o gen.out

# Размер теста (1 миллион символов)
N=500000
echo "Генерируем тест на $N символов..."
echo $N | ./gen.out > /dev/null

run_test() {
    local label=$1
    local cmd=$2
    local testfile=$3
    echo -n "Running $label... "
    /usr/bin/time -v ./$cmd < $testfile > /dev/null 2> res.txt
    TIME_VAL=$(grep "Elapsed (wall clock) time" res.txt | cut -d ':' -f 2- | xargs)
    MEM_VAL=$(grep "Maximum resident set size" res.txt | cut -d ':' -f 2- | xargs)
    echo "Done."
    echo "   Time: $TIME_VAL"
    echo "   Memory: $MEM_VAL KB"
}

TEST_FILE="test_killer.txt"
echo -e "\n--- BENCHMARK RESULTS (N=$N) ---"
run_test "naive" "naive.out" "$TEST_FILE"
run_test "BOOTH (Z-Style)" "zfunc.out" "$TEST_FILE"
run_test "SUFFIX TREE" "tree.out" "$TEST_FILE"