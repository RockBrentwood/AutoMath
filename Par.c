#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

Par CurPar = NULL, TopList = NULL;
#define SubOf(P) ((P) != NULL? (P)->Sub: TopList)

int ShowPar(FILE *ExF, Par P) {
   if (P == NULL) return fprintf(ExF, "{cover}");
   else if (P->Sup == NULL) return fprintf(ExF, "%s", P->Id);
   else {
      int N = ShowPar(ExF, P->Sup);
      return N + fprintf(ExF, ".%s", P->Id);
   }
}

int ShowRelPar(FILE *ExF, Par P, Par Q) {
   if (P == Q) return 0; else if (P == NULL) return fprintf(ExF, "{cover}");
   for (Par R = Q; R != NULL; R = R->Sup) if (R == P) return fprintf(ExF, "%s", P->Id);
   int N = ShowRelPar(ExF, P->Sup, Q);
   return N + fprintf(ExF, ".%s", P->Id);
}

void BegPar(void) { Mark((char *)CurCon); }

void EndPar(void) {
   if (CurPar != NULL) {
      Error("book doesn't end at cover: \"%P\"\n", CurPar);
      while (CurPar != NULL) CurPar = CurPar->Sup, SetCon((Ex)RestoreToMark());
   }
   SetCon((Ex)RestoreToMark());
}

Par SupPar(char *Id) {
   for (Par P = CurPar; P != NULL; P = P->Sup) if (P->Id == Id) return P;
   Error("incorrect paragraph reference: \"%s\" in paragraph \"%P\"\n", Id, CurPar);
   return CurPar;
}

Par SubPar(Par P, char *Id) {
   for (Par Q = SubOf(P); Q != NULL; Q = Q->Prev) if (Q->Id == Id) return Q;
   Error("incorrect paragraph reference: \"-%s\" in paragraph \"%P\"\n", Id, P);
   return P;
}

void OpenPar(char *Id) {
   for (Par P = SubOf(CurPar); P != NULL; P = P->Prev) if (P->Id == Id) {
      CurPar = P, Mark((char *)CurCon);
      for (St D = CurPar->Beg; D != NULL; D = D->Next) PutValue(D->Id, (char *)D);
      return;
   }
   for (Par P = CurPar; P != NULL; P = P->Sup)
      if (P->Id == Id) Error("duplicate nested identifiers: \"%s\" in paragraph \"%P\"\n", Id, CurPar);
   Par P = Allocate(sizeof *P);
   P->Id = Id, P->Sup = CurPar, P->Sub = NULL, P->Prev = SubOf(CurPar);
   if (CurPar != NULL) CurPar->Sub = P; else TopList = P;
   P->End = P->Beg = NULL, CurPar = P;
   Mark((char *)CurCon);
}

void ClosePar(char *Id) {
   if (CurPar != NULL && (CurPar->Id == Id || Id == NULL)) CurPar = CurPar->Sup, SetCon((Ex)RestoreToMark());
   else if (Id == NULL) Error("closing paragraph with no paragraphs open\n");
   else {
      Error("closing paragraph that's not open: \"%s\" in paragraph \"%P\"\n", Id, CurPar);
      for (Par P = CurPar; P != NULL; P = P->Sup) if (P->Id == Id) {
         for (Par R = CurPar; R != P; R = R->Sup) SetCon((Ex)RestoreToMark());
         CurPar = P;
         break;
      }
   }
}
