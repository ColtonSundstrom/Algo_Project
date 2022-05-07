#!/bin/bash
prog_Name="basic.cpp"
max=15
input_Path="../Project/datapoints"
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
    ./a.out "${input_Path}/in${i}.txt" "${output_Path}/out${i}.txt"
  done
fi
