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

一方、モジュールにリンクされていないものは以下の関数実行時に読み込まれていそう。
main.c L630

```c
  ap_server_conf = ap_read_config(process, ptemp, confname, &ap_conftree);
```

以下で、`httpd.conf`の内容をパースしてる。
config.c L2335

```c
    error = ap_process_resource_config(s, confname, conftree, p, ptemp);
```

process_command_config→ap_process_resource_config→ap_build_config

`httpd.conf`の内容を 1 行ずつ'vb'に読み込み、ap_build_config_sub で処理している。`ap_direcive_t`構造体にパースした値をつっこんでいくんだけど、その過程で、モジュールのダイナミックロードもやってる。

config.c L1422

```c
    while ((rc = ap_varbuf_cfg_getline(&vb, parms->config_file, max_len))
           == APR_SUCCESS) {
        errmsg = ap_build_config_sub(p, temp_pool, vb.buf, parms,
                                     &current, &curr_parent, conftree);

```

config.c L1160

```c

    if ((cmd = ap_find_command_in_modules(cmd_name, &mod)) != NULL) {
        newdir->directive = cmd->name;

        if (cmd->req_override & EXEC_ON_READ) {
            ap_directive_t *sub_tree = NULL;

            parms->err_directive = newdir;
            retval = execute_now(cmd_name, args, parms, p, temp_pool,
                                 &sub_tree, *curr_parent);
```

cmd には、httpd.conf でパースしたコマンドが設定される
以下の行を処理している最中であれば、以下の値が設定される。
LoadModule authn_file_module modules/mod_authn_file.so

cmd: LoadModule
args: authn_file_module modules/mod_authn_file.so

ap_find_command_in_modules は、LoadModule のようなコマンドが、既に登録されているモジュールに存在しているかを確認している。
LoadModule は Apache に事前に含まれる mod_so.c のコマンドとして登録されている。

以下の定義。
mod_so.c L426

```c
static const command_rec so_cmds[] = {
    AP_INIT_TAKE2("LoadModule", load_module, NULL, RSRC_CONF | EXEC_ON_READ,
      "a module name and the name of a shared object file to load it from"),
    AP_INIT_ITERATE("LoadFile", load_file, NULL, RSRC_CONF  | EXEC_ON_READ,
      "shared object file or library to load into the server at runtime"),
    { NULL }
};

```

そして、コマンドの種類が`EXEC_ON_READ`の場合、この設定ファイルを読む段階で、コマンドの実行を、`execute_now`関数で行う。

```c
static const char *execute_now(char *cmd_line, const char *args,
                               cmd_parms *parms,
                               apr_pool_t *p, apr_pool_t *ptemp,
                               ap_directive_t **sub_tree,
                               ap_directive_t *parent)
{
    const command_rec *cmd;
    ap_mod_list *ml;

    // cmd_lineは、LoadModule
    // apr_psrtdupはtemp->poolに文字列LoadModuleをつっこんでその参照をdirに返す
    char *dir = apr_pstrdup(parms->temp_pool, cmd_line);

    ap_str_tolower(dir);

    // これがあんまよくわかってない。dirに設定された文字列LoadModuleをkeyに、ハッシュテーブルを検索？してる
    // これにより、コマンドLoadModuleを持つmoduleのリストが返却されるっぽい
    // ここでは、 so_cmds ※mod_soのコマンドテーブルの名前が設定される
    ml = apr_hash_get(ap_config_hash, dir, APR_HASH_KEY_STRING);

    if (ml == NULL) {
        return apr_pstrcat(parms->pool, "Invalid command '",
                           cmd_line,
                           "', perhaps misspelled or defined by a module "
                           "not included in the server configuration",
                           NULL);
    }

    for ( ; ml != NULL; ml = ml->next) {
        const char *retval;
        cmd = ml->cmd;

        // ほんで、ここでそのコマンドが実行される
        retval = invoke_cmd(cmd, parms, sub_tree, args);

        if (retval != NULL) {
            return retval;
        }
    }

    return NULL;
}
```

`invoke_cmd`内でさきほどの定義に書かれている、mod_so.c の load_module 関数が呼ばれる。
mod_so.c L426

```c
static const command_rec so_cmds[] = {
    AP_INIT_TAKE2("LoadModule", load_module, NULL, RSRC_CONF | EXEC_ON_READ,
      "a module name and the name of a shared object file to load it from"),
};

```

### CGI

`mod_cgi`と`mod_cgid`についてのパフォーマンス。
https://hb.matsumoto-r.jp/entry/2014/09/11/025533

#### mod_cgi

`mod_cgi`をまずはみてみる。
prefork したプロセスが cgi を実行してくれる？
まずは cgi を有効にする。と思ったが、`modules`配下には`mod_cgid`しかなくって`mod_cgi`がない。
どうもデフォルトの設定では`mod_cgid`が用意されているっぽい。

Apache のソースコードに`mod_cgi.c`はいるけど、同じディレクトリの Makefile には、`mod_cgi`の定義が見当たらなかった。
なので、ビルドの設定をかえてみる。

```sh
# defaultは--enable-cgidになってるっぽ
./configure --enable-cgi
```

上記を実行してみてみたところ、Makefile に`mod_cgi`の定義が追加されていることが確認できた。
`generator`配下で make&make install することで、modules 配下に無事追加された。

続いて、設定を変更する。
http.conf

```
# モジュールを読み込んで
LoadModule cgid_module modules/mod_cgi.so
# cgiを実行するようにする
```
