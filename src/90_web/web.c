#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static void service(FILE *in, FILE *out, char *docroot);
static struct HTTPRequest* read_request(FILE *in);
static struct HTTPHeaderField* read_header_field(FILE *in);
static struct HTTPRequest* read_request(FILE *in);
static void read_request_line(struct HTTPRequest *req, FILE *in);
static void upcase(char *str);
static void* xmalloc(size_t sz);
static void log_exit(char *fmt, ...);

#define LINE_BUF_SIZE 4096

struct HTTPHeaderField {
    char *name;
    char *value;
    struct HTTPHeaderField *next;
};

struct HTTPRequest {
    int protocol_minor_version;
    char *method;
    char *path;
    struct HTTPHeaderField *header;
    char *body;
    long length;
};


int main(int argc, char *argv[])
{
    service(stdin, stdout, argv[1]);
    exit(0);
}


static void service(FILE *in, FILE *out, char *docroot)
{
    struct HTTPRequest *req;

    req = read_request(in);

    /*
    int c;
    while( (c = fgetc(in)) !=EOF ){
        printf("%d\n", c);
    }
    */
}

static struct HTTPRequest* read_request(FILE *in)
{
    struct HTTPRequest *req;

    req = xmalloc(sizeof(struct HTTPRequest));

    read_request_line(req, in);
}

static void read_request_line(struct HTTPRequest *req, FILE *in)
{
    char buf[LINE_BUF_SIZE];
    char *path;
    char *p;

    //fgetsは一文字も読み込むことなく、EOFを読み込んだ場合、NULLを返す。
    if(!fgets(buf, LINE_BUF_SIZE, in)){
        log_exit("no request line");
    }

    //bufから文字' 'を探す。あれば該当のアドレスを返却する。なければNULL
    p = strchr(buf, ' ');

    //GET /path version のGET直後のスペースがない場合
    if(!p) log_exit("parse error on request line(1):%s", buf);

    //GET直後の空白に終端文字列を設定してポインタを進める
    *p++ = '\0';

    req->method = xmalloc(p - buf);

    //*p++のところで終端文字手列を設定してるんで、stcpyでいいかんじになる
    strcpy(req->method, buf);

    //method名を大文字にする
    upcase(req->method);
    
}

static void upcase(char *str)
{
    char *p;

    //引数strを直接参照しないのはなんでだろう
    for(p = str; *p; p++){
        *p = (char)toupper((int)*p);  
        printf("%c\n", *p);
    }

}

static void*
xmalloc(size_t sz)
{
    void *p;

    p = malloc(sz);
    if (!p) log_exit("failed to allocate memory");
    return p;
}

static void
log_exit(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
    exit(1);
}