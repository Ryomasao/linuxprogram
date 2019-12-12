# 本家

https://github.com/aamine/stdlinux2-source/blob/master/httpd.c

## define \_GNU_SOURCE について

Docker で CentOS7 のイメージでコンパイルを行うと、シグナルの構造体を扱う`sigaction`あたりで、そんな名前しらないよとエラーがでてしまった。

```sh
web.c: In function 'trap_signal':
web.c:89:20: error: storage size of 'act' isn't known
   struct sigaction act;
```

`man sigaction`としてみると構造体の定義は、`signal.h`に記載されていることがわかる。

コンパイルする際に gcc が参照するデフォルトのヘッダファイルの情報は`cpp -b`で参照することができる。
`cpp`はコンパイルする前のプリプロセッサ！

```sh
$ cpp -v
Using built-in specs.
COLLECT_GCC=cpp
Target: x86_64-redhat-linux
# 省略
#include <...> search starts here:
 /usr/lib/gcc/x86_64-redhat-linux/4.8.5/include
 /usr/local/include
 /usr/include
End of search list.
```
