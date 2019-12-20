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

気を取り直して再度。

```
$ gdb httpd
# breakpointoをエントリーポイントに設定
> b main
> run
```

現在のプロセスの ID を表示する。

```
info proc
```

```
main.c
  config.c
    これがなんなのかわからない
    AP_IMPLEMENT_HOOK_RUN_ALL
```

## Apache モジュールを試しに作成する

hook 処理を理解するためにまずは使ってみよう。
https://blog.asial.co.jp/715

```sh
# モジュールを作るツールのapxsはperlでできてるっぽいので、perlをインストールする。
$ yum install -y perl
```

```sh
$ cd /usr/local/apache2/bin
$ vi apxs
```

apxs のシバンにインストールした perl のパスを指定する

```sh
#!/usr/bin/perl -w
```

```
$ ./apxs -g -n hello_world
```

作成された`hello_world`に移動して以下を実行すると、`mod_hello_world.so`が作成される。

```
$ make
$ make install
```

`httpd.conf`に以下を追加して、localhost/hello にアクセスすると、追加した`hello_world`モジュールが実行される。

```
LoadModule hello_world_module modules/mod_hello_world.so
<Location /hello>
  SetHandler hello_world
</Location>
```

## コードを読む

http://dev.ariel-networks.com/articles/webdb-vol35/webdb-vol35/

Apache のソースコードは、マクロが頻繁につかわれているので初見だときつい。
マクロをまずは展開させることにする。

プリコンパイルだけ行うことで、マクロが展開される。

```
$ cd server
$ rm main.o
$ make main.0
↑で出力された内容の-cを-Eに変更して標準出力に適当なファイル名を指定して実行。
```

### Apache モジュールを読み込む仕組みを探る

main.c L485

```c
  error = ap_setup_prelinked_modules(process);
```

ここで事前にリンク済みのモジュールの設定を行なっている。

config.c

```c
     *  Initialise total_modules variable and module indices
     */
    total_modules = 0;
    for (m = ap_preloaded_modules; *m != NULL; m++)
        (*m)->module_index = total_modules++;

    max_modules = total_modules + DYNAMIC_MODULE_LIMIT + 1;
    conf_vector_length = max_modules;
```

`ap_preloaded_modules`変数は、どこで設定してんだろ？と調べた。

modules.c で prelink するモジュールが定義されている。
グローバルスコープで定義されているので、main 関数実行より前に設定が終わっていることになる。

```c
module *ap_preloaded_modules[] = {
  &core_module,
  &so_module,
  &http_module,
  &mpm_event_module,
  NULL
};
```

`core_module`等はどこからきているのだろうか。
→`mpm_event_module`でいえば`event.c`の最下部に以下のマクロがある。

```c
AP_DECLARE_MODULE(mpm_event) = {
    MPM20_MODULE_STUFF,
    NULL,                       /* hook to run before apache parses args */
    NULL,                       /* create per-directory config structure */
    NULL,                       /* merge per-directory config structures */
    NULL,                       /* create per-server config structure */
    NULL,                       /* merge per-server config structures */
    event_cmds,                 /* command apr_table_t */
    event_hooks                 /* register_hooks */
};
```

これを展開すると、以下の設定が現れる。

```c
module mpm_event_module = {
    20120211, 88, -1, "event.c", ((void *)0), ((void *)0), 0x41503234UL,
    ((void *)0),
    ((void *)0),
    ((void *)0),
    ((void *)0),
    ((void *)0),
    event_cmds,
    event_hooks
};
```
