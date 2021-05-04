#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

static List SubstExs(Ex D, Lam L, List A) {
   if (A != NULL) {
      List B = SubstExs(D, L, A->Prev); Ex C = SubstVar(D, L, A->Arg);
      if (B != A->Prev || C != A->Arg) return AddList(B, C);
   }
   return A;
}

Ex SubstVar(Ex D, Lam L, Ex E) {
   assert(E != NULL);
   switch (E->Kind) {
      case TermK: {
         Term T = (Term)E; List A = SubstExs(D, L, T->Args);
         if (A != T->Args) return (Ex)NewTerm(T->Fun, A);
      }
      break;
      case AppK: {
         App A = (App)E; Ex F = SubstVar(D, L, A->Fun), G = SubstVar(D, L, A->Arg);
         if (F != A->Fun || G != A->Arg) return Apply(F, G);
      }
      break;
      case AbsK: {
         Lam LE = (Lam)E; Ex F = SubstVar(D, L, LE->Type);
         assert(LE->Clone == NULL);
         Var V = AbsFrame(LE->Id, F); LE->Clone = V;
         Ex G = SubstVar(D, L, LE->Body); LE->Clone = NULL;
         if (F != LE->Type || G != LE->Body) {
            E = (Ex)V->Bind, LE = (Lam)E, LE->Body = G; return E;
         }
      }
      break;
      case VarK: {
         Var V = (Var)E; if (V->Bind == L) return D;
         V = V->Bind->Clone; if (V != NULL) return (Ex)V;
      }
      break;
   }
   return E;
}

static Lam WireAbst(Lam E, Ex Type, Ex Body) {
   Var D = AbsFrame(E->Id, Type);
   Lam L = D->Bind; L->Body = SubstVar((Ex)D, E, Body);
   return L;
}

static Ex FillEx(Ex E); // (@) Recursive call.

static List FillExs(List A) {
   if (A != NULL) {
      List B = FillExs(A->Prev); Ex C = FillEx(A->Arg);
      if (B != A->Prev || C != A->Arg) return AddList(B, C);
   }
   return A;
}

static Ex FillEx(Ex E) {
   if (E != NULL) switch (E->Kind) {
      case ConK: {
         Con C = (Con)E; E = C->Ref; assert(E != NULL);
      }
      break;
      case TermK: {
         Term T = (Term)E; List A = FillExs(T->Args);
         if (A != T->Args) return (Ex)NewTerm(T->Fun, A);
      }
      break;
      case AppK: {
         App A = (App)E; Ex F = FillEx(A->Fun), G = FillEx(A->Arg);
         if (F != A->Fun || G != A->Arg) return Apply(F, G);
      }
      break;
      case AbsK: {
         Lam L = (Lam)E; Ex F = FillEx(L->Type), G = FillEx(L->Body);
         if (F != L->Type || G != L->Body) return (Ex)WireAbst(L, F, G);
      }
      break;
   }
   return E;
}

Ex SubstCon(List A, Con C, Ex E) {
   Con D = C;
   for (; A != NULL && D != NULL; A = A->Prev, D = D->Back) { assert(D->Ref == NULL); D->Ref = A->Arg; }
   assert(A == NULL && D == NULL);
   Ex F = FillEx(E);
   for (D = C; D != NULL; D = D->Back) { assert(D->Ref != NULL); D->Ref = NULL; }
   return F;
}

Ex TypeOf(Ex E) {
   assert(E != NULL);
   switch (E->Kind) {
      case ConK: { Con C = (Con)E; return C->Type; }
      case TermK: { Term T = (Term)E; return SubstCon(T->Args, T->Fun->Back, T->Fun->Type); }
      case AppK: { App A = (App)E; return Apply(TypeOf(A->Fun), A->Arg); }
      case AbsK: { Lam L = (Lam)E; Ex D = TypeOf(L->Body); return D->Kind == OneK && DoB? D: (Ex)WireAbst(L, L->Type, D); }
      case VarK: { Var V = (Var)E; return V->Bind->Type; }
   }
   return 0;
}

int Degree(Ex E) {
   int N = 1;
Evaluate:
   assert(E != NULL);
   switch (E->Kind) {
      case OneK: return N;
      case DefK: case ConK: { St S = (St)E; return N + S->Deg; }
      case TermK: { Term T = (Term)E; E = (Ex)T->Fun; } goto Evaluate;
      case AppK: { App A = (App)E; E = A->Fun; } goto Evaluate;
      case AbsK: { Lam L = (Lam)E; E = L->Body; } goto Evaluate;
      case VarK: { Var V = (Var)E; E = (Ex)V->Bind->Type, N++; } goto Evaluate;
   }
   assert(false); return 0;
}
