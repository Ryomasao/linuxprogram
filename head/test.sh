#!/bin/bash

try() {
  fileName="$1"
  make head
  ./head $fileName
}

echo "headコマンドを作成する"
try test.txt
