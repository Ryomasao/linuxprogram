# head コマンドをつくる

## FILE 型

システムコールの`open`では、ストリームへのファイルディスクリプタは直接`int`で管理していた。一方、stdlib では、FILE 型でファイルディスクリプタを管理している。
FILE には stdio のバッファとかも含まれているっぽい。

FILE からファイルディスクリプタの整数値を取得する`fileno`関数だったり、その逆の`fdopen`なんてのもある。
これは、`fopen`だけではできない操作があるから、`open`してから`fdopen`するとのこと。
パーミッションを指定する場合とか、それにあたるみたい。

## fprintf

`printf`は標準出力のみ。`fprintf`は出力先を指定できる。バリデーションの結果であれば、エラー出力に出すべきなのね。

## fgetc と ASC

`fgtec`は、バイト単位でファイルディスクリプタのストリームからデータを取得する。返り値は、`int`。
`char`じゃないんだーとなるんだけど、EOF の場合に-1 を返却するから`int`型が返る。
なんとなく`int`型を`putchar`で標準出力に出力すると、`int`だから数字がそのままでちゃんじゃないかなと思うけど、バイナリで見れば ASC コードを出力してるだけだからね！

バイナリエディタでで文字列 aiueo のテキストファイルを見た図

```
  Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
00000000: 61 69 75 65 6F 0A                                  aiueo.
```

## ショートオプションとロングオプション

ex) ショートオプション `-n`
ex) ロングオプション `--line`

Linux の伝統でショートとロングでこんな書き方を受け入れるものが多い。
`--line=5`,`-n5`,`-n 5`, `--line 5`

`getopt`、`getopt_long`でオプションを解析するライブラリが用意されている。
Linux の GNUlib とそれ以外とで仕様が違ったりするみたい。
