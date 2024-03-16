#!/bin/bash
#../../../../assembler input > expected.txt
passed_tests=0
total_tests=0

echo "================ preAssembler ================"

run_test() {
  local test_name="$1"

  cd "./$test_name" || exit
  ../../../assembler "input" >"tmp.txt"
  sed -r "s/\x1B\[[0-9;]*[mGK]//g" "tmp.txt" >"output.txt"

  diff_output=$(diff --strip-trailing-cr "output.txt" "expected.txt")

  unwanted_files=("am" "ob" "ent" "ext")
  unwanted_found=0
  for extension in "${unwanted_files[@]}"; do
    if [ -e "$test_name.$extension" ]; then
      echo "error: file $test_name.$extension was created"
      unwanted_found=1
    fi
  done
  if [ "$unwanted_found" -eq 0 ] && [ $? -eq 0 ]; then
    echo "$test_name: pass"
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
run_test "1_empty_mcr_declaration"

echo "---------------- test 2 ----------------"
run_test "2_mcr_start_with_non_alphabetic"

echo "---------------- test 3 ----------------"
run_test "3_mcr_saved_word"

echo "---------------- test 4 ----------------"
run_test "4_redeclaration_mcr"

echo "---------------- test 5 ----------------"
run_test "5_extra_txt_after_mcr"

echo "---------------- test 6 ----------------"
run_test "6_extra_txt_after_endmcr"

echo "---------------- test 7 ----------------"
run_test "7_unexpected_endmcr"

echo "---------------- test 8 ----------------"
run_test "8_no_endmcr_before_eof"

echo "---------------- test 9 ----------------"
run_test "9_nested_macro"


cd ../../../ || exit

percentage_passed=$((passed_tests * 100 / total_tests))
echo "passed $percentage_passed% tests ($passed_tests/$total_tests)"


# ../../../assembler "input" >"tmp.txt"
# ../../../assembler "input" >"tmp.txt"; sed -r "s/\x1B\[[0-9;]*[mGK]//g" "tmp.txt" >"expected.txt"; rm -f "tmp.txt"
# rm -f "tmp.txt"
