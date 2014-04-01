LDFLAGS= 

all: cimplelang

clean: 
	rm -f cimplelang *.o parser.c *.tab.h *.tab.c lexer.c

LEX=flex
     
cimplelang: parser.o lexer.o cimplelang.o

lexer.o: lexer.c

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lexer.c: parser.tab.h lexer.l


test: cimplelang
	./cimplelang < test.sl
