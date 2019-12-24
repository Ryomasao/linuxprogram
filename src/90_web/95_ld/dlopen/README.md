# ライブラリのおさらい

https://www.ibm.com/developerworks/jp/linux/library/l-dynamic-libraries/index.html

大きく、Static と Shared がある。
Static は、コンパイル時にプログラムそのものにライブラリがリンクされる。
Shared は、コンパイル時にはリンクされないんだ。
興味深いのは Shared の場合、プロセス間でメモリ上に展開されたライブラリが共有されるということ。
そんで、Shared にはさらに、DynamicLink と DynamicLoad とがある。
DynamicLink は、プログラム実行時に、ライブラリをメモリ上に展開する方式で、DynamicLoad は、プログラム内で呼ぶライブラリを選択して、メモリ上に展開する方式。

## DyamicLink

`-shared`オプションをつけることで、Shared ライブラリとしてコンパイルされる。普通のオブジェクトファイルと何が違うんだろう。
ファイル名は lib をつける必要があるとのこと。拡張子も.so 固定なのかな。

```
$ gcc -shared sub.c -o libsub.so
```

OSX だと上記で問題なかったけでども、CentOS だと`-fPIC`のオプションが必要だった。

`-fPIC`オプションについては、ELF についてもうちょっと調べないと理解できなさそう。

http://0xcc.net/blog/archives/000107.html

```
$ gcc -fPIC -shared sub.c -o libsub.so
```

コンパイルするときは、`-l`オプションをつけてライブラリの名前を指定してあげる。また`-L`オプションでライブラリのディレクトリも指定する。

```
$ gcc dl.c -o dl -lsub -L./
```

これで実行すると、libsub.so なんてないよっていわれる。
コンパイル時とは別に、Shared ライブラリの場所を環境変数で指定する必要があるとのこと。

https://sleepy-yoshi.hatenablog.com/entry/20090510/p1

※ OSX だとこのエラーはでなかった。デフォルトカレントディレクトリも対象にしているのかしら。

```
$ ./dl
./dl: error while loading shared libraries: libsub.so: cannot open shared object file: No such file or directory
```

`ldd`コマンドで、参照している Shared オブジェクトとディレクトリがわかる。

```
$ldd dl
	linux-vdso.so.1 =>  (0x00007ffeb1ab4000)
	libsub.so => not found
	libc.so.6 => /lib64/libc.so.6 (0x00007fa1264ca000)
	/lib64/ld-linux-x86-64.so.2 (0x00007fa126898000)
```

カレントディレクトリを追加をライブラリのパスに追加することで実行することができた。

```
$export LD_LIBRARY_PATH=./
```

## DynamicLoad

http://itref.fc2web.com/unix/dlopen.html

DynamicLoad は`dlopen`を使うことで実現できる。

```c
int main() {
  // dynamic load
  void *handle;
  // http://itref.fc2web.com/unix/dlopen.html
  // 絶対パス名と相対パス名のどちらで指定しても構いません。
  // パス名ではなく、ファイル名だけを指定することもできます。ファイル名だけを指定した場合、環境変数LD_LIBRARY_PATHに設定されたディレクトリ群から指定された共有ライブラリを探します。
  handle = dlopen("libdyn.so", RTLD_LAZY);

  if(!handle) {
    printf("failed to dlopen\n");
    exit(-1);
  }

  void (*func)(int);
  func = dlsym(handle, "dynamic");

  if(!func) {
    printf("failed to dlsym\n");
    exit(-1);
  }

  func(1);

  return 0;
}
```

## メモ

```
$ readelf -l dl
```

`readelf`コマンドを使うと、オブジェクトファイルの内容が見れる。
正確に書くと、ELF (Executable and Linking Format)と呼ぶらしい。
これをメモリに展開すれば実行できる形式なのかな。データ領域とかそういうやつ。

リンカ、ローダーの勉強をすると少し理解が進みそう。
http://kozos.jp/documents/linker_kernelvm5.pdf
