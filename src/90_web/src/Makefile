# Makefileのdebug用
# 変数の中身が表示されるとのこと
#$(warning MAKE = $(MAKE))
#$(warning CC = $(CC))
#$(warning CFLAGS = $(CFLAGS))
CFLAGS=-std=c11 -g -static 
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
web: $(OBJS)
			$(CC) -o web $(OBJS) $(LDFLAGS) 
$(OBJS): web.h
test: web
	./test.sh
clean:
	rm -f web *.o *~ tmp* $(LIBOBJECT)