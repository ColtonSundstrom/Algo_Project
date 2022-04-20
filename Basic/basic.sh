#!/bin/bash
echo "Enter the programme name: "
read prog_Name
if [ -z "${prog_Name}" ]; then
  prog_Name="basic.cpp"
fi
echo "Enter the maximum number of input files: "
read max
if [ -z "${max}" ]; then
  max=5
fi
echo "Enter the input directory path relative to the current folder (exclude end slashes, and leave blank for default path): "
read input_Path
if [ -z "${input_Path}" ]; then
  input_Path="../Project/SampleTestCases"
fi
echo "Enter the output directory path relative to the current folder (exclude end slashes, and leave blank for default path): "
read output_Path
if [ -z "${output_Path}" ]; then
  output_Path="output"
fi
g++ -Werror $prog_Name
if [[ $? == 0 ]]; then
  if ls | grep "${output_Path}"; then
    echo "Output folder with path \'${output_Path}\' already exists, replacing!"
    rm -r "${output_Path}"
  fi
  mkdir "${output_Path}"
  for i in `seq 1 $max`
  do
    ./a.out "${input_Path}/input${i}.txt" "${output_Path}/output${i}.txt"
  done
fi
