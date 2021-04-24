%{
#include "AutoMath.h"
// Word classes: 'x': Identifier, 't': Kind ("type", "prop").
// Words: 'v' = "var", 'p' = "prim", 'd' = "def".
%}
%type <SVal> 'x' 't'
%type <PVal> IDs
%type <EVal> Ex, Var
%type <LVal> Exs, Lam, Lams
%type <IVal> N
%%
Sts: | Sts St { Yield(); };

St: '+' 'x' { OpenPar($2); };
St: '-' 'x' { ClosePar($2); };
St: '.' { ClosePar((char *)NULL); };
St: '@' { SetCon((Ex)NULL); };
St: Var '@' { SetCon($1); };
St: 'v' { BegCon(); } Ex 'x' ';' { NewCon($4, $3); };
St: 'p' { BegCon(); } Ex 'x' ';' { NewDef($4, (Ex)NULL, $3, false); };
St: 'd' { BegCon(); } Ex 'x' '=' N Ex ';' { NewDef($4, $7, $3, $6); };
St: error { Recover(); };

N: { $$ = false; } | '~' { $$ = !DoF; };

Ex: 't' { $$ = OneEx($1); };
Ex: Var { $$ = Call($1, (List)NULL, false); };
Ex: Var '(' ')' { $$ = Call($1, (List)NULL, true); };
Ex: Var '(' Exs ')' { $$ = Call($1, $3, true); };
Ex: Ex '<' '>' { $$ = Applies($1, (List)NULL); };
Ex: Ex '<' Exs '>' { $$ = Applies($1, $3); };
Ex: '[' Lam Ex ']' { $$ = CloseAbses($2, $3); };

Lams: Ex 'x' { $$ = AddAbs((List)NULL, $2, $1); };
Lams: Lams ',' Ex 'x' { $$ = AddAbs($1, $4, $3); };
Lam: ':' { $$ = (List)NULL; };
Lam: Lams ':' { $$ = $1; };

Exs: Ex { $$ = AddList((List)NULL, $1); } | Exs ',' Ex { $$ = AddList($1, $3); };

Var: 'x' { $$ = FindSym($1); };
Var: 'x' '{' IDs '}' { $$ = FindEx($3, $1); };

IDs: { $$ = CurPar; };
IDs: 'x' { $$ = SupPar($1); };
IDs: IDs '.' 'x' { $$ = SubPar($1, $3); };

%%
