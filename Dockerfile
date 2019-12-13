FROM centos:7.6.1810
RUN yum -y update && yum clean all
RUN yum install -y \
## コンパイラ
gcc \ 
## ビルドツール
make \
## リンカとかを含むさまざまなツール
binutils \
## Cのライブラリ？ centosにはlibc6-devがありそうだけど
## 何に使ってるかわからないので、一旦外してエラーがでたら追加してみよう
#libc-dev \
## デバッカ
gdb