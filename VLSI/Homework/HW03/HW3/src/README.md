# README
- Go to `bin` file and run command below
- `./hw3 input.txt output.out dead_space_ratio <wirelength_lamda_ratio> <penalty_alpha_ratio>`
    - ex : `./hw3 ../testcase/public1.txt ../output/public1.out 0.1`
    - `./hw3 ../testcase/public1.txt ../output/public1.out 0.1 0.5`
        - specify the wirelength contribute how much to cost
    - `./hw3 ../testcase/public1.txt ../output/public1.out 0.1 1 100`
        - specify both wirelength and penalty contribute how much to cost
- use `make grade` to use HW3_grading.sh to grade program