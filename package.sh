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
scripts_dir="$shell_dir/scripts"
examples_dir="$shell_dir/examples"
dist_dir="$shell_dir/dist"
dist_package_dir="$dist_dir/package"

if [ ! -n "$1" ] ; then
  echo -e "\033[31;1mError: argument NET_ID cannot be empty.\033[0m"
  echo ""
  echo "Usage: ./package.sh NET_ID"
  echo "  e.g. \`./package.sh ab1234\`"
  exit 1
fi


# Clean artifacts

if [ -d "$dist_package_dir" ] ; then
  echo "Cleaning previous package artifacts..."
  rm -rf "$dist_package_dir"
  echo " - Finished."
  echo ""
fi


# Create package directory

if [ ! -d "$dist_package_dir" ] ; then
  mkdir -p "$dist_package_dir"
fi


# Copy files to package directory

echo "Copying \`README.md\`...";
cp "$scripts_dir/README.md" "$dist_package_dir/README.md"
sed -i "s/<NET_ID>/$1/g" "$dist_package_dir/README.md"
echo " - Finished."

echo "Copying \`trans.l\`...";
cp "$source_dir/trans.l" "$dist_package_dir/$1.trans.l"
sed -i "s/\"treenode.h\"/\"$1.treenode.h\"/g" "$dist_package_dir/$1.trans.l"
sed -i "s/\"trans.tab.h\"/\"$1.trans.tab.h\"/g" "$dist_package_dir/$1.trans.l"
echo " - Finished."

echo "Copying \`trans.y\`...";
cp "$source_dir/trans.y" "$dist_package_dir/$1.trans.y"
sed -i "s/\"treenode.h\"/\"$1.treenode.h\"/g" "$dist_package_dir/$1.trans.y"
echo " - Finished."

echo "Copying \`treenode.h\`...";
cp "$source_dir/treenode.h" "$dist_package_dir/$1.treenode.h"
echo " - Finished."

echo "Copying \`treenode.cpp\`...";
cp "$source_dir/treenode.cpp" "$dist_package_dir/$1.treenode.cpp"
sed -i "s/\"treenode.h\"/\"$1.treenode.h\"/g" "$dist_package_dir/$1.treenode.cpp"
echo " - Finished."

echo "Copying \`trans.sh\`...";
cp "$scripts_dir/trans.sh" "$dist_package_dir/$1.trans.sh"
sed -i "s/\"<NET_ID>\"/\"$1\"/g" "$dist_package_dir/$1.trans.sh"
echo " - Finished."

echo "Copying \`tinybasic.in\`...";
cp "$examples_dir/tinybasic.in" "$dist_package_dir/$1.tinybasic.in"
echo " - Finished."

echo "Copying \`tinybasic.l\`...";
cp "$examples_dir/tinybasic.l" "$dist_package_dir/$1.tinybasic.l"
sed -i "s/\"tinybasic.tab.h\"/\"$1.tinybasic.tab.h\"/g" "$dist_package_dir/$1.tinybasic.l"
echo " - Finished."

echo "Copying \`README.md\`...";
cp "$scripts_dir/README.md" "$dist_package_dir/README.md"
sed -i "s/<NET_ID>/$1/g" "$dist_package_dir/README.md"
echo " - Finished."

echo "Copying \`tinybasic.sh\`...";
cp "$scripts_dir/tinybasic.sh" "$dist_package_dir/$1.tinybasic.sh"
sed -i "s/\"<NET_ID>\"/\"$1\"/g" "$dist_package_dir/$1.tinybasic.sh"
echo " - Finished."


echo ""
echo "Package task done."
