#!/bin/sh
#
# Use:
#   ./build.sh [-t BuildType] [-a] [-d directory] [-n test_name] [-f]
#
# Opzioni:
#   -t <BuildType>   Default: Debug
#   -d <directory>   Recursive directory execution
#
# Esempi:
#   ./test.sh                          
#   ./test.sh -t Release
#   ./test.sh -d test/<directory>

set -e

BUILD_TYPE="Debug"
MODE="all"
DIRPATH=""

while getopts "t:d:" opt; do
  case "$opt" in
    t) BUILD_TYPE="$opt" ;;
    d) MODE="directory"; DIRPATH="$OPTARG" ;;
    *) echo "Option is not valid: '$opt' (OPTARG='$OPTARG')"; exit 1 ;;
  esac
done

echo "Build type  : $BUILD_TYPE"
echo "Mode        : $MODE"
echo "Directory   : $DIRPATH"

sleep 1 && echo "\t-3"
sleep 1 && echo "\t-2"
sleep 1 && echo "\t-1"
sleep 1

./build.sh $BUILD_TYPE

CTEST_ARGS="--output-on-failure --stop-on-failure"

case "$MODE" in
  all)
    echo "Executing all tests..."
    cd ../build/test
    ctest $CTEST_ARGS .
    ;;
  directory)
    echo "Executing tests in directory: $DIRPATH"
    cd "../build/$DIRPATH"
    ctest $CTEST_ARGS . 
    ;;
esac