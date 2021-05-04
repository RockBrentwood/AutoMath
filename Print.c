#include <stdlib.h>
#include <stdio.h>
#include "AutoMath.h"

int PutSym(FILE *ExF, Ex E) {
   assert(E != NULL && (E->Kind == ConK || E->Kind == DefK));
   St S = (St)E;
   char *Id = S->Id;
   int N = 0;
   if ((Ex)GetId(Id) == E) N += fprintf(ExF, "%s", Id);
   else {
      Par P = S->At;
      if (FindEx(P, Id) == E) N += fprintf(ExF, "%s{", Id), N += ShowRelPar(ExF, P, CurPar), N += fprintf(ExF, "}");
      else N += fprintf(ExF, "/*%s{", Id), N += ShowRelPar(ExF, P, CurPar), N += fprintf(ExF, "}*/");
   }
   return N;
}

// int ShowEx(FILE *ExF, Ex E); // Recursive call. (@) Already declared in AutoMath.h

static int ShowExs(FILE *ExF, List A, List B) {
   int N = 0;
   if (A != NULL) {
      if (A != B) N += ShowExs(ExF, A->Prev, B), N += fprintf(ExF, ", ");
      N += ShowEx(ExF, A->Arg);
   }
   return N;
}

int ShowEx(FILE *ExF, Ex E) {
   int N = 0;
   if (E == NULL) N += fprintf(ExF, "/*???*/");
   else switch (E->Kind) {
      case OneK: {
         One O = (One)E; N += fprintf(ExF, "%s", O->Id);
      }
      break;
      case ConK: N += PutSym(ExF, E); break;
      case DefK: N += fprintf(ExF, "/*"), N += PutSym(ExF, E), N += fprintf(ExF, "*/"); break;
      case TermK: {
         Term T = (Term)E;
         N += PutSym(ExF, (Ex)T->Fun); //(@) Added the (Ex) typecast to the original.
         List B = NULL;
         switch (ArgK) {
            case 0: {
               List A = T->Args; Con C = T->Fun->Back;
               for (; A != NULL && C != NULL; A = A->Prev, C = C->Back) if (A->Arg != (Ex)C) B = A;
            }
            break;
            case 1: {
               List A = T->Args; Con C = T->Fun->Back;
               for (; A != NULL && C != NULL; A = A->Prev, C = C->Back) if (A != C->Exs) B = A;
            }
            break;
            case 2: default:
               for (List A = T->Args; A != NULL; A = A->Prev) B = A;
            break;
         }
         if (B != NULL) N += fprintf(ExF, "("), N += ShowExs(ExF, T->Args, B), N += fprintf(ExF, ")");
      }
      break;
      case AppK: {
         App A = (App)E; N += ShowEx(ExF, A->Fun), N += fprintf(ExF, "<"), N += ShowEx(ExF, A->Arg), N += fprintf(ExF, ">");
      }
      break;
      case AbsK: {
         Lam L = (Lam)E; char *Id = L->Id;
         if (DoI) N += fprintf(ExF, "/*%08x*/", (unsigned)E);
         N += fprintf(ExF, "["), N += ShowEx(ExF, L->Type), N += fprintf(ExF, " %s: ", Id);
         char *ExVal = GetId(Id);
         SetId(Id, (char *)NULL), N += ShowEx(ExF, L->Body), SetId(Id, ExVal);
         N += fprintf(ExF, "]");
      }
      break;
      case VarK: {
         Var V = (Var)E;
         if (DoI) N += fprintf(ExF, "/*%08x*/", (unsigned)V->Bind);
         N += fprintf(ExF, "%s", V->Bind->Id);
      }
      break;
      default: N += fprintf(ExF, "/*{%d}*/", (int)E->Kind); break;
   }
   return N;
}

bool Occurs(Ex E, Lam L) {
   if (E != NULL) switch (E->Kind) {
      case TermK: {
         Term T = (Term)E; for (List A = T->Args; A != NULL; A = A->Prev) if (Occurs(A->Arg, L)) return true;
      }
      return false;
      case AppK: { App A = (App)E; return Occurs(A->Fun, L) || Occurs(A->Arg, L); }
      case AbsK: { Lam LE = (Lam)E; return Occurs(LE->Type, L) || Occurs(LE->Body, L); }
      case VarK: { Var V = (Var)E; return V->Bind == L; }
   }
   return false;
}

static bool NameClash(Ex E, Lam L) {
   if (E != NULL) switch (E->Kind) {
      case TermK: {
         Term T = (Term)E; for (List A = T->Args; A != NULL; A = A->Prev) if (NameClash(A->Arg, L)) return true;
      }
      return false;
      case AppK: { App A = (App)E; return NameClash(A->Fun, L) || NameClash(A->Arg, L); }
      case AbsK: { Lam LE = (Lam)E; return LE->Id == L->Id? Occurs(LE->Body, L): NameClash(LE->Type, L) || NameClash(LE->Body, L); }
   }
   return false;
}

static void Alpha(Ex E) {
   if (E != NULL) switch (E->Kind) {
      case TermK: {
         Term T = (Term)E; for (List A = T->Args; A != NULL; A = A->Prev) Alpha(A->Arg);
      }
      break;
      case AppK: {
         App A = (App)E; Alpha(A->Fun), Alpha(A->Arg);
      }
      break;
      case AbsK: {
         Lam L = (Lam)E;
         while (NameClash(L->Body, L)) L->Id = Other(L->Id);
         Alpha(L->Type), Alpha(L->Body);
      }
      break;
   }
}

int PutEx(FILE *ExF, Ex E) { Alpha(E); return ShowEx(ExF, E); }
