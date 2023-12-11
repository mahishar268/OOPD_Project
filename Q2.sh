#!/bin/bash

# Function to create files in a directory
create_files() {
  local size=$1
  local count=$2
  local directory=$3

  mkdir -p "$directory"
  for ((i = 1; i <= count; i++)); do
    dd if=/dev/zero of="$directory/file_$i" bs="$size" count=1 &>/dev/null
  done
}

# Case 1: 100 files of 1KB each
time create_files 1K 100 directory1

# Case 2: 100 files of 10KB each
time create_files 10K 100 directory2

# Case 3: 100 files of 10KB each, with subdirectories recursively up to 1000 files
create_recursive_files() {
  local size=$1
  local count=$2
  local directory=$3
  local depth=$4

  if [[ $depth -eq 0 ]]; then
    return
  fi

  mkdir -p "$directory"
  for ((i = 1; i <= count; i++)); do
    dd if=/dev/zero of="$directory/file_$i" bs="$size" count=1 &>/dev/null
  done

  for ((i = 1; i <= 10; i++)); do
    create_recursive_files "$size" "$count" "$directory/subdirectory_$i" "$((depth - 1))"
  done
}

# Measure time for the recursive case
time create_recursive_files 10K 100 directory3 10

# Count the total number of files
total_files=$(find directory3 -type f | wc -l)
echo "Total files created: $total_files"
