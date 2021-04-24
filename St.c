#include <stdio.h>
#include <stdlib.h>
#include "AutoMath.h"

bool InBody = false;
Con CurCon = NULL;
St FirstSt = NULL;

static enum { ConEx = 0, ConAt = 1, ConIn = 2 } ConQ = ConEx;
static int Seq = 0, Cons = 0, Defs = 0, Args = 0;
static One Type1, Prop1;

void BegSt(void) {
   Type1 = Allocate(sizeof *Type1), Type1->Kind = OneK, Type1->Id = FindId("type");
   Prop1 = Allocate(sizeof *Prop1), Prop1->Kind = OneK, Prop1->Id = FindId("prop");
}

void EndSt(void) {
   if (DoL) fprintf(StdSum, "%d + %d = %d definitions, %d + %d = %d lines\n", Cons, Defs, Cons + Defs, Args, Cons + Defs, Cons + Defs + Args);
}

Ex OneEx(char *Kind) { return (Ex)(*Kind == 't'? Type1: Prop1); }

static void PullArgs(Con C) {
   if (C != NULL) PullArgs(C->Back), PutValue(C->Id, (char *)C);
}

void BegCon(void) {
   if (DoC) Mark((char *)NULL), PullArgs(CurCon); // Stack overflow?
   for (Con C = CurCon; C != NULL; C = C->Back) C->Ref = (Ex)C;
   InBody = true, ConQ = ConIn;
}

void EndCon(void) {
   InBody = false;
   for (Con C = CurCon; C != NULL; C = C->Back) C->Ref = NULL;
// Delay RestoreToMark() until just before the end of NewCon()/NewDef(), for PutEx().
   ConQ = ConAt;
}

void FinalEndCon(void) {
   if (DoC) assert(RestoreToMark() == NULL);
   ConQ = ConEx;
}

void Recover(void) {
   switch (ConQ) {
      case ConIn:
         EndCon();
      case ConAt:
         FinalEndCon();
      break;
   }
}

void SetCon(Ex C) {
   if (C != NULL && C->Kind != ConK) {
      assert(C->Kind == DefK);
      Def D = (Def)C;
      Error("can't set context to a defined constant: \"%s\"\n", D->Id), CurCon = NULL;
   } else CurCon = (Con)C;
}

static St AddSt(St S) {
   static St CurCover = NULL, CurSt = NULL;
   St ExSt = CurPar == NULL? CurCover: CurPar->End;
   if (CurPar == NULL) CurCover = S;
   else {
      if (ExSt != NULL) ExSt->Next = S; else CurPar->Beg = S;
      CurPar->End = S;
   }
   if (CurSt != NULL) CurSt->Forth = S; else FirstSt = S;
   CurSt = S;
   return ExSt;
}

void NewCon(char *Id, Ex Type) {
   EndCon();
   for (Con C = CurCon; C != NULL; C = C->Back)
      if (C->Id == Id) Error("duplicate variable identifier in context: \"%s\"\n", Id);
   Con C = Allocate(sizeof *C);
   C->Kind = ConK, C->Id = Id, C->At = CurPar, C->Back = CurCon, C->Prev = AddSt((St)C), C->Next = NULL, C->Forth = NULL;
   C->Echo = 0, C->Ref = NULL;
   List Exs = Allocate(sizeof *Exs);
   Exs->Prev = CurCon == NULL? NULL: CurCon->Exs, Exs->Arg = (Ex)C, C->Exs = Exs;
   PutMem();
   if (Type == NULL || !Check(Type) || !CheckDegree(Type, 1, 2) || CurCon != NULL && CurCon->Type == NULL)
      C->Type = NULL, C->Deg = 0;
   else
      C->Type = Type, C->Deg = Degree(Type);
   GetMem(), FinalEndCon();
   St S = (St)GetId(Id);
   if (S != NULL) {
      assert(S->Kind == ConK || S->Kind == DefK);
      if (S->At == CurPar && S->Kind == DefK) Error("variable covers definition with same name on same level: \"%s\"\n", Id);
   }
   PutValue(Id, (char *)C), CurCon = C, Args++;
}

void NewDef(char *Id, Ex Body, Ex Type, bool NoEx) {
   EndCon();
   Def D = Allocate(sizeof *D);
   D->Kind = DefK, D->Id = Id, D->At = CurPar, D->Back = CurCon, D->Prev = AddSt((St)D), D->Next = NULL, D->Forth = NULL;
   D->Echo = DoX && (Body == NULL && ArgX == 0 || Line == ArgX)? 1: 0, D->Seq = Seq++;
   PutMem();
   if (Type == NULL || !Check(Type) || !CheckDegree(Type, 1, 2) || CurCon != NULL && CurCon->Type == NULL)
      D->Type = NULL, D->Deg = 0;
   else
      D->Type = Type, D->Deg = Degree(Type), D->Body = Body == NULL || !Check(Body) || !CheckType(Body, Type) || NoEx? NULL: Body;
   GetMem(), FinalEndCon();
   St S = (St)GetId(Id);
   if (S != NULL) {
      assert(S->Kind == ConK || S->Kind == DefK);
      if (S->At == CurPar) Error("definition covers item with same name on same level: \"%s\"\n", Id);
   }
   PutValue(Id, (char *)D);
   if (Body != NULL) Defs++; else Cons++;
}
