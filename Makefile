all:
		clang -lpng -o encode encode.c
		clang -lpng -o decode decode.c

clean:
	rm encode decode
