#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

bool Truncate; int Ratio; // Grundlagen needs 74 for T10{.8286}.

bool CheckType(Ex Body, Ex Type) {
   assert(Body != NULL && Type != NULL);
   int N = Degree(Body); assert(N > 0);
   if (N <= 1) { Error("type error: \"%E\" does not have a type, != %E\n", Body, Type); return false; }
   int M = Degree(Type); assert(M > 0);
   Ex T = TypeOf(Body); assert(T != NULL);
   bool H = N == M + 1;
   if (H) {
      Truncate = DoN; if (Truncate) Ratio = ArgN;
      H = Eval(T, Type);
      if (!H && !DoE) {
         if (Truncate) Ratio = ArgN;
         Extensional = true, H = Eval(T, Type), Extensional = false;
      }
      Truncate = false;
   }
   if (!H) { Error("type error: %E: %E != %E\n", Body, T, Type); return false; }
   return true;
}

static bool CheckArgs(Con C, List A) {
   assert((A == NULL) == (C == NULL));
   return A == NULL || CheckArgs(C->Back, A->Prev) && CheckType(A->Arg, SubstCon(A->Prev, C->Back, C->Type));
}

static Ex Domain(Ex E) {
   do {
      assert(E != NULL && E->Kind != DefK);
      if (E->Kind == AbsK) { Lam L = (Lam)E; return L->Type; }
      else if (Degree(E) > 1) { Ex F = Domain(TypeOf(E)); if (F != NULL) return F; }
   } while (E->Kind == TermK && Delta(&E) || E->Kind == AppK && Beta(&E));
   return NULL;
}

static bool CheckDomain(Ex Fun, Ex Arg) {
   assert(Fun != NULL && Arg != NULL);
   Ex Dom = Domain(Fun); if (Dom == NULL) { Error("expression is not a function: %E\n", Fun); return false; }
   return CheckType(Arg, Dom);
}

bool CheckDegree(Ex E, int Lo, int Hi) {
   assert(E != NULL);
   int N = Degree(E);
   if (N < Lo || N > Hi) {
      if (Lo != Hi) Error("expression should have degree %d or %d: %E\n", Lo, Hi, E);
      else Error("expression should have degree %d: %E\n", Lo, E);
      return false;
   }
   return true;
}

bool Check(Ex E) {
   assert(E != NULL);
   switch (E->Kind) {
      case OneK: case VarK: break;
      case ConK: {
         Con C = (Con)E; if (C->Type == NULL) return false;
      }
      break;
      case TermK: {
         Term T = (Term)E;
         if (T->Fun->Type == NULL) return false;
         for (List A = T->Args; A != NULL; A = A->Prev) if (!Check(A->Arg) || !CheckDegree(A->Arg, 2, 3)) return false;
         if (!CheckArgs(T->Fun->Back, T->Args)) return false;
      }
      break;
      case AppK: {
         App A = (App)E;
         if (!Check(A->Fun) || !Check(A->Arg) || !CheckDegree(A->Arg, 3, 3) || !CheckDomain(A->Fun, A->Arg)) return false;
      }
      break;
      case AbsK: {
         Lam L = (Lam)E;
         if (!Check(L->Type) || !Check(L->Body) || !CheckDegree(L->Type, 2, 2)) return false;
         if (Degree(L->Body) == 1 && !DoA) { Error("illegal abstraction of degree one: %E\n", E); return false; }
      }
      break;
      default: case DefK: assert(false); break;
   }
   return true;
}
