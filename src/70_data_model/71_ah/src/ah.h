//　実践Cプログラムの無限配列を実装する
// バケツの中に配列があって、配列に収まりきらない場合、次のバケツの配列へ格納する機能。

#define BLOCK_SIZE 10

// ひとつのバケツ
typedef struct bucket {
  // 配列に格納するデータ
  int data[BLOCK_SIZE];
  // 次のバケツへのリンク
  bucket *next;

} bucket;

// indexから、どのバケツにいるのかを判断し、対象のバケツへのポインタを返却する
static bucket *ah_locate(bucket *bucket, int index);

// 最初のバケツを確保して返却する
bucket *init_bucket();

// 指定したindexにvalueを格納する
// 成功した場合0を、失敗した場合-1を返却する
int store_item(int value, int index);

// indexに格納されている値を取得して返却する
int get_item(int index);