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

`httpd.conf`を git で管理するため、conf の場所を変更する。

```
$ cd /usr/local/apache2/conf
$ ln -s /data/src/90_web/99_apache/conf/httpd.conf httpd.conf
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

### Apache でモジュールを読み込む仕組みを探る

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

`invoke_cmd`を辿っていくと、`dlopen`にたどり着く。

dso.c はマクロがいっぱい書いてあるので、プロプロセッサだけ実行してみたほうがわかりやすい。

```c
APR_DECLARE(apr_status_t) apr_dso_load(apr_dso_handle_t **res_handle,
                                       const char *path, apr_pool_t *ctx)
{
    void *os_handle = dlopen((char *)path, RTLD_NOW | RTLD_GLOBAL);

    *res_handle = apr_pcalloc(ctx, sizeof(*res_handle));

    if(os_handle == NULL) {
        (*res_handle)->errormsg = dlerror();
        return APR_EDSOOPEN;
    }

    (*res_handle)->handle = (void*)os_handle;
    (*res_handle)->pool = ctx;
    (*res_handle)->errormsg = NULL;

    apr_pool_cleanup_register(ctx, *res_handle, dso_cleanup, apr_pool_cleanup_null);

    return APR_SUCCESS;
}
```

`dlopen`のソースコードみてみたいんだけど、ソースコードをどうみればいいのかがまだよくわかってない。
調べてわかた t ことは`dlopen`は man 3 ででてくるので、C 標準？ライブラリと思っていいはず。
システムコールを伴わずに呼び出しができるんだ。

以下に、動作の記事があったので、ソースコードが読めたらみてみよう。

https://www.ibm.com/developerworks/jp/linux/library/l-dynamic-libraries/index.html

以下にメモを書いた。
src/90_web/95_ld/dlopen/README.md

## Apache Hook について

各モジュールでは、以下のように module 構造体が定義されている。

```c
module AP_MODULE_DECLARE_DATA hello_world_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    hello_world_register_hooks  /* register hooks                      */
};
```

一番最後の register_hooks で、このモジュールがやる処理を書いていく。

`ap_hook_処理タイミング`の関数で、その処理タイミングで行う関数をセットする。
以下は、handler が処理されるタイミングで、`hello_world_handler`を呼ぶことになる。

```
static void hello_world_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(hello_world_handler, NULL, NULL, APR_HOOK_MIDDLE);
}
```

処理タイミングには、以下のようにいろんなタイミングがある。
http://ohgrkrs-blog.blogspot.com/2014/06/blog-post_16.html

登録された hook は Apache 側で、以下のように`ap_run_pre_config`のように実行される。
main.c L651

```c
  if(ap_run_pre_config(pconf, plog, ptemp) != OK) {
    ap_log_error(APLOG_MARK, APLOG_STARTUP | APLOG_ERR, 0, NULL,
                 APLOGNO(00013) "Pre-configuration failed");
    destroy_and_exit_process(process, 1);
  }
```

※ DSO の機能は、mod_so.c 内の処理で実行されてたけど、hook の仕組みではないと思う。

HOOK の実態は、`config.c`のグローバルで定義されている構造体。

```c
APR_HOOK_STRUCT(
           APR_HOOK_LINK(header_parser)
           APR_HOOK_LINK(pre_config)
           APR_HOOK_LINK(check_config)
           APR_HOOK_LINK(post_config)
           APR_HOOK_LINK(open_logs)
           APR_HOOK_LINK(child_init)
           APR_HOOK_LINK(handler)
           APR_HOOK_LINK(quick_handler)
           APR_HOOK_LINK(optional_fn_retrieve)
           APR_HOOK_LINK(test_config)
           APR_HOOK_LINK(open_htaccess)
)

// ↑のマクロを展開するとこんな感じ。
static struct {
  apr_array_header_t *link_header_parser;
  apr_array_header_t *link_pre_config;
  apr_array_header_t *link_check_config;
  apr_array_header_t *link_post_config;
  apr_array_header_t *link_open_logs;
  apr_array_header_t *link_child_init;
  apr_array_header_t *link_handler;
  apr_array_header_t *link_quick_handler;
  apr_array_header_t *link_optional_fn_retrieve;
  apr_array_header_t *link_test_config;
  apr_array_header_t *link_open_htaccess;
} _hooks;
```

こちらも`config.c`のマクロを展開したもの。以下のように\_hooks 構造体に、登録していることが確認できる。

```c
void ap_hook_pre_config(ap_HOOK_pre_config_t *pf, const char *const *aszPre,
                        const char *const *aszSucc, int nOrder) {
  ap_LINK_pre_config_t *pHook;
  if(!_hooks.link_pre_config) {
    _hooks.link_pre_config = apr_array_make(apr_hook_global_pool, 1, sizeof(ap_LINK_pre_config_t));
    apr_hook_sort_register("pre_config", &_hooks.link_pre_config);
  }
  pHook = apr_array_push(_hooks.link_pre_config);
  pHook->pFunc = pf;
  pHook->aszPredecessors = aszPre;
  pHook->aszSuccessors = aszSucc;
  pHook->nOrder = nOrder;
  pHook->szName = apr_hook_debug_current;
```

よくわかってないのが、同じ hook を 2 回実行していること。
コメントでちゃんと書いてあるね。

L619

```
  /* Note that we preflight the config file once
   * before reading it _again_ in the main loop.
   * This allows things, log files configuration
   * for example, to settle down.
   */
```

メインループでも config を見るんだけど、その前に一回読むよ。
これによって、ログ設定とかうまいこといくよ的な感じ？
どういうこっちゃ。

まじかと思うのが prefork の動き。
prefork は pre_config の hook で登録されているので、上記の 2 回 hook 実行される流れで、2 回 fork することにならないかなと思った。
prefork の動きをみると、2 回目に実行されたときに実際に fork するっていう処理になってる。

prefork.c L1263

```c
    /* sigh, want this only the second time around */
    if (retained->mpm->module_loads == 2) {
        if (!one_process && !foreground) {
            /* before we detach, setup crash handlers to log to errorlog */
            ap_fatal_signal_setup(ap_server_conf, pconf);
            rv = apr_proc_detach(no_detach ? APR_PROC_DETACH_FOREGROUND
```

### prefork をもう少し

以下で fork 処理を行なっている。
これが prefork 処理なのかなとおもったけど、httpd.conf で設定した数分 prefork する等の処理が見受けられない。
おそらく、デーモン化するための事前処理と捉えていいのだろうか？

prefork.c L 1262

```c
    if (retained->mpm->module_loads == 2) {
        if (!one_process && !foreground) {
            /* before we detach, setup crash handlers to log to errorlog */
            ap_fatal_signal_setup(ap_server_conf, pconf);
            rv = apr_proc_detach(no_detach ? APR_PROC_DETACH_FOREGROUND
```

fork したプロセスを gdb で追っかけるには、以下の設定を行う。

```sh
(gdb) set follow-fork-mode child
# preforkする箇所でbreakpoint
# 前述の通り1回目はただスルーして、2回目にフォーク処理が走る
(gdb) b prefork_pre_config
```

```c
static struct {
  apr_array_header_t *link_monitor;
  apr_array_header_t *link_drop_privileges;
  apr_array_header_t *link_mpm;
  apr_array_header_t *link_mpm_query;
  apr_array_header_t *link_mpm_register_timed_callback;
  apr_array_header_t *link_mpm_get_name;
  apr_array_header_t *link_end_generation;
  apr_array_header_t *link_child_status;
  apr_array_header_t *link_suspend_connection;
  apr_array_header_t *link_resume_connection;
} _hooks;
```

## CGI

`mod_cgi`と`mod_cgid`についてのパフォーマンス。
https://hb.matsumoto-r.jp/entry/2014/09/11/025533

### mod_cgi

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
LoadModule cgi_module modules/mod_cgi.so
# cgiを実行するようにする
```
