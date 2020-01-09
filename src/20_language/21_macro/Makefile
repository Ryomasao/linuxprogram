
SRCS=main.c
OBJS=$(SRCS:.c=.o)

# makeのtargetって.cにするべきと思ってたんだけど、
# オブジェクトファイルを指定するのが正しいっぽい。

# 実行可能ファイルmainは、main.oに依存している。
# main.oはmain.cから作られるはず←このへんmakeが判断してくれるとのこと。
main: $(OBJS)
	$(CC) $(OBJS) -o main

# プリプロセスだけ実行する
pre: $(SRCS)
	$(CC) -E $(SRCS) > main.E.c

clean:
	rm -f main main.E.c *.o 