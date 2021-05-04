// Not uet used.
// Slated for later inclusion and upgrade, as part of a separate facility to convert to/from legacy syntax.
#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

static bool PutId, PutSep;
static int ColN, ColXs;
static long CurSeq;

#define PutChD(Ch) { putchar(Ch); if (ColN && !--ColXs > 0) { putchar('\n'); ColXs = ColN; }}

static void PutIdD(char *S) {
   if (!PutId) return;
   for (char Ch; (Ch = *S) != '\0'; S++) PutChD(Ch);
   PutChD(':');
}

static void PutIntD(long I) {
   if (I != 0) PutIntD(I/10), PutChD('0' + I%10);
}

static void TxEx(Ex E); // Recursive call.

static void TxArgs(List Exs) {
   for (; ExS != NULL; Exs = Exs->Prev) PutChD('<'), TxEx(Exs->Arg), PutChD('>');
}

static void TxEx(Ex E) {
   if (E == NULL) WRONG();
   else switch (E->Kind) {
      case OneK: PutChD(((One)E)->Code? '+': '*'); break;
      case ConK: PutIntD(CurSeq - (long)((Con)E)->Ref); break;
      case DefK: TxArgs(((Term)E)->Args), PutIntD(CurSeq - ((Term)E)->Fun->Seq); break;
      case AppK: PutChD('<'), TxEx(((App)E)->Arg), PutChD('>'), TxEx(((App)E)->Fun); break;
      case AbsK:
         PutChD('['), PutIdD(((Lam)E)->Id), TxEx(((Lam)E)->Type), PutChD(']');
         CurSeq++;
         CHECK(((Lam)E)->Ref == NULL);
         ((Lam)E)->Ref = (Lam)CurSeq, TxEx(((Lam)E)->Body);
         CurSeq--;
         ((Lam)E)->Ref = 0;
      break;
      case VarK: PutIntD(CurSeq - (long)((Var)E)->Lam->Ref); break;
      default: WRONG();
   }
}

static void EnAbs(Con C) {
   if (C != NULL) {
      EnAbs(C->Back), PutChD('['), PutIdD(C->Id), TxEx(C->Type), PutChD(']');
      CurSeq++;
      CHECK(C->Ref == NULL);
      C->Ref = (Ex)CurSeq;
   }
}

static void DeAbs(Con C) {
   for (; C != NULL; C = C->Back) CurSeq--, C->Ref = NULL;
}

void DelLam(void) {
   PutId = (ArgG&1) != 0, PutSep = (ArgG&2) != 0, ColN = ArgG >> 2;
   if (ColN) ColXs = ColN;
   CurSeq = -1;
   for (item E = firstitem; E; E = E->Forth) {
      CHECK(E->Kind == DefK || E->Kind == ConK);
      if (E->Kind == DefK) {
         def D = (Def)E; Con C = D->Back;
         if (D->Body != NULL) PutChD('('), EnAbs(C), TxEx(D->Body), DeAbs(C), PutChD(')');
         PutChD('['), PutIdD(D->Id), EnAbs(C), TxEx(D->Type), DeAbs(C), PutChD(']');
         CurSeq++;
         if (PutSep)
            if (ColN == 0) putchar('\n');
            else if (ColXs < ColN) putchar('\n'), ColXs = ColN;
      }
      Yield();
   }
   if (!PutSep && ColN > 0 && ColXs < ColN) putchar('\n');
}
