# ふつうの Linux プログラミング学習リポジトリ

Web サーバーをつくるところまでいきたい。

各章ごとに Dir をわけてる。
各章で学んだことは、その Dir 配下の README.md に記載することにする。

ここでは、忘れがちなコンパイル方法とか Docker とかの環境周りの設定を書いておく。

## Docker

ホスト環境で開発しても問題はないけど、C 実行環境の Docker コンテナだけ用意してる。

main コンテナを起動して、bash を実行する(これによりコンテナに入ることができる)

```
$ docker-compose run --rm main bash
```

起動済みのコンテナに入る。

```
# コンテナIDを確認する
$ docker ps
# 起動中のコンテナで新しくbashプロセスを起動し、-itオプションでダミーの？制御端末の割り当てなどをしてる
$ docker exec -it コンテナID bash
```

docker-compose を使った方が楽。

```
$ docker-compose exec main bash
```

`docker-compose.yml`に書いたけど、centos の journald を起動するコマンドを実行したいので`docker-comoose up`を使った方がよさげ。

```
$ docker-compose up -d
$ docker-compose exec main bash
```

syslog の監視は、以下が `tail -f` っぽくてよい

```
$ journalctl -ef
```

プロセスが生み出される様子は、watch コマンドで ps を叩くのがよさげ

```
$ watch "ps -ef|grep web"
```

## Make 基本

Makefile は以下の構文

`target`が`prereq`より古い場合、command が実行される

```
target: prereq1
  command
```
