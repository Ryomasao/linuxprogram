#!/bin/bash

expect() {
  expected=$1
  actual=$2

  if [ "$actual" = "$expected" ]; then
    echo "OK"
  else
    echo "$expected exptected, but got $actual"
    exit 1
  fi
}

echo "リクエストが空っぽのときは、リターンコード20で終了する。"
cat /dev/null | ./web 2>/dev/null
expect 20  $?

echo "METHODを区切る最初のスペースがない場合、リターンコード21で終了する"
echo "NOFIRST_SPACE" |./web 2>/dev/null
expect 21  $?

echo "PATHを区切る最初の2つ目のスペースがない場合、リターンコード22で終了する"
echo "NOSECONDE SPACE" |./web 2>/dev/null
expect 22  $?

echo "HTTP/1.の文字列がない場合、リターンコード23で終了する"
echo "METHOD PATH HTTP/2" |./web 2>/dev/null
expect 23  $?

