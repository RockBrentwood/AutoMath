// Id values are initialized to 0.
// Only call [SG]etId() on char *'s returned by FindId().
#include <stdlib.h>
#include <stdio.h>
#include "AutoMath.h"

static char *IdTab[2021]; const size_t HashMod = sizeof IdTab/sizeof IdTab[0];
static char *OtherX = NULL; static size_t OtherN = 0;

char *FindId(char *S) {
   int H = 0;
   for (char *P = S; *P != '\0'; P++) H += (H << 4) + *P + 1;
   H = (unsigned)H%HashMod;
   for (char *R = IdTab[H]; R != NULL; R = *(char **)R) {
      char *T = R + 2*sizeof(char *), Ch;
      for (char *P = S, *Q = T; (Ch = *P++) == *Q++; ) if (Ch == '\0') return T;
   }
   int N = 0;
   for (char *P = S; *P != '\0'; N++, P++);
   char *R = Allocate(2*sizeof(char *) + N + 1);
   *(char **)R = IdTab[H], IdTab[H] = R, *(char **)(R + sizeof(char *)) = NULL;
   char *T = R + 2*sizeof(char *), *Q = T;
   for (char *P = S; *P != '\0'; ) *Q++ = *P++;
   *Q = '\0';
   return T;
}

void SetId(char *S, char *X) { *(char **)(S - sizeof(char *)) = X; }
char *GetId(char *S) { return *(char **)(S - sizeof(char *)); }

char *Other(char *S) {
   int N = 0;
   for (char *P = S; *P != '\0'; N++, P++);
   if (N + 2 > OtherN) {
      if (OtherN == 0) OtherN = 0x10; else OtherN *= 2;
      if (N + 2 > OtherN) OtherN = N + 2;
      OtherX = Allocate(OtherN);
   }
   char *Q = OtherX;
   for (char *P = S; (*Q = *P++) != '\0'; Q++);
   *Q++ = '\'', *Q = '\0';
   MayRestore = false;
   return FindId(OtherX);
}
