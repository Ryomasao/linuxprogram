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

# どういうときに発生するのかわかってない
echo "Skip socketのディスクリプターが作成？できないとき、リターンコード101で終了する。"
# ポートが使われている時とかに発生する
echo "Skip socketのbindができないとき、リターンコード101で終了する。"
# どういうときに発生するのかわかってない
echo "Skip socketのlistenができないとき、リターンコード101で終了する。"

echo "リクエストが空っぽのときは、リターンコード20で終了する。"
cat /dev/null | ./web 2>/dev/null
expect 20  $?

echo "リクエストラインにMETHODを区切る最初のスペースがない場合、リターンコード21で終了する"
echo "NOFIRST_SPACE" |./web 2>/dev/null
expect 21  $?

echo "リクエストラインにPATHを区切る最初の2つ目のスペースがない場合、リターンコード22で終了する"
echo "NOSECONDE SPACE" |./web 2>/dev/null
expect 22  $?

echo "リクエストラインにHTTP/1.の文字列がない場合、リターンコード23で終了する"
echo "METHOD PATH HTTP/2" |./web 2>/dev/null
expect 23  $?

echo "ヘッダーフィールドのContent-Lengthの値が0未満の場合、リターンコード24で終了する"
cat << EOS > sample.txt
METHOD PATH HTTP/1.1
Host: google.com
User-Agent: curl/7.54.0
Accept: */*
Content-Length: -1

EOS
./web < sample.txt
expect 24  $?

echo "Skip リクエストBodyのサイズが1024*1024を超える場合、リターンコード25で終了する"
echo "Skip リクエストでファイルがオープンできない場合、リターンコード26で終了する"
echo "Skip リクエストでファイルのREADできない場合、リターンコード27で終了する"
echo "Skip リクエストで標準出力にWRITEできない場合、リターンコード28で終了する"

echo "正しいリクエストヘッダーの場合、リターンコードは0、またNOT FOUNDのHTMLを返却する"
cat << EOS > sample.txt
GET /hoge HTTP/1.1
Host: google.com
User-Agent: curl/7.54.0
Accept: */*
Content-Length: 4

Body

EOS
./web < sample.txt
expect 0 $?

echo "GETリクエストでファイルが存在する場合、ファイルの内容を出力する"
cat << EOS > sample.txt
GET /hoge.txt HTTP/1.1
Host: google.com
User-Agent: curl/7.54.0
Accept: */*
Content-Length: 4

Body

EOS
./web < sample.txt
expect 0 $?

echo "HEADリクエストでファイルが存在する場合、ファイルの内容を出力しない"
cat << EOS > sample.txt
HEAD /hoge.txt HTTP/1.1
Host: google.com
User-Agent: curl/7.54.0
Accept: */*
Content-Length: 4

Body

EOS
./web < sample.txt
expect 0 $?

echo "HEADリクエストでファイルが存在する場合、ファイルの内容を出力しない"
cat << EOS > sample.txt
HEAD /hoge.txt HTTP/1.1
Host: google.com
User-Agent: curl/7.54.0
Accept: */*
Content-Length: 4

Body

EOS
./web < sample.txt
expect 0 $?



