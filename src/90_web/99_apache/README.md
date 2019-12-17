# Apache のソースコードを読んでみるディレクトリ

読む対象のコードは、リポジトリの管理対象外にする。

公式
https://httpd.apache.org/download.cgi#apache24

ソースコードをするにあたり、ビルドしとくとよいとのことなのでビルド方法をメモっとく。
以前やったことがあることを思い出した。
https://github.com/Ryomasao/ansible/blob/master/vagrant/ansible/roles/role-apache/tasks/build.yml

## Apache をインストールする

インストール方法は以下を参照。
https://httpd.apache.org/docs/2.4/install.html

ソースコードを公式から取得する

```
$ wget http://ftp.meisei-u.ac.jp/mirror/apache/dist//httpd/httpd-2.4.41.tar.gz
```

ファイルを展開する

```
$ tar zxvf httpd-2.4.41.tar.gz
$ cd httpd-2.4.41
$ ./configure
```

`APR`が必要っていわれたのでインストールする。
APR は、Apache が OS に依存している部分を APR が間に入ることで、OS 間の差異を埋めてくれるものとのこと。

```
$ wget http://ftp.yz.yamagata-u.ac.jp/pub/network/apache//apr/apr-1.7.0.tar.gz
$ wget http://ftp.yz.yamagata-u.ac.jp/pub/network/apache//apr/apr-util-1.6.1.tar.gz
```

```
$ tar zxvf apr-1.7.0.tar.gz
$ tar zxvf apr-util-1.6.1.tar.gz
```

apr をインストールする。
結果`usr/local/apr`ディレクトリにインストールされた。

```
$ cd apr-1.7.0
$ ./configure
$ make
$ make install
```

apr-util をインストールする。
`./configure`時にオプションで apr のインストールされたディレクトリを指定するっぽい。

```sh
$ cd apr-util-1.6.1
$ ./configure --with-apr=/usr/local/apr
$ make
xml/apr_xml.c:35:19: fatal error: expat.h: No such file or directory
 #include <expat.h>
```

`expat.h`がないって怒られた。
https://blog.apar.jp/linux/7655/

`expat-devel`が必要とのことなので、インストールする。

```
$ yum install -y expat-devel
```

気をとり直して、もっかい。

```
$ make
$ make install
```

APR の導入が終わったので、再度 Apache 本体に移る。

```
$ cd httpd-2.4.41
```

`./configure`で Makefile を作る前に、Apache の MPM について簡単に。
Apache はリクエストを処理する際のモードに、`prefork`・`worker`・`event`の 3 つがある。
モードの詳細はここでは割愛して、切り替え方法についてここに書いとく。

各モードは、コンパイル時に`prefork`でいくぜ！と決め、apache 本体に`prefork`モードを含んでしまうもの(static module)と、
外に出して、実行時にモードを変更できるもの(DSO module)とがある。

どっちにするかは、`./configure`のオプションで決めれるっぽい。
https://httpd.apache.org/docs/2.4/en/mpm.html

apahce 本体にモードも含めた場合、モード切り替え時にリビルドする必要があるので、後者の DSO module としてコンパイルすることにする。

```
$ cd httpd-2.4.41
$ ./configure --enable-mpms-shared=all
$ make
$ make install
```

## 最低限の設定をする

```
$ cd /usr/local/apache2/conf
$ vi httpd.conf
```

```sh
# 待ち受けるport
Listen 8888
# ServerNameがないと実行時に怒られたので
ServerName localhost:8888
# MPMの使うモジュールをロードしておく(ひとまずprefork)
LoadModule mpm_prefork_module modules/mod_mpm_prefork.so
# MPMの設定ファイルを参照する
Include conf/extra/httpd-mpm.conf
```

## 動作確認

```
$ cd /usr/local/apache2/bin
```

`apachectl`は`httpd`をサービスとして使えるようにしてくれるもの？

```
$ ./apachectl -V
Server version: Apache/2.4.41 (Unix)
Server built:   Dec 16 2019 07:48:48
Server's Module Magic Number: 20120211:88
Server loaded:  APR 1.7.0, APR-UTIL 1.6.1
Compiled using: APR 1.7.0, APR-UTIL 1.6.1
Architecture:   64-bit
Server MPM:     prefork
  threaded:     no
    forked:     yes (variable process count)
Server compiled with....
 -D APR_HAS_SENDFILE
 -D APR_HAS_MMAP
 -D APR_HAVE_IPV6 (IPv4-mapped addresses enabled)
 -D APR_USE_PROC_PTHREAD_SERIALIZE
 -D APR_USE_PTHREAD_SERIALIZE
 -D SINGLE_LISTEN_UNSERIALIZED_ACCEPT
 -D APR_HAS_OTHER_CHILD
 -D AP_HAVE_RELIABLE_PIPED_LOGS
 -D DYNAMIC_MODULE_LIMIT=256
 -D HTTPD_ROOT="/usr/local/apache2"
 -D SUEXEC_BIN="/usr/local/apache2/bin/suexec"
 -D DEFAULT_PIDLOG="logs/httpd.pid"
 -D DEFAULT_SCOREBOARD="logs/apache_runtime_status"
 -D DEFAULT_ERRORLOG="logs/error_log"
 -D AP_TYPES_CONFIG_FILE="conf/mime.types"
 -D SERVER_CONFIG_FILE="conf/httpd.conf"
```

デバックしたいだけなので、直接 httpd を起動する。

```
$ ./httpd
$ ps -ef|grep httpd
root     70679     0  0 03:26 ?        00:00:00 ./httpd
daemon   70680 70679  0 03:26 ?        00:00:00 ./httpd
```

`fork`された子プロセス含め起動していることが確認できた。

リクエストを飛ばすと、動いていることが確認できた。

```
$ curl localhost:8888
<html><body><h1>It works!</h1></body></html>
```

## デバックする

`gdb`で以下を実行すると、debuginfo-install のメッセージがでてきたので指示に従う。

```
$ gdb httpd
> run
```

結構サイズがでかい。

```
debuginfo-install expat-2.1.0-10.el7_3.x86_64 glibc-2.17-292.el7.x86_64 nss-softokn-freebl-3.44.0-5.el7.x86_64 pcre-8.32-17.el7.x86_64
```
