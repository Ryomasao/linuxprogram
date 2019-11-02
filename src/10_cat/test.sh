#!/bin/bash

try() {
  fileName="$1"
  make cat
  ./cat $fileName
}

echo "引数で渡されたファイルを標準出力に出力する"
try test.txt
