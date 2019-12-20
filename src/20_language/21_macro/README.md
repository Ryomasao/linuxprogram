# マクロの基本

読んだもの
http://www.c-lang.org/detail/macro_basic.html

```c
#define APR_HOOK_REALLY_FIRST (-10)
#define MAX(a, b) (a) > (b) ? (a) : (b))

//apacheのマクロ
#define MODULE(foo) int foo##_module

int hoge_module;
MODULE(hoge) = 1;

int main() {

  // オブジェクト形式のマクロ。ただの置換。
  int a = APR_HOOK_REALLY_FIRST;

  // 関数形式のマクロ
  int b = MAX(1, 2);

  printf("hoge=%d", hoge_module);

  return 0;
}
```

## 検証

Makefile を見る

# あとで読む

https://qiita.com/satoru_takeuchi/items/3769a644f7113f2c8040
