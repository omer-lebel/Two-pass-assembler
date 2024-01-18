#!/bin/bash

passed_tests=0
total_tests=0

cd ./testing/testPre || exit

echo "================ preAssembler ================"

run_test() {
    local test_name="$1"

    cd "./$test_name" || exit
    ../../../preAssembler "input"
    diff_output=$(diff --strip-trailing-cr input.am expected.am)

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
    cd ..
}

error_test() {
    local test_name="$1"

    ../../../preAssembler "$test_name" output

    if [ $? -eq 1 ]; then
            echo "$test_name: pass"
            echo ""
            ((passed_tests++))
        else
            echo "$test_name: error"
            echo ""
            echo "$diff_output"
        fi

    ((total_tests++))
}



echo "---------------- test 1 ----------------"
run_test "test1_mcr"

echo "---------------- test 2 ----------------"
run_test "test2_comments"

echo "---------------- test 3 ----------------"
run_test "test3_emptyLine"


cd ./test4-6_validMcr || exit
echo "---------------- test 4 ----------------"

error_test "test4_mcr_noName"

echo "---------------- test 5 ----------------"
error_test "test5_mcr_savedWord"

echo "---------------- test 6 ----------------"
error_test "test6_mcr_dupName"

cd ..



percentage_passed=$((passed_tests * 100 / total_tests))
echo "passed $percentage_passed% tests ($passed_tests/$total_tests)"