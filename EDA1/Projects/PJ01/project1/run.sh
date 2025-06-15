#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: ./run.sh <studentID_name_proj1>"
    exit 1
fi

EXECUTABLE=$1

# Compile the program
make clean
make TARGET=$EXECUTABLE

# Check if compilation was successful
if [ ! -f "$EXECUTABLE" ]; then
    echo "Compilation failed."
    exit 1
fi

# Create output directory if it does not exist
mkdir -p output

# Run the program with each test case
for testcase in testcases/*.in; do
    case_name=$(basename "$testcase" .in)
    ./"$EXECUTABLE" "$testcase" "output/${case_name}.out"
    echo "Executed: ./$EXECUTABLE $testcase output/${case_name}.out"
done

echo "All test cases executed. Results are in the 'output' folder."
