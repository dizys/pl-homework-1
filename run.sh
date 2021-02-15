#!/bin/sh

#################################
#  Script for Running Examples  #
#                               #
#    by Ziyang Zeng (zz2960)    #
#          Feb. 2021            #
#################################

# Initialize paths

shell_dir=$(dirname "$0")
dist_dir="$shell_dir/dist"
dist_binary_dir="$dist_dir/bin"

# Check if the translator binary exists

if [ ! -f "$dist_binary_dir/trans" ] ; then
  echo -e "\033[31;1mError: cannot find binary for the translator. Please run the shell script \`make.sh\` to build the translator first.\033[0m"
  exit 1
fi

# Check the argument

if [ ! -f "$1" ] ; then
  echo -e "\033[31;1mError: argument EXAMPLE_PATH cannot be empty.\033[0m"
  echo ""
  echo "Usage: ./run.sh EXAMPLE_PATH"
  echo "  e.g. \`./run.sh ./examples/cal1.y\`"
  exit 1
fi

# Run the translator

cat $1 | $dist_binary_dir/trans
