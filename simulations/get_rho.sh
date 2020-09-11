#!/usr/bin/env bash

fileset="results/*.vec"
result_dir=$1
mkdir -p $1

for file in $fileset; do
  suffix=$(echo $file | cut -d'-' -f 2)
  suffix=$(echo $suffix | cut -d'.' -f 1)
  result_file=$1$"/r_"$suffix".txt"
  echo $result_file
  read index <<< $( awk -F $' ' '/rho/{print $2}' $file )
  awk -F $'\t' -v i=$index '{if($1 == i) print $3, $4}' $file > $result_file
done

# echo "Processing sample"

# rho_set=$1"/*"
# i=0
# n=()
# accum=()
# for file in $rho_set; do
#   read total <<< $(wc -l < $file)
#   n+=($total)
#   j=0
#   while IFS= read -r line
#   do
#     read observation <<< $(echo $line | awk -F $' ' '{print $2}')
#     if [[ $i == 0 ]]; then
#       accum+=($observation)
#     else
#       accum[j]=$(echo ${accum[j]} + $observation | bc)
#       ((j++))
#     fi
#   done <"$file"
#   ((i++))
# done

# echo "Computing average"

# avg=()
# k=0
# for accum in "${sample[@]}"
# do
#   avg+=$(echo ${sample[k]} / ${n[k]} | bc)
#   ((k++))
# done

# for val in "${avg[@]}"
# do
#   $(echo ${sample[k]} / ${n[k]} | bc) >> 
#   ((k++))
# done