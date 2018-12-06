#!/bin/sh
find src/ -iname "*.cpp" -exec clang-format60 -i {} \;
find src/ -iname "*.h" -exec clang-format60 -i {} \;
