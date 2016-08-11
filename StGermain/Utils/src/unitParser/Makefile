srcdir = .

CC = gcc
CFLAGS = -lm
CXX = g++
CXXFLAGS = 

LEX = /usr/bin/flex
LEXFLAGS = 
YACC = /usr/bin/bison
YACCFLAGS = -d

all: u

u.tab.c u.tab.h: u.y 
	$(YACC) $(YACCFLAGS) $<

lex.yy.c: u.l u.tab.h
	$(LEX) $(LEXFLAGS) $<

u: lex.yy.c u.tab.c
	$(CC) $(CFLAGS) lex.yy.c u.tab.c -o $@ 


clean-object:
	rm -f lex.yy.o u.tab.o

clean-lexer:
	rm -f lex.yy.c

clean-parser:
	rm -f u.tab.c u.tab.h

clean: clean-object clean-lexer clean-parser 
