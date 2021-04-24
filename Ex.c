#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

static List ErrExs;

void BegEx(void) {
   ErrExs = Allocate(sizeof *ErrExs);
   ErrExs->Prev = NULL, ErrExs->Arg = NULL;
}

Term NewTerm(Def Fun, List Args) {
   Term T = Allocate(sizeof *T);
   T->Kind = TermK, T->Fun = Fun, T->Args = Args;
   return T;
}

Ex Call(Ex Fun, List Args, bool IsCall) {
   if (Fun == NULL || Args == ErrExs) return NULL;
   if (Fun->Kind == DefK) {
      Def F = (Def)Fun; List B = NULL, A = Args; Con D = F->Back;
      for (; A != NULL && D != NULL; B = A, A = A->Prev, D = D->Back);
      if (A != NULL) { Error("function has too many arguments: \"%s\"\n", F->Id); return NULL; }
      if (D != NULL) {
         if (D->Ref == NULL) { Error("implicit variable not in context: \"%s\", of function \"%s\"\n", D->Id, F->Id); return NULL; }
         if (B != NULL) B->Prev = D->Exs; else Args = D->Exs;
      }
      return (Ex)NewTerm(F, Args);
   } else {
      if (IsCall) {
         if (Fun->Kind == ConK) {
            Con C = (Con)Fun; Error("a constant can't have arguments: \"%s\"\n", C->Id);
         } else if (Fun->Kind == VarK) {
            Var V = (Var)Fun; Error("a variable can't have arguments: \"%s\"\n", V->Bind->Id);
         } else Error(""), assert(false);
      }
      assert(Fun->Kind == ConK || Fun->Kind == VarK);
      return Fun;
   }
}

Ex Apply(Ex Fun, Ex Arg) {
   if (Arg == NULL || Fun == NULL) return NULL;
   App E = Allocate(sizeof *E);
   E->Kind = AppK, E->Fun = Fun, E->Arg = Arg;
   return (Ex)E;
}

Ex Applies(Ex Fun, List Args) {
   if (Args != (List)NULL) {
      List ArgsR = (List)NULL;
      do { List Prev = Args->Prev; Args->Prev = ArgsR, ArgsR = Args, Args = Prev; } while (Args != (List)NULL);
      for (; ArgsR != (List)NULL; ArgsR = ArgsR->Prev) Fun = Apply(Fun, ArgsR->Arg);
   }
   return Fun;
}

Var AbsFrame(char *Id, Ex Type) {
   Lam E = Allocate(sizeof *E);
   E->Kind = AbsK, E->Id = Id, E->Type = Type, E->Body = NULL, E->Ref = NULL, E->Clone = NULL;
   Var V = Allocate(sizeof *V);
   V->Kind = VarK, V->Bind = E;
   return V;
}

Ex OpenAbs(char *Id, Ex Type) {
   Var V = AbsFrame(Id, Type);
   PutValue(Id, (char *)V);
   return (Ex)V;
}

List AddAbs(List Ds, char *Id, Ex Type) { return AddList(Ds, OpenAbs(Id, Type)); }

Ex CloseAbs(Ex D, Ex Body) {
   assert(D != NULL && D->Kind == VarK);
   Var V = (Var)D;
   Lam E = V->Bind;
   assert(E != NULL && E->Kind == AbsK);
   assert(GetId(E->Id) == (char *)D);
   GetValue();
   assert(GetId(E->Id) != (char *)D);
   if (E->Type == NULL || Body == NULL) return NULL;
   assert(E->Body == NULL);
   E->Body = Body;
   return (Ex)E;
}

Ex CloseAbses(List Ds, Ex Body) {
   for (; Ds != (List)NULL; Ds = Ds->Prev) Body = CloseAbs(Ds->Arg, Body);
   return Body;
}

List AddList(List Prev, Ex E) {
   if (Prev == ErrExs || E == NULL) return ErrExs;
   List A = Allocate(sizeof *A);
   A->Prev = Prev, A->Arg = E;
   return A;
}
