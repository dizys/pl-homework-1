#!/bin/sh

#####################################
#  Translate TinyBasic EBNF to BNF  #
#                                   #
#      by Ziyang Zeng (zz2960)      #
#             Feb. 2021             #
#####################################

net_id="<NET_ID>"

shell_dir=$(dirname "$0")
source_dir="$shell_dir"
dist_dir="$shell_dir/${net_id}_dist"
dist_c_source_dir="$dist_dir/c_src"
dist_binary_dir="$dist_dir/bin"

# Clean artifacts

if [ -d "$dist_dir" ] ; then
  echo "Cleaning previous build artifacts..."
  rm -f "$dist_c_source_dir/$net_id.tinybasic.*"
  rm -f "$dist_dir/tinybasic"
  echo " - Finished."
  echo ""
fi

echo "Translating \`tinybasic.in\` from EBNF to BNF..."
cat $source_dir/$net_id.tinybasic.in | $dist_binary_dir/trans > $source_dir/$net_id.tinybasic.y
if [ ! -f "$source_dir/$net_id.tinybasic.y" ] ; then
  echo -e "\033[31;1mError: failed to translate TinyBasic grammar from EBNF to BNF.\033[0m"
  exit
fi
echo " - Success."
echo ""

echo "Bison: generating TinyBasic parser c source file..."
bison -d "$source_dir/$net_id.tinybasic.y" -o "$dist_c_source_dir/$net_id.tinybasic.tab.c"
if [ ! -f "$dist_c_source_dir/$net_id.tinybasic.tab.c" ] ; then
  echo -e "\033[31;1mError: failed to generate parser using Bison.\033[0m"
  exit
fi
echo " - Success. Tokenizor source at: $dist_c_source_dir/$net_id.tinybasic.tab.c"
echo ""

echo "Flex: generating TinyBasic tokenizer c source file..."
flex -o"$dist_c_source_dir/$net_id.tinybasic.yy.c" "$source_dir/$net_id.tinybasic.l"
if [ ! -f "$dist_c_source_dir/$net_id.tinybasic.yy.c" ] ; then
  echo -e "\033[31;1mError: failed to generate tokenizer using Flex.\033[0m"
  exit
fi
echo " - Success. Parser source at: $dist_c_source_dir/$net_id.tinybasic.yy.c"
echo ""

echo "g++: compiling TinyBasic parser..."
g++ \
  -std=c++11 \
  -o "$dist_binary_dir/tinybasic" \
  "$dist_c_source_dir/$net_id.tinybasic.yy.c" \
  "$dist_c_source_dir/$net_id.tinybasic.tab.c"
if [ ! -f "$dist_binary_dir/tinybasic" ] ; then
  echo -e "\033[31;1mError: failed to compile TinyBasic parser.\033[0m"
  exit
fi
echo " - Success. Executable binary at: $dist_binary_dir/tinybasic"
echo ""

echo "TinyBasic build task done."