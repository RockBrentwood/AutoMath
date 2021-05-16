#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

bool Extensional = false;
static int Betas = 0, Deltas = 0, Etas = 0;
static bool Trace; static int TraceLine, Depth;
const int MaxDepth = 32;
#define TraceOn (Trace && (!TraceLine || Line == TraceLine))
#define TraceReturn(R) if (TraceOn) { EndTrace(R); return R; } else return R

void BegEval(void) {
   Trace = DoT;
   if (Trace) TraceLine = ArgT;
}

void EndEval(void) {
   if (DoR) fprintf(StdSum, "%d beta reduction(s), %d delta reduction(s), %d eta reduction(s)\n", Betas, Deltas, Etas);
}

static bool EvalArgs(List A, List B) {
   if (A == B) return true;
   for (; A != NULL && B != NULL; A = A->Prev, B = B->Prev) if (!Eval(A->Arg, B->Arg)) return false;
   assert(A == NULL && B == NULL);
   return true;
}

static void Exhausted(void) {
   if (Ratio == -1) {
      if (TraceOn) fprintf(stdout, "*\n");
      if (DoE || Extensional) Error("maximum number of reductions exceeded: type errors can be wrong\n");
   }
}

bool Delta(Ex *EP) {
   Ex E = *EP; assert(E->Kind == TermK); Term T = (Term)E;
   Def F = T->Fun; Ex B = F->Body; if (B == NULL) return false;
   Yield();
   if (Truncate && Ratio-- <= 0) { Exhausted(); return false; }
   Deltas++; *EP = SubstCon(T->Args, F->Back, B); return true;
}

bool Beta(Ex *EP) {
   Ex E = *EP; assert(E->Kind == AppK); App A = (App)E;
   Ex F = A->Fun;
   while (true) switch (F->Kind) {
      case AbsK:
         assert(F->Kind == AbsK); Lam L = (Lam)F;
         Yield();
         if (Truncate && Ratio-- <= 0) { Exhausted(); return false; }
         Betas++; *EP = SubstVar(A->Arg, L, L->Body);
      return true;
      case AppK:
         if (!Beta(&F)) return false;
      break;
      case TermK:
         if (!Delta(&F)) return false;
      break;
      default: return false;
   }
}

static bool Eta(Ex *EP) {
   assert(Extensional);
   assert(EP != NULL);
   Ex E = *EP; assert(E->Kind == AbsK); Lam L = (Lam)E;
   Ex F = L->Body;
   while (true) switch (F->Kind) {
      case AbsK:
         if (!Eta(&F)) return false;
      break;
      case AppK: {
         App A = (App)F; Ex G = A->Arg;
         if (G->Kind == VarK) {
            Var V = (Var)G;
            if (V->Bind == L && !Occurs(A->Fun, L)) {
               assert(F->Kind == AppK); App A = (App)F;
               Yield();
               if (Truncate && Ratio-- <= 0) { Exhausted(); return false; }
               Etas++; *EP = A->Fun; return true;
            }
         }
         if (!Beta(&F)) return false;
      }
      break;
      case TermK:
         if (!Delta(&F)) return false;
      break;
      default: return false;
   }
}

static void BegTrace(Ex D, Ex E, char *LastRed) {
   fprintf(stdout, "%*s", (Depth - 1)%MaxDepth, "");
   PutEx(stdout, D), fprintf(stdout, " ?= "), PutEx(stdout, E);
   if (*LastRed != '\0') fprintf(stdout, " {:%s}", LastRed);
   fprintf(stdout, "\n"), fflush(stdout);
}

static void EndTrace(bool R) {
   Depth--;
   fprintf(stdout, "%*s%s\n", Depth%MaxDepth, "", R? "+": "-"), fflush(stdout);
}

bool Eval(Ex D, Ex E) {
   assert(D != NULL && E != NULL);
   if (D == E) return true;
#if 1
   if (Degree(D) != Degree(E)) return false;
#endif
   if (TraceOn) Depth++;
   char *LastRed = "";
Reduce:
   if (TraceOn) BegTrace(D, E, LastRed);
   LastRed = "beta";
   if (D->Kind == AppK && Beta(&D) || E->Kind == AppK && Beta(&E)) goto Reduce;
   LastRed = "";
   if (D->Kind == E->Kind) switch (E->Kind) {
      case OneK: TraceReturn(true);
      case ConK: TraceReturn(D == E);
      case VarK: {
         Var VD = (Var)D, VE = (Var)E;
         Lam L = VD->Bind->Ref;
      TraceReturn((L != NULL? L: VD->Bind) == VE->Bind);
      }
      case TermK: {
         Term TD = (Term)D, TE = (Term)E; if (TD->Fun == TE->Fun && EvalArgs(TD->Args, TE->Args)) TraceReturn(true);
      }
      break;
      case AppK: {
         App AD = (App)D, AE = (App)E; if (Eval(AD->Fun, AE->Fun) && Eval(AD->Arg, AE->Arg)) TraceReturn(true);
      }
      break;
      case AbsK: {
         if (D == E) TraceReturn(true);
         Lam LD = (Lam)D, LE = (Lam)E;
         if (Eval(LD->Type, LE->Type)) {
            LD->Ref = LE;
            bool H = Eval(LD->Body, LE->Body);
            LD->Ref = NULL;
            if (H) TraceReturn(true);
         }
      }
      break;
      default: assert(false); break;
   }
   if (E->Kind == OneK && D->Kind == AbsK && DoQ) {
      Lam L = (Lam)D; if (Eval(L->Body, E)) TraceReturn(true);
   }
   LastRed = "delta";
   if (D->Kind == TermK) {
      if (E->Kind == TermK) {
         Term TD = (Term)D, TE = (Term)E;
         if (TD->Fun->Seq > TE->Fun->Seq? Delta(&D) || Delta(&E): Delta(&E) || Delta(&D)) goto Reduce;
      } else if (Delta(&D)) goto Reduce;
   }
   if (E->Kind == TermK && Delta(&E)) goto Reduce;
   LastRed = "eta";
   if (Extensional && (D->Kind == AbsK && Eta(&D) || E->Kind == AbsK && Eta(&E))) goto Reduce;
   TraceReturn(false);
}
