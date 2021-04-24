#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

static bool NewOnes = true, Contexting, CurConValid;

static void MarkSt(St S) {
   if (S->Echo == 0) S->Echo = 1, NewOnes = true;
}

static void ScanEx(Ex E) {
   if (E != NULL) switch (E->Kind) {
      case TermK: {
         Term T = (Term)E; MarkSt((St)T->Fun); // (##) Added the (St) typecast to the original.
         for (List A = T->Args; A != NULL; A = A->Prev) ScanEx(A->Arg);
      }
      break;
      case AppK: {
         App A = (App)E; ScanEx(A->Fun), ScanEx(A->Arg);
      }
      break;
      case AbsK: {
         Lam L = (Lam)E; ScanEx(L->Type), ScanEx(L->Body);
      }
      break;
   }
}

static void NewParams(Par P) {
   for (; P != NULL; P = P->Prev) P->End = P->Beg = NULL, NewParams(P->Sub);
}

static bool Contains(Par P, Par Q) {
   for (; Q != P; Q = Q->Sup) if (Q == NULL) return false;
   return true;
}

static void ChangePar(FILE *ExF, Par P, Par Q) {
   if (P == Q) return;
   ChangePar(ExF, P, Q->Sup);
   if (Q->End != NULL) CurConValid = false;
   fprintf(ExF, "+%s\n", Q->Id);
   OpenPar(Q->Id);
}

static void PutSt(FILE *ExF, St S) {
   Par P = S->At;
   if (P != CurPar) {
      if (Contexting) fprintf(ExF, "\n");
      while (!Contains(CurPar, P)) fprintf(ExF, "-%s\n", CurPar->Id), ClosePar((char *)NULL);
      if (CurPar != P) ChangePar(ExF, CurPar, P);
      Contexting = false;
   }
   if (!CurConValid || S->Back != CurCon) {
      CurCon = S->Back; if (CurCon != NULL) PutSym(ExF, (Ex)CurCon);
      fprintf(ExF, "@"), CurConValid = true;
   }
   BegCon();
   assert(S->Kind == DefK || S->Kind == ConK);
   switch (S->Kind) {
      case DefK: {
         Def D = (Def)S;
         if (Contexting) fprintf(ExF, "\n");
         if (D->Body == NULL) fprintf(ExF, "prim "), PutEx(ExF, S->Type), fprintf(ExF, " %s;\n", S->Id);
         else fprintf(ExF, "def "), PutEx(ExF, S->Type), fprintf(ExF, " %s = ", S->Id), PutEx(ExF, D->Body), fprintf(ExF, ";\n");
         Contexting = false;
      }
      break;
      case ConK:
         fprintf(ExF, "var "), PutEx(ExF, S->Type), fprintf(ExF, " %s; ", S->Id);
         Contexting = true, CurCon = (Con)S;
      break;
   }
   EndCon(), FinalEndCon();
}

static void ShowEcho(FILE *ExF) {
   NewParams(TopList), CurConValid = true, Contexting = false;
   for (St S = FirstSt; S != NULL; S = S->Forth) if (S->Echo != 0) {
      PutSt(ExF, S);
      if (CurPar != NULL) {
         St E = CurPar->End; S->Prev = E;
         if (E != NULL) E->Next = S; else CurPar->Beg = S;
         CurPar->End = S;
      }
      S->Next = NULL, PutValue(S->Id, (char *)S), Yield();
   }
   while (CurPar != NULL) fprintf(ExF, "-%s\n", CurPar->Id), ClosePar((char *)NULL);
}

void Excerpt(void) {
   while (NewOnes) {
      NewOnes = false;
      for (St S = FirstSt; S != NULL; S = S->Forth) if (S->Echo == 1) {
         if (S->Kind == DefK) {
            Def D = (Def)S;
            for (Con E = S->Back; E != NULL; E = E->Back) MarkSt((St)E);
            ScanEx(D->Body);
         }
         ScanEx(S->Type), S->Echo = 2, Yield();
      }
   }
   ShowEcho(stdout);
}

void Everything(void) {
   for (St S = FirstSt; S != NULL; S = S->Forth) S->Echo = 2;
   ShowEcho(stdout);
}
