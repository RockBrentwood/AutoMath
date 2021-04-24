// @(#)yaccpar 1.9 (Berkeley) 1993 02 21
#define NoCh (-1)
#define EndCh 0

// %{
#include "AutoMath.h"
// Word classes: 'x': Identifier, 't': Kind ("type", "prop").
// Words: 'v' = "var", 'p' = "prim", 'd' = "def".
// %}
// %type <SVal> 'x' 't'
// %type <PVal> IDs
// %type <EVal> Ex, Var
// %type <LVal> Exs, Lam, Lams
// %type <IVal> N
#define ErrCh 0x100
#if 0
static const char *NameY[];
// Named tokens:
//   0 00 end-of-file
//  40 28 '('   41 29 ')'   43 2b '+'   44 2c ','   45 2d '-'   46 2e '.'   58 3a ':'   59 3b ';'
//  60 3c '<'   61 3d '='   62 3e '>'   64 40 '@'   91 5b '['   93 5d ']'  100 64 'd'  112 70 'p'
// 116 74 't'  118 76 'v'  120 78 'x'  123 7b '{'  125 7d '}'  126 7e '~'
const size_t TS = sizeof NameY/sizeof NameY[0] - 1;
#else
const size_t TS = 0x101;
#endif
// %%

#define RS 35
#define GS 12
#define QS 63
#define NS 214

typedef int IntY;
static const IntY HeadY[RS] = { // [0,RS) → [0,GS)
   -1,    0,    0,    8,    8,    8,    8,    8,    9,    8,   10,    8,   11,    8,    8,    7,
    7,    2,    2,    2,    2,    2,    2,    2,    6,    6,    5,    5,    4,    4,    3,    3,
    1,    1,    1
};
static const IntY SizeY[RS] = { // [0,RS) → [0,∞)
    2,    0,    2,    2,    2,    1,    1,    2,    0,    5,    0,    5,    0,    8,    1,    0,
    1,    1,    1,    3,    4,    3,    4,    4,    2,    4,    1,    2,    1,    3,    1,    4,
    0,    1,    3
};
static const IntY DefRY[QS] = { // [0,QS) → [0,RS)
    1,    0,   14,    0,    0,    0,    5,    6,    8,   10,   12,    0,    2,    0,    3,    4,
    0,    0,    0,    7,   33,    0,   17,    0,    0,    0,    0,    0,    0,   31,   26,    0,
    0,    0,    0,    0,    0,    0,    0,   34,   24,    0,    0,   27,    9,   21,    0,    0,
   19,    0,   11,    0,   23,    0,   22,    0,   20,   16,    0,   25,    0,    0,   13
};
static const IntY DefGY[GS] = { // [0,GS) → [0,QS)
    1,   21,   46,   25,   47,   32,   33,   58,   12,   16,   17,   18
};
static const IntY BegQY[QS] = { // [0,QS) →
    0,  -43,    0,  -91,  -81,  -74,    0,    0,    0,    0,    0,  -12,    0,  -70,    0,    0,
  -65,  -65,  -65,    0,    0,  -38,    0,  -46,  -53,   20,  -47,  -44,  -56,    0,    0,  -42,
  -65,  -14,   12,  -48,  -30,   22,   23,    0,    0,  -45,  -65,    0,    0,    0,   28,  -27,
    0,   21,    0,  -34,    0,  -37,    0,  -65,    0,    0,  -65,    0,   28,  -22,    0
};
static const IntY BegRY[QS] = { // [0,QS) →
    0,    0,    0,  -35,    0,    0,    0,    0,    0,    0,    0,    0,    0,  -36,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -40,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,  -13,    0,
    0,    0,    0,  -57,    0,    0,    0,    0,    0,    0,    0,    0,   -8,    0,    0
};
static const IntY BegGY[GS] = { // [0,GS) →
    0,    0,   24,   90,   57,    0,    0,    0,    0,    0,    0,    0
};
static const IntY TableY[NS] = { // [0,NS) → [0,QS)
    4,   18,    5,    6,   18,   30,   30,   35,   28,   30,   32,   48,   30,   35,   45,   35,
   35,   55,   35,   18,   18,    7,   18,   35,   30,   30,   23,   30,   28,   30,   42,   28,
   13,   29,   15,   54,   29,   62,   35,   14,   24,   26,   27,   23,   43,   23,   15,   31,
   52,   28,   20,   22,   19,   18,   29,    3,   41,   10,   30,   15,   36,   23,   56,   15,
   39,   55,   53,   34,   22,    9,   22,   44,    3,   37,    3,    8,   38,    3,   40,   60,
   18,   50,   61,   59,   51,   30,   22,   29,   35,   32,    3,   11,   57,   49,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    2
};
static const IntY CheckY[NS] = { // [0,NS) → [-1,TS)
  '+',  ')',  '-',  '.',  ',',  '(',  ')',  '<',  '.',  ',',  '.',  ')',  ':',  '<',  '>',  '<',
  '<',  ',',  '<',  ';',  '<',  '@',  '>',  '<',  ';',  '<',  '[',  '>',  ')',  '@',  ',',  ',',
  '{',  ')',  '[',  '>',  ',',  ';',  '<',  'x',   16,   17,   18,  '[',  ':',  '[',  'x',   23,
  ']',  '>',  'x',  't',  '@',  ']',  '>',  'x',   32,  'd',  ']',  't',  '(',  '[',  ')',  'x',
  'x',  ',',   42,  'x',  't',  'p',  't',  ';',  'x',  'x',  'x',  'v',  'x',  'x',  'x',   55,
  'x',  ';',  ':',  'x',  '=',  'x',  't',  '}',  '<',  '}',  'x',    1,  '~',   36,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,ErrCh
};

#define FinalQ 1
#if 0
static const char *RuleY[QS]; // [0,QS) → char *
#endif

// Define the initial stack-size.
#define InitQ 500
#if !defined MaxQ
#   define MaxQ InitQ
#endif

// Variables for the parser stack.
unsigned TopQ;
IntY *BegQ, *CurQ, *EndQ;
YYSTYPE *BegX, *CurX;

#define YYPURE 0

YYSTYPE CurV, yylval;

#include <stdlib.h>	// For malloc(), etc.
#include <string.h>	// For memset().

// Allocate the initial stack or double the stack size, up to MaxQ.
static bool GrowStack(void) {
   if (TopQ == 0) TopQ = InitQ;
   else if (TopQ >= MaxQ) return false;
   else if ((TopQ *= 2) > MaxQ) TopQ = MaxQ;
   int Q = (int)(CurQ - BegQ);
   BegQ = realloc(BegQ, TopQ*sizeof *BegQ); if (BegQ == NULL) return false;
   CurQ = BegQ + Q, EndQ = BegQ + TopQ - 1;
   BegX = realloc(BegX, TopQ*sizeof *BegX); if (BegX == NULL) return false;
   CurX = BegX + Q;
   return true;
}

#define AbortY goto ReturnY
#define AcceptY do { Status = false; goto ReturnY; } while (false)
#define StackY do { yyerror("yacc stack overflow"); AbortY; } while (false)

bool yyparse(void) {
   bool Status = true;
   int M, N, R, G, Ch;
   int ErrN = 0;
#if YYPURE
   TopQ = 0, EndQ = CurQ = BegQ = NULL, CurX = BegX = NULL;
#endif
   if (BegQ == NULL && !GrowStack()) StackY; else CurQ = BegQ, CurX = BegX;
   int Q = *CurQ = 0;
NextInput:
   Ch = NoCh;
NextAction:
   if ((R = DefRY[Q]) != 0) goto Reduce;
   if (Ch < 0 && (Ch = yylex()) < 0) Ch = EndCh;
   if ((N = BegQY[Q]) != 0 && (N += Ch) >= 0 && N < NS && CheckY[N] == Ch) {
      if (CurQ >= EndQ && !GrowStack()) StackY; else Q = *++CurQ = TableY[N], *++CurX = yylval;
      if (ErrN > 0) ErrN--;
      goto NextInput;
   } else if ((N = BegRY[Q]) != 0 && (N += Ch) >= 0 && N < NS && CheckY[N] == Ch) {
      R = TableY[N]; goto Reduce;
   }
   if (ErrN == 0) yyerror("syntax error");
   if (ErrN < 3) {
      ErrN = 3;
      while (true) {
         if ((N = BegQY[*CurQ]) != 0 && (N += ErrCh) >= 0 && N < NS && CheckY[N] == ErrCh) {
            if (CurQ >= EndQ && !GrowStack()) StackY; else Q = *++CurQ = TableY[N], *++CurX = yylval;
            goto NextAction;
         } else {
            if (CurQ <= BegQ) AbortY;
            CurQ--, CurX--;
         }
      }
   } else if (Ch == EndCh) AbortY;
   else goto NextInput;
Reduce:
   M = SizeY[R];
   if (M != 0) CurV = CurX[1 - M]; else memset(&CurV, 0, sizeof CurV);
   switch (R) {
   // $accept: Sts;
   // case 0: break;
   // Sts: | Sts St { Yield(); };
   // case 1: break;
      case 2: Yield(); break;
   // St: '+' 'x' { OpenPar($2); };
      case 3: OpenPar(CurX[0].SVal); break;
   // St: '-' 'x' { ClosePar($2); };
      case 4: ClosePar(CurX[0].SVal); break;
   // St: '.' { ClosePar((char *)NULL); };
      case 5: ClosePar((char *)NULL); break;
   // St: '@' { SetCon((Ex)NULL); };
      case 6: SetCon((Ex)NULL); break;
   // St: Var '@' { SetCon($1); };
      case 7: SetCon(CurX[-1].EVal); break;
   // $$1: { BegCon(); };
   // St: 'v' $$1 Ex 'x' ';' { NewCon($4, $3); };
      case 8: BegCon(); break;
      case 9: NewCon(CurX[-1].SVal, CurX[-2].EVal); break;
   // $$2: { BegCon(); };
   // St: 'p' $$2 Ex 'x' ';' { NewDef($4, (Ex)NULL, $3, false); };
      case 10: BegCon(); break;
      case 11: NewDef(CurX[-1].SVal, (Ex)NULL, CurX[-2].EVal, false); break;
   // $$3: { BegCon(); };
   // St: 'd' $$3 Ex 'x' '=' N Ex ';' { NewDef($4, $7, $3, $6); };
      case 12: BegCon(); break;
      case 13: NewDef(CurX[-4].SVal, CurX[-1].EVal, CurX[-5].EVal, CurX[-2].IVal); break;
   // St: error { Recover(); };
      case 14: Recover(); break;
   // N: { $$ = false; } | '~' { $$ = !DoF; };
      case 15: CurV.IVal = false; break;
      case 16: CurV.IVal = !DoF; break;
   // Ex: 't' { $$ = OneEx($1); };
      case 17: CurV.EVal = OneEx(CurX[0].SVal); break;
   // Ex: Var { $$ = Call($1, (List)NULL, false); };
      case 18: CurV.EVal = Call(CurX[0].EVal, (List)NULL, false); break;
   // Ex: Var '(' ')' { $$ = Call($1, (List)NULL, true); };
      case 19: CurV.EVal = Call(CurX[-2].EVal, (List)NULL, true); break;
   // Ex: Var '(' Exs ')' { $$ = Call($1, $3, true); };
      case 20: CurV.EVal = Call(CurX[-3].EVal, CurX[-1].LVal, true); break;
   // Ex: Ex '<' '>' { $$ = Applies($1, (List)NULL); };
      case 21: CurV.EVal = Applies(CurX[-2].EVal, (List)NULL); break;
   // Ex: Ex '<' Exs '>' { $$ = Applies($1, $3); };
      case 22: CurV.EVal = Applies(CurX[-3].EVal, CurX[-1].LVal); break;
   // Ex: '[' Lam Ex ']' { $$ = CloseAbses($2, $3); };
      case 23: CurV.EVal = CloseAbses(CurX[-2].LVal, CurX[-1].EVal); break;
   // Lams: Ex 'x' { $$ = AddAbs((List)NULL, $2, $1); };
      case 24: CurV.LVal = AddAbs((List)NULL, CurX[0].SVal, CurX[-1].EVal); break;
   // Lams: Lams ',' Ex 'x' { $$ = AddAbs($1, $4, $3); };
      case 25: CurV.LVal = AddAbs(CurX[-3].LVal, CurX[0].SVal, CurX[-1].EVal); break;
   // Lam: ':' { $$ = (List)NULL; };
      case 26: CurV.LVal = (List)NULL; break;
   // Lam: Lams ':' { $$ = $1; };
      case 27: CurV.LVal = CurX[-1].LVal; break;
   // Exs: Ex { $$ = AddList((List)NULL, $1); } | Exs ',' Ex { $$ = AddList($1, $3); };
      case 28: CurV.LVal = AddList((List)NULL, CurX[0].EVal); break;
      case 29: CurV.LVal = AddList(CurX[-2].LVal, CurX[0].EVal); break;
   // Var: 'x' { $$ = FindSym($1); };
      case 30: CurV.EVal = FindSym(CurX[0].SVal); break;
   // Var: 'x' '{' IDs '}' { $$ = FindEx($3, $1); };
      case 31: CurV.EVal = FindEx(CurX[-1].PVal, CurX[-3].SVal); break;
   // IDs: { $$ = CurPar; };
      case 32: CurV.PVal = CurPar; break;
   // IDs: 'x' { $$ = SupPar($1); };
      case 33: CurV.PVal = SupPar(CurX[0].SVal); break;
   // IDs: IDs '.' 'x' { $$ = SubPar($1, $3); };
      case 34: CurV.PVal = SubPar(CurX[-2].PVal, CurX[0].SVal); break;
   }
   CurQ -= M, CurX -= M;
   Q = *CurQ, G = HeadY[R];
   if (Q == 0 && G == 0) {
      Q = *++CurQ = FinalQ, *++CurX = CurV;
      if (Ch < 0 && (Ch = yylex()) < 0 || Ch == EndCh) AcceptY;
   } else {
      Q = (N = BegGY[G]) != 0 && (N += Q) >= 0 && N < NS && CheckY[N] == Q? TableY[N]: DefGY[G];
      if (CurQ >= EndQ && !GrowStack()) StackY; else *++CurQ = (IntY)Q, *++CurX = CurV;
   }
   goto NextAction;
ReturnY:
#if YYPURE || defined YY_NO_LEAKS
   TopQ = 0, free(BegQ), EndQ = CurQ = BegQ = NULL, free(BegX), CurX = BegX = NULL;
#endif
   return Status;
}
// %%
