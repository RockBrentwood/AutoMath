#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

static Value ExVs = NULL, UnVs = NULL;

void PutValue(char *Id, char *Val) {
   Value V;
   if (UnVs != NULL) V = UnVs, UnVs = UnVs->Prev; else V = Allocate(sizeof *V);
   V->Id = Id;
   if (Id != NULL) V->ExVal = GetId(Id), SetId(Id, Val); else V->ExVal = Val;
   V->Prev = ExVs, ExVs = V;
}

void Mark(char *Val) { PutValue(NULL, Val); }

static void PopValue(void) {
   assert(ExVs != NULL);
   Value V = ExVs; ExVs = V->Prev, V->Prev = UnVs, UnVs = V;
}

void GetValue(void) {
   assert(ExVs != NULL);
   SetId(ExVs->Id, ExVs->ExVal), PopValue();
}

char *RestoreToMark(void) {
   while (ExVs != NULL && ExVs->Id != NULL) GetValue();
   assert(ExVs != NULL);
   char *Val = ExVs->ExVal;
   PopValue();
   return Val;
}

static Ex CheckContext(Ex E) {
   if (E->Kind == ConK && InBody) {
      Con C = (Con)E; if (C->Ref == NULL) { Error("variable not in context: \"%s\"\n", C->Id); return NULL; }
   }
   return E;
}

Ex FindSym(char *Id) {
   Ex E = (Ex)GetId(Id);
   if (E == NULL) Error("unknown identifier: \"%s\"\n", Id); else E = CheckContext(E);
   return E;
}

Ex FindEx(Par P, char *Id) {
   if (P != NULL) {
      for (St E = P->End; E != NULL; E = E->Prev) if (E->Id == Id) {
         if (DoO && E->Kind == ConK && InBody) Error("variable should not have explicit paragraph: \"%s\"\n", Id);
         return CheckContext((Ex)E);
      }
      Error("unknown identifier in paragraph: \"%s\" in paragraph \"%P\"\n", Id, P);
   }
   return NULL;
}
