#MakeのCommandがgcc オブジェクトファイルなのかソースコードなのかよくわからなくなった。
libsub: sub.o sub.h
	$(CC) sub.c -shared -fPIC -o libsub.so

libdyn: dynamic.o 
	$(CC) dynamic.c -shared -fPIC -o libdyn.so

# dlopenがないっていわれたので、-lにdlを追加したらコンパイルが通った
dl: dl.o
	$(CC) dl.c -o dl -lsub -ldl -L./

clean: 
	rm -f libsub.so dl.o dl sub.o dynamic.o libdyn.so