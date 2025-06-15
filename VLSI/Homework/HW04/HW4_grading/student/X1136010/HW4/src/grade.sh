#!/bin/bash

cd ../..
mkdir -p HW4_grading/student/X1136010
tar -zcvf HW4_grading/student/X1136010/CS6135_HW4_X1136010.tar.gz HW4/

cd HW4_grading
bash HW4_grading.sh
