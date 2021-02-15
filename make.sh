#!/bin/sh

#################################
#  Flex and Bison build script  #
#                               #
#    by Ziyang Zeng (zz2960)    #
#           Feb. 2021           #
#################################


# Initialize paths

shell_dir=$(dirname "$0")
source_dir="$shell_dir/src"
dist_dir="$shell_dir/dist"
dist_c_source_dir="$dist_dir/c_src"
dist_binary_dir="$dist_dir/bin"


# Clean artifacts

if [ -d "$dist_dir" ] ; then
  echo "Cleaning previous build artifacts..."
  rm -rf "$dist_dir"
  echo " - Finished."
  echo ""
fi


# Setup and build

if [ ! -d "$dist_c_source_dir" ] ; then
  mkdir -p "$dist_c_source_dir"
fi

if [ ! -d "$dist_binary_dir" ] ; then
  mkdir -p "$dist_binary_dir"
fi

echo "Bison: generating parser c source file..."
bison -d "$source_dir/trans.y" -o "$dist_c_source_dir/trans.tab.c"
if [ ! -f "$dist_c_source_dir/trans.tab.c" ] ; then
  echo -e "\033[31;1mError: failed to generate parser using Bison.\033[0m"
  exit 1
fi
echo " - Success. Tokenizor source at: $dist_c_source_dir/trans.tab.c"
echo ""

echo "Flex: generating tokenizer c source file..."
flex -o"$dist_c_source_dir/trans.yy.c" "$source_dir/trans.l"
if [ ! -f "$dist_c_source_dir/trans.yy.c" ] ; then
  echo -e "\033[31;1mError: failed to generate tokenizer using Flex.\033[0m"
  exit 1
fi
echo " - Success. Parser source at: $dist_c_source_dir/trans.yy.c"
echo ""

echo "g++: compiling translator..."
g++ \
  -std=c++11 \
  -I "$source_dir" \
  -o "$dist_binary_dir/trans" \
  "$dist_c_source_dir/trans.yy.c" \
  "$dist_c_source_dir/trans.tab.c" \
  "$source_dir/treenode.cpp"
if [ ! -f "$dist_binary_dir/trans" ] ; then
  echo -e "\033[31;1mError: failed to compile translator.\033[0m"
  exit 1
fi
echo " - Success. Executable binary at: $dist_binary_dir/trans"
echo ""

echo "Build task done."
