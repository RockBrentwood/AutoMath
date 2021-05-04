// exit status:
//	0: correct automath book
//	1: incorrect automath book (either syntax or typing)
//	2: incorrect usage, problems reading or writing files or user abort
//	3: insufficient address space or memory
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include "AutoMath.h"
#ifdef macintosh
#   include <CursorCtl.h> // For SpinCursor() and InitCursorCtl()
#else
#   define SpinCursor(Increment) ((void)(Increment))
#   define InitCursorCtl(NewCursors) ((void)(NewCursors))
#endif

bool DoA, DoC, DoO, DoQ, DoD, DoL, DoM, DoR, DoV;
bool DoB, DoE, DoF, DoI, DoY, DoZ, DoK, DoN, DoT, DoX;
int ArgK, ArgN, ArgT, ArgX;

static char *AppName, *File = "-", **Files;

void Yield(void) {
   SpinCursor(1); // The wait icon.
}

int Line;
static int Errors;
void Error(char *Format, ...) {
   fflush(stdout), fprintf(stderr, "file \"%s\"; line %d # ", File, Line), Errors++;
   va_list AP; va_start(AP, Format);
   for (char *S = Format; *S != '\0'; S++)
      if (*S != '%') fputc(*S, stderr);
      else switch (*++S) {
         case 's': {
            char *T = va_arg(AP, char *); if (T != NULL) for (; *T != '\0'; T++) fputc(*T, stderr);
         }
         break;
         case 'd': {
            int I = va_arg(AP, int); fprintf(stderr, "%d", I);
         }
         break;
         case 'M': ShowMemUsed(stderr); break;
         case 'E': {
            Ex E = va_arg(AP, Ex); ShowEx(stderr, E);
         }
         break;
         case 'P': {
            Par P = va_arg(AP, Par); ShowPar(stderr, P);
         }
         break;
         case '\0': S--;
         default: fputc(*S, stderr); break;
      }
   va_end(AP);
}

static jmp_buf AbortEnv;
static void AbortFn(int Sig) {
   Error("aborted in paragraph \"%P\" with signal %d\n", CurPar, Sig);
   longjmp(AbortEnv, 1);
}

void yyerror(char *Msg) {
   Error("%s: unexpected \"%s\"\n", Msg, yytext);
}

int ComDepth;
bool yywrap(void) {
   if (*Files != NULL) {
      File = *Files++;
      if (!freopen(File, "r", stdin)) {
         char *S = Allocate(strlen(File) + 5); sprintf(S, "%s.aut", File), File = S;
         if (!freopen(File, "r", stdin)) fprintf(stderr, "%s: no such file\n", File), exit(2);
      }
      Line = 1; return false;
   }
   if (ComDepth > 0) Error("book ends inside comment, nested braces: %d\n", ComDepth);
   return true;
}

static void Usage(void) {
   fprintf(stderr, "Usage: %s [-QZ] [-abcdefilmoqrvyz] [-kntx parameters] [--] [files]\n", AppName), exit(2);
}

int main(int AC, char **AV) {
   signal(SIGINT, &AbortFn);
   InitCursorCtl(0);
   AppName = AV[0]; if (AppName == NULL || *AppName == '\0') AppName = "AutoMath";
   ArgK = 1;
   int A = 1; char *Arg;
   for (; A < AC && (Arg = AV[A]) != NULL && *Arg++ == '-'; A++) {
      bool B = false; int Ch, *ParP = NULL;
      for (; (Ch = *Arg) != '\0'; Arg++) switch (Ch) {
         case 'Q': DoQ = DoO = DoC = DoA = true; break;
         case 'Z': DoV = DoR = DoM = DoL = DoD = true; break;
         case 'a': DoA = true; break; case 'c': DoC = true; break; case 'o': DoO = true; break; case 'q': DoQ = true; break;
         case 'd': DoD = true; break; case 'l': DoL = true; break; case 'm': DoM = true; break; case 'r': DoR = true; break; case 'v': DoV = true; break;
         case 'b': DoB = true; break; case 'e': DoE = true; break; case 'f': DoF = true; break;
         case 'i': DoI = true; break; case 'y': DoY = true; break; case 'z': DoZ = true; break;
         case 'k': DoK = true, ParP = &ArgK; goto GetPar;
         case 'n': DoN = true, ParP = &ArgN; goto GetPar;
         case 't': DoT = true, ParP = &ArgT; goto GetPar;
         case 'x': DoX = true, ParP = &ArgX; GetPar:
            if (++A >= AC) { Usage(); break; }
            for (char *T = AV[A]; *T != '\0'; T++) if (*T < '0' || *T > '9') { Usage(); break; }
            *ParP = atoi(AV[A]);
         break;
         case '-': B = true; break;
         default: Usage(); break;
      }
      if (B) { A++; break; }
   }
   time_t Start;
   if (DoD) Start = time(0);
   BegMem(), BegPar(), BegSt(), BegEx(), BegEval();
   ComDepth = 0, Line = 1, Files = AV + A;
   yywrap(); // Open the first file.
   Errors = 0;
   bool Aborted = setjmp(AbortEnv) != 0; if (Aborted) CurPar = NULL; else yyparse();
   EndEval(), EndSt(), EndPar();
   if (Errors == 0) {
      if (DoX) Excerpt();
      if (DoY) Everything();
   }
   EndMem();
   if (DoD) {
      int dT = difftime(time(0), Start);
      fprintf(StdSum, "%d seconds = %d minutes %d seconds\n", dT, dT/60, dT%60);
   }
   if (DoV) fprintf(StdSum, "AutoMath 4.3, (c) 1997 by satan software; (c) 2016,2021 Darth Ninja\n");
   return Aborted? 2: Errors > 0? 1: 0;
}
