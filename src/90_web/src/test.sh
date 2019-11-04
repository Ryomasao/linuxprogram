#!/bin/bash

expect() {
  actual=$1
  expected=$2

  if [ "$actual" = "$expected" ]; then
    echo "OK"
  else
    echo "$expected exptected, but got $actual"
    exit 1
  fi
}

echo "リクエストが空っぽのときは、リターンコード20で終了する。"
cat /dev/null | ./web
expect 20  $?

