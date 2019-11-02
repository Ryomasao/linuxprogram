# cat コマンドをつくる

## cat と stdio_cat の違い

cat は、Linux のシステムコール(`open`, `write`)を使ってる。
stdio_cat は stdio.h に用意されている C のライブラリを使っている。
