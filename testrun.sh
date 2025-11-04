#!/bin/bash

make clean
test_files=($(find . -type f -name '*test*' -name '*.cpp' -exec basename {} .cpp \;))
mkdir -p build
for test_file in "${test_files[@]}"; do
    make test EXECS=${test_file}
done

for test_file in "${test_files[@]}"; do
    ./build/${test_file} > ./build/${test_file}.out 2>&1
    echo ${test_file} "finished"
done
