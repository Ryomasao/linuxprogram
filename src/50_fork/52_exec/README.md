# C 言語から Node.js を実行してみる

別のプログラムを実行するには、`exec`族と呼ばれる関数を使う。
`exec`族にはいろんな種類があるんだけど、`execve`が Linux のシステムコールでそれ以外は、それをラップしているライブラリ関数。
`execve`のすごいところは、実行すると引数で指定したプログラムを自プロセスに上書きして、実行するところ。
上書きされた処理は当然実行されない。
なので、このこれらの処理を行うときは、fork してから exec することが一般的っぽい。

早速使ってみる。

## execve

sample.js

```js
console.log("hello");
```

main.c

```c
void main() {
  char *argv[3] = {"node", "sample.js", NULL};
  execve("/usr/bin/node", argv, NULL);

  printf("execveが成功しているのであれば、このコードは実行されない\n");
}
```

`execve`の第 1 引数には、実行するプログラムのパスを書いて、第 2 引数にはプログラムに渡す引数を文字列の配列として渡す。
文字列の配列の最初の要素はプログラムの名前をセットする慣習があるので、`node`という文字列をセットした。
イメージ的に、以下のように C 言語を実行した場合の argv[0]には fuga ではなく、プログラム名の hoge がセットされているので、この慣習に合わせる必要があるんだと思う。

```
$ ./hoge fuga
```

第 3 引数は、プログラムを実行する際の環境変数を設定する。NULL の場合、現在の環境変数をそのまま使う。

これを実行すると、以下のように`sample.js`の実行結果が標準出力に表示された。

```
$ ./exec
hello
```

## fork で組み合わせる

以下のように fork と組み合わせることで子プロセスの場合に`execve`を実行することができる。

```c
void main() {
  pid_t pid;
  pid = fork();

  if(pid < 0) {
    fprintf(stderr, "fork(2) failed\n");
    exit(1);
  }

  if(pid == 0) {
    // 子プロセス
    char *argv[3] = {"hoge", "sample.js", NULL};
    execve("/usr/bin/node", argv, NULL);

    printf("execveが成功しているのであれば、このコードは実行されない\n");
  } else {
    int status;
    // 子プロセスの終了を待つ
    wait(&status);
  }
  exit(0);
}
```
