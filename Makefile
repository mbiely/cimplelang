LDFLAGS=
CFLAGS=-g 

all: cimplelang

clean:
	rm -f cimplelang *.o *.tab.h *.tab.c lexer.c

LEX=flex

cimplelang: parser.tab.o lexer.o cimplelang.o ast.o inter.o

lexer.o: lexer.c

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lexer.c: parser.tab.h lexer.l


test: cimplelang
	./cimplelang < test.sl

fac: cimplelang
	./cimplelang < fac.sl
