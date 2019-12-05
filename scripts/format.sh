#! /bin/sh
cd "`dirname $0`/.."
find tests -type f -name '*.[hc]pp' | xargs clang-format -verbose -i
