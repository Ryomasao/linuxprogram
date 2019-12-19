#include <stdio.h>
#include <stdlib.h>

//　実践Cプログラムの無限配列を実装する
// バケツの中に配列があって、配列に収まりきらない場合、次のバケツの配列へ格納する機能。

#define BLOCK_SIZE 10

// ひとつのバケツ
typedef struct bucket {
  // 配列に格納するデータ
  int *data;
  // 次のバケツへのリンク
  struct bucket *next;

} bucket;

// バケツの設定を行う
void ah_init_bucket();
// index指定箇所にvalueを格納する
void ah_store_item(int index, int value);
// indexに格納されている値を取得する
int ah_get_item(int index);
// バケツの中身をすべて表示する
void ah_print_bucket();

// indexから、どのバケツにいるのかを判断し、対象のバケツへのポインタを返却する
static bucket *ah_locate(int index);
static bucket *create_bucket();
