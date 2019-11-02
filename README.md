# ふつうの Linux プログラミング学習リポジトリ

Web サーバーをつくるところまでいきたい。

各章ごとに Dir をわけてる。
各章で学んだことは、その Dir 配下の README.md に記載することにする。

ここでは、忘れがちなコンパイル方法とか Docker とかの環境周りの設定を書いておく。

## Docker

ホスト環境で開発しても問題はないけど、C 実行環境の Docker コンテナだけ用意してる。

```
$ docker-compose run --rm main bash
```

## Make 基本

Makefile は以下の構文

`target`が`prereq`より古い場合、command が実行される

```
target: prereq1
  command
```
