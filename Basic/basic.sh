#!/bin/bash
prog_Name="basic.cpp"
max=5
input_Path="../Project/SampleTestCases"
output_Path="output"

g++ -Werror $prog_Name
if [[ $? == 0 ]]; then
  if ls | grep "${output_Path}"; then
    #echo "Output folder with path \'${output_Path}\' already exists, replacing!"
    rm -r "${output_Path}"
  fi
  mkdir "${output_Path}"
  for i in `seq 1 $max`
  do
    ./a.out "${input_Path}/input${i}.txt" "${output_Path}/output${i}.txt"
  done
fi
