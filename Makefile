CC=CParser
#CC=gcc
SH=/bin/sh
CFLAGS=-O -g
O=.o
X=
OBJS=Syntax$O Lexical$O Allocate$O Id$O AutoMath$O Par$O St$O Value$O Ex$O Print$O Type$O Eval$O Check$O Excerpt$O
BINDIR=$(HOME)/bin
PROGRAM=./aut$X

$(PROGRAM): ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o $(PROGRAM)
.c$O:
	${CC} ${CFLAGS} -c $<
Lexical$O: AutoMath.h
Syntax$O: AutoMath.h
Allocate$O: AutoMath.h
Id$O: AutoMath.h
AutoMath$O: AutoMath.h
Par$O: AutoMath.h
St$O: AutoMath.h
Value$O: AutoMath.h
Ex$O: AutoMath.h
Print$O: AutoMath.h
Type$O: AutoMath.h
Eval$O: AutoMath.h
Check$O: AutoMath.h
Excerpt$O: AutoMath.h

## The lexer has been replaced by a manually programmed one.
##LEX=flex -Cf
##Lexical.c: Lexical.l
##	${LEX} Lexical.l
##	mv lex.yy.c Lexical.c
## The parser has been replaced by a manually recoded one.
##YACC=yacc
##Syntax.c: Syntax.y
##	${YACC} Syntax.y
##	mv y.tab.c Syntax.c
Syntax.c: Syntax0.c
	cp Syntax0.c Syntax.c

test: $(PROGRAM) GrundLagen.aut GrundLagenPn.aut
	$(SH) Test.sh
install: $(PROGRAM)
	install -c -s $(PROGRAM) ${BINDIR}
small:
	rm -f y.tab.c y.tab.h y.output *$O core
clean: small
	rm -f $(PROGRAM)
#	rm -f Lexical.c ## This was from when Lexical.c was generated from Lexical.l.
#	rm -f Syntax.c Syntax.h ## This was from when Syntax.c and Syntax.h were generated from Syntax.y.
