#include "ah.h"

int main() {
  FILE *fp;
  ah_init_bucket();
  ah_store_item(22, 100);
  printf("value:%d\n", ah_get_item(22));
  // ah_print_bucket();

  return 0;
}

bucket *bucket_head;

void ah_init_bucket() {
  bucket *bucket = create_bucket();
  bucket->next = NULL;
  bucket_head = bucket;
}

static bucket *create_bucket() {
  bucket *bucket = malloc(sizeof(bucket));
  bucket->data = calloc(1, sizeof(int) * BLOCK_SIZE);
  bucket->next = NULL;
  return bucket;
}

static bucket *ah_locate(int index) {
  // 対象のindexを格納しているバケツの位置を取得する
  int bucket_no = index / BLOCK_SIZE;

  bucket *current_bucket = bucket_head;
  bucket *prev_bucket;

  // 先頭のバケツから、バケツの位置までたどっていく
  for(int i; i < bucket_no; i++) {
    prev_bucket = current_bucket;
    current_bucket = current_bucket->next;
    // バケツをたどる途中で、バケツが存在していなかったら、バケツをつくる
    // その際、リンクが途切れないように、つなげていく
    if(!current_bucket) {
      current_bucket = create_bucket();
      prev_bucket->next = current_bucket;
    }
  }

  return current_bucket;
}

void ah_store_item(int index, int value) {
  bucket *bucket;
  int data_index = index / BLOCK_SIZE;

  bucket = ah_locate(index);
  bucket->data[data_index] = value;
}

int ah_get_item(int index) {
  bucket *bucket;
  int data_index = index / BLOCK_SIZE;
  bucket = ah_locate(index);
  return bucket->data[data_index];
}

void ah_print_bucket() {
  struct bucket *current_bucket = bucket_head;
  int bucket_no = 0;
  while(current_bucket) {
    for(int index = 0; index < BLOCK_SIZE; index++) {
      printf("bucketNo: %d, index: %d, value:%d \n", bucket_no, index, current_bucket->data[index]);
    }
    current_bucket = current_bucket->next;
    bucket_no++;
  }
};
