%{
#include "AutoMath.h"
// Converted by hand to Lexical.c.
%}
%start Text Comment
%%
<Text>"var"			return 'v';
<Text>"def"			return 'd';
<Text>"prim"			return 'p';
<Text>"type"			yylval.SVal = FindId(yytext); return 't';
<Text>"prop"			yylval.SVal = FindId(yytext); return 't';
<Text>[a-zA-Z0-9_\b`']+		yylval.SVal = FindId(yytext); return 'x';
<Text>"/*"			ComDepth++; BEGIN(Comment);
<Text>[#%][^\n\r]*[\n\r]	Line++;
<Text>[#%][^\n\r]*\r\n		Line++;
<Text>"//"[^\n\r]*[\n\r]	Line++;
<Text>"//"[^\n\r]*\r\n		Line++;
<Text>[\n\r]			Line++;
<Text>\r\n			Line++;
<Text>[ \t]			;
<Text>.				return *yytext;
<Comment>"/*"			ComDepth++;
<Comment>"*/"			if (--ComDepth == 0) BEGIN(Text);
<Comment>[\n\r]			Line++;
<Comment>\r\n			Line++;
<Comment>.			;
.|[\n\r]			unput(*yytext); BEGIN(Text);
%%
