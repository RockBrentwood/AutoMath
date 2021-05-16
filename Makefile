## The compile command and flags (the local compiler CParser will also be released for distribution later).
CC=gcc
CFLAGS=-O -g -std=gnu99
#CC=CParser
#CFLAGS=-O -g
## The shell command.
SH=/bin/sh
## The link command (use "cp" if the host does not support symbolic linking).
#LN=cp
LN=ln -s -f
## The file extensions
O=.o
X=
OBJS=Syntax$O Lexical$O Allocate$O Id$O AutoMath$O Par$O St$O Value$O Ex$O Print$O Type$O Eval$O Check$O Excerpt$O
BINDIR=${HOME}/bin
PROGRAM=./aut$X

${PROGRAM}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${PROGRAM}
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
#LEX=flex -Cf
##Lexical.c: Lexical.l
##	${LEX} Lexical.l
##	mv lex.yy.c Lexical.c
Lexical.c: Lexical0.c
	${LN} Lexical0.c Lexical.c
## The parser has been replaced by a manually programmed one.
#YACC=yacc
##Syntax.c: Syntax.y
##	${YACC} Syntax.y
##	mv y.tab.c Syntax.c
Syntax.c: Syntax0.c
	${LN} Syntax0.c Syntax.c

test: ${PROGRAM} GrundLagen.aut GrundLagenPn.aut
	${SH} Test.sh
install: ${PROGRAM}
	install -c -s ${PROGRAM} ${BINDIR}
small:
#	rm -f y.tab.c y.tab.h y.output
	rm -f *$O core
clean: small
	rm -f ${PROGRAM}
	rm -f Lexical.c
	rm -f Syntax.c
