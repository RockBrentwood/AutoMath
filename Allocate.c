// Allocate() currently returns memory in blocks sized in 4-byte multiples.
// It _should_ be maximally aligned, really PutMem()/GetMem() pairs cannot be nested.
#include <stdlib.h>
#include <stdio.h>
#include "AutoMath.h"

typedef struct Block *Block;
struct Block { char *X; int N; Block Next; };
static Block BegB, CurB, OldB;
static char *CurX, *EndX, *OldX;

// HeaderN: Here, the 4 is hard-coded.
// BlockN: 32 bytes for _them_: malloc uses 4 and the memory manager uses another 12.
static const size_t HeaderN = (sizeof(struct Block) + 3)&~3, BlockN = 0x8000 - 32 - 12;

bool MayRestore;

int ShowMemUsed(FILE *ExF) {
   int N = 0;
   for (Block B = BegB; B != NULL; N++, B = B->Next);
   int M = fprintf(ExF, "%d blocks = ", N);
   N = 0;
   for (Block B = BegB; B != NULL; N += HeaderN + B->N, B = B->Next);
   return M + fprintf(ExF, "%d kilobytes", (N + (1 << 10) - 1) >> 10);
}

void NoMem(void) {
   Error("out of memory: %M were used\n");
   for (Block B = BegB; B != NULL; ) { Block A = B->Next; free(B), B = A; }
   exit(3);
}

void BegMem(void) {
   CurB = BegB = malloc(HeaderN + BlockN); if (CurB == NULL) NoMem();
   CurB->X = (char *)CurB + HeaderN, CurB->N = BlockN, CurB->Next = NULL;
   CurX = CurB->X, EndX = CurX + CurB->N;
   OldB = NULL, OldX = NULL;
}

void EndMem(void) {
   if (DoM) ShowMemUsed(StdSum), fprintf(StdSum, "\n");
}

void *Allocate(size_t N) {
   N = (N + 3)&~3; // Here the four is hard coded.
   char *X = CurX; CurX = X + N;
   if (CurX > EndX) {
      Block A = CurB, B = A->Next;
      for (; B != NULL; A = B, B = A->Next) if (B->N >= N) break;
      if (B != NULL) A->Next = B->Next;
      else {
         int M = BlockN; if (M < N) M = N;
         B = (Block)malloc(HeaderN + M); if (B == NULL) NoMem();
         B->X = (char *)B + HeaderN, B->N = M;
      }
      B->Next = CurB->Next, CurB = CurB->Next = B;
      CurX = CurB->X, EndX = CurX + CurB->N;
      X = CurX, CurX = X + N;
   }
   return (void *)X;
}

void PutMem(void) {
   assert(OldB == NULL && OldX == NULL);
   OldB = CurB, OldX = CurX, MayRestore = true;
}

void GetMem(void) {
   assert(OldB != NULL && OldX != NULL);
   if (MayRestore) CurB = OldB, CurX = OldX, EndX = CurB->X + CurB->N;
   OldB = NULL, OldX = NULL;
}
