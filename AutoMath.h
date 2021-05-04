#include <stdio.h> // For the definition of FILE.

typedef enum { false = 0, true = 1 } bool;

enum KindT { OneK = 1, DefK, ConK, TermK, AppK, AbsK, VarK };
typedef struct Ex *Ex;		// Generic.
typedef struct One *One;	// Kind ≡ OneK.
typedef struct St *St;		// Kind ≡ DefK ∨ Kind ≡ ConK.
typedef struct Def *Def;	// Kind ≡ DefK.
typedef struct Con *Con;	// Kind ≡ ConK.
typedef struct Term *Term;	// Kind ≡ TermK.
typedef struct App *App;	// Kind ≡ AppK.
typedef struct Lam *Lam;	// Kind ≡ AbsK.
typedef struct Var *Var;	// Kind ≡ VarK.

// In-paragraph statement list:
// ∙	St∷Prev, St∷Next are the links.
// ∙	CurPar ≠ NULL: Par∷Beg, Par∷End are the list endpoints;
// ∙	CurPar ≡ NULL: [CurCover is the list end].
// Top-level statement list:
// ∙	FirstSt is the list head;
// ∙	St∷Forth is the list link;
// ∙	[CurSt is the list end].
// Context lists:
// ∙	Def∷Back is the context list;
// ∙	Con∷Back is the link;
// ∙	CurCon is the current context.
// Argument lists:
// ∙	Con∷Exs is the list tail;
// ∙	List∷Prev is the link;
// ∙	C->Con∷Exs->Arg is (Ex)C, itself.
// Definition list:
// ∙	Def∷Seq is the definition index;
// ∙	Later definitions are evaluated first, since they may depend on earlier definitions.
// Current context chain:
// ∙	CurCon is the current context.
// ∙	Par∷Sup is the link.
// Sub-context list:
// ∙	TopList is the list header for the top level.
// ∙	C->Par∷Sub is the list header for context C.
// ∙	Par∷Prev is the list link.
typedef struct Par *Par; // NULL means "cover".
struct Par { char *Id; Par Sup, Sub, Prev; St Beg, End; };
typedef struct List *List;
struct List { Ex Arg; List Prev; };

struct Ex {
   enum KindT Kind;
};
struct One {
   enum KindT Kind; char *Id;
};
struct St { // Generic statement.
   enum KindT Kind; char *Id; Par At; Ex Type; short Deg; Con Back; St Prev, Next, Forth; short Echo;
};
struct Con { // "var" statement.
   enum KindT Kind; char *Id; Par At; Ex Type; short Deg; Con Back; St Prev, Next, Forth; short Echo; Ex Ref; List Exs;
};
struct Def { // "prim" statement if Body == NULL, "def" statement if Body != NULL.
   enum KindT Kind; char *Id; Par At; Ex Type; short Deg; Con Back; St Prev, Next, Forth; short Echo; Ex Body; int Seq;
};
struct Term {
   enum KindT Kind; Def Fun; List Args;
};
struct App {
   enum KindT Kind; Ex Fun, Arg;
};
struct Lam {
   enum KindT Kind; char *Id; Ex Type; Ex Body; Lam Ref; Var Clone;
};
struct Var {
   enum KindT Kind; Lam Bind;
};

// Syntax.c:
typedef union { bool IVal; char *SVal; Par PVal; Ex EVal; List LVal; } YYSTYPE;
extern YYSTYPE yylval;
bool yyparse(void);

// Lexical.c:
int yylex(void);
extern char yytext[]; // With the original lex this should be: "extern char yytext[];"!

// AutoMath.c:
extern int Line, ComDepth;
extern bool DoA, DoC, DoO, DoQ, DoD, DoL, DoM, DoR, DoV;
extern bool DoB, DoE, DoF, DoI, DoY, DoZ, DoK, DoN, DoT, DoX;
extern int ArgK, ArgN, ArgT, ArgX;

#define StdSum (DoZ? stdout: stderr)

void Yield(void);
void Error(char *Format, ...);
void yyerror(char *Msg);
#undef yywrap
bool yywrap(void);

// Ex.c:
void BegEx(void);
Term NewTerm(Def Fun, List Args);
Ex Call(Ex Fun, List Args, bool IsCall);
Ex Apply(Ex Fun, Ex Arg);
Ex Applies(Ex Fun, List Args);
Var AbsFrame(char *Id, Ex Type);
Ex OpenAbs(char *Id, Ex Type);
List AddAbs(List Ds, char *Id, Ex Type);
Ex CloseAbs(Ex D, Ex Body);
Ex CloseAbses(List Ds, Ex Body);
List AddList(List Prev, Ex E);

typedef struct Value *Value;
struct Value { char *Id, *ExVal; Value Prev; };

#ifdef NOCHECK
#   define assert(X) ((void)(X))
#else
#   include <assert.h>
#endif

// Allocate.c:
extern bool MayRestore;
int ShowMemUsed(FILE *ExF);
void NoMem(void);
void BegMem(void);
void EndMem(void);
void *Allocate(size_t N);
void PutMem(void);
void GetMem(void);

// Id.c:
char *FindId(char *S);
void SetId(char *S, char *X);
char *GetId(char *S);
char *Other(char *S);

// Par.c:
extern Par CurPar, TopList;
int ShowPar(FILE *ExF, Par P);
int ShowRelPar(FILE *ExF, Par P, Par Q);
void BegPar(void);
void EndPar(void);
Par SupPar(char *Id);
Par SubPar(Par P, char *Id);
void OpenPar(char *Id);
void ClosePar(char *Id);

// St.c:
extern bool InBody;
extern Con CurCon;
extern St FirstSt;
void BegSt(void);
void EndSt(void);
Ex OneEx(char *Kind);
void BegCon(void);
void EndCon(void);
void FinalEndCon(void);
void Recover(void);
void SetCon(Ex C);
void NewCon(char *Id, Ex Type);
void NewDef(char *Id, Ex Body, Ex Type, bool NoEx);

// Value.c:
void PutValue(char *Id, char *Val);
void Mark(char *Val);
void GetValue(void);
char *RestoreToMark(void);
Ex FindSym(char *Id);
Ex FindEx(Par P, char *Id);

// Print.c:
int PutSym(FILE *ExF, Ex E);
int ShowEx(FILE *ExF, Ex E);
bool Occurs(Ex E, Lam L);
int PutEx(FILE *ExF, Ex E);

// Type.c:
Ex SubstVar(Ex D, Lam L, Ex E);
Ex SubstCon(List A, Con C, Ex E);
Ex TypeOf(Ex E);
int Degree(Ex E);

// Eval.c:
extern bool Extensional;
void BegEval(void);
void EndEval(void);
bool Beta(Ex *EP);
bool Delta(Ex *EP);
bool Eval(Ex D, Ex E);

// Check.c:
extern bool Truncate; extern int Ratio;
bool CheckType(Ex Body, Ex Type);
bool CheckDegree(Ex E, int Lo, int Hi);
bool Check(Ex E);

// Excerpt.c:
void Excerpt(void);
void Everything(void);
