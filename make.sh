#!/bin/sh

#################################
#  Flex and Bison build script  #
#                               #
#       by Ziyang Zeng          #
#         Feb. 2021             #
#################################


# Initialize paths

shell_path=$(dirname "$0")
shell_dir="$(basename $shell_path)"
source_dir="$shell_dir/src"
dist_dir="$shell_dir/dist"
dist_c_source_dir="$dist_dir/c_src"
dist_binary_dir="$dist_dir/bin"


# Clean artifacts

if [ -d "$dist_dir" ] ; then
    echo "Cleaning previous build artifacts..."
    rm -rf "$dist_dir"
    echo " - Finished."
    echo -e "\n"
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
echo " - Success. Tokenizor source at: $dist_c_source_dir/trans.tab.c"
echo -e "\n"

echo "Flex: generating tokenizer c source file..."
flex -o"$dist_c_source_dir/trans.yy.c" "$source_dir/trans.l"
echo " - Success. Parser source at: $dist_c_source_dir/trans.yy.c"
echo -e "\n"

echo "g++: compiling translator..."
g++ \
    -std=c++11 \
    -I "$source_dir" \
    -o "$dist_binary_dir/trans" \
    "$dist_c_source_dir/trans.yy.c" \
    "$dist_c_source_dir/trans.tab.c" \
    "$source_dir/treenode.cpp"
echo " - Success. Executable binary at: $dist_binary_dir/trans"
echo -e "\n"

echo "Build task finished."