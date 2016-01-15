# Makefile

OBJS	= bison.o lex.o main.o value.o ast.o parseessentials.o vartype.o environment.o

GPP	= g++
GCC	= g++
CFLAGS	= -g -Wall -ansi -pedantic -std=c++11

all:	occ

occ:		$(OBJS)
		$(GCC) $(CFLAGS) $(OBJS) -o occ -lfl

value.o:	value.cpp value.h
		$(GPP) $(CFLAGS) -c value.cpp -o value.o

ast.o:		ast.cpp ast.h
		$(GPP) $(CFLAGS) -c ast.cpp -o ast.o

parseessentials.o:	parseessentials.cpp parseessentials.h
		$(GPP) $(CFLAGS) -c parseessentials.cpp -o parseessentials.o

vartype.o:		vartype.cpp vartype.h
		$(GPP) $(CFLAGS) -c vartype.cpp -o vartype.o

environment.o:		environment.cpp environment.h
		$(GPP) $(CFLAGS) -c environment.cpp -o environment.o

lex.o:		lex.c
		$(GPP) $(CFLAGS) -c lex.c -o lex.o

lex.c:		occ.lex 
		flex occ.lex
		cp lex.yy.c lex.c

bison.o:	bison.c
		$(GPP) $(CFLAGS) -c bison.c -o bison.o

bison.c:	occ.y
		bison -d -v --report=itemset --report=lookahead occ.y
		cp occ.tab.c bison.c
		cmp -s occ.tab.h tok.h || cp occ.tab.h tok.h

main.o:		main.cc
		$(GPP) $(CFLAGS) -c main.cc -o main.o

lex.o yac.o main.o	: heading.h
lex.o main.o		: tok.h

clean:
	rm -f *.o *~ lex.c lex.yy.c bison.c tok.h occ.tab.c occ.tab.h occ.output occ

