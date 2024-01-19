#!/bin/bash

passed_tests=0
total_tests=0

cd ./testing/testFirst || exit

run_test() {
    local test_name="$1"

    cd "./$test_name" || exit
    ../../../firstPass "input" > "tmp.txt"
    sed -r "s/\x1B\[[0-9;]*[mGK]//g" "tmp.txt" > "output.txt"
    diff_output=$(diff --strip-trailing-cr "output.txt" "expected.txt")

    if [ $? -eq 0 ]; then
        echo "$test_name: pass"
        echo ""
        ((passed_tests++))
    else
        echo "$test_name: error"
        echo ""
        echo "$diff_output"
    fi

    ((total_tests++))
    rm -f "tmp.txt"  # Remove temporary file
    cd ..
}


echo "---------------- test 1 ----------------"
run_test "test1_str"

echo "---------------- test 2 ----------------"
run_test "test2_label"




percentage_passed=$((passed_tests * 100 / total_tests))
echo "passed $percentage_passed% tests ($passed_tests/$total_tests)"