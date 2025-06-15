#!/bin/bash

cd ../..
mkdir -p HW5_grading/student/X1136010
tar -zcvf HW5_grading/student/X1136010/CS6135_HW5_X1136010.tar.gz HW5/

cd HW5_grading
bash HW5_grading.sh
