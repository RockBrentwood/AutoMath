#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "AutoMath.h"

char yytext[0x100]; static char *yyAdP = yytext; const char *yyAdEnd = yytext + sizeof yytext/sizeof yytext[0];
static char yyback[0x100], *yyDeP = yyback; const char *yyDeEnd = yyback + sizeof yyback/sizeof yyback[0];

static int GetCh(bool Keep) {
   int Ch = yyDeP > yyback? *--yyDeP: getchar();
   if (Keep && Ch != EOF) {
      if (yyAdP >= yyAdEnd) fprintf(stderr, "Character buffer yytext full.\n"), exit(1);
      *yyAdP++ = Ch;
   }
   if (Ch == '\n') Line++;
   return Ch;
}

static int NextCh(void) { return yyDeP > yyback? yyDeP[-1]: (*yyDeP++ = getchar()); }

static int UnGetCh(void) {
   int Ch = yyAdP <= yytext? '\0': *--yyAdP;
   if (Ch != '\0' || yyDeP > yyback) {
      if (yyDeP >= yyDeEnd) fprintf(stderr, "Character buffer yyback full.\n"), exit(1);
      *yyDeP++ = Ch;
   }
   if (Ch == '\n') Line--;
   return Ch;
}

static void KeepCh(void) {
   if (yyAdP >= yyAdEnd) fprintf(stderr, "Character buffer A full.\n"), exit(1);
   *yyAdP = '\0', yylval.SVal = FindId(yytext), yyAdP = yytext;
}

#define IsIdCh(Ch) (isalnum(Ch) || (Ch) == '_' || (Ch) == '\b' || (Ch) == '`' || (Ch) == '\'')

int yylex(void) {
   int Ch = '\0';
ReDo:
   yyAdP = yytext;
   switch (Ch = GetCh(true)) {
      case 'v':
         if ((Ch = GetCh(true)) == 'a' && (Ch = GetCh(true)) == 'r' && !IsIdCh(NextCh())) { Ch = 'v'; goto EndX; }
      break;
      case 'd':
         if ((Ch = GetCh(true)) == 'e' && (Ch = GetCh(true)) == 'f' && !IsIdCh(NextCh())) { Ch = 'd'; goto EndX; }
      break;
      case 'p':
         if ((Ch = GetCh(true)) == 'r') switch (Ch = GetCh(true)) {
            case 'i':
               if ((Ch = GetCh(true)) == 'm' && !IsIdCh(NextCh())) { Ch = 'p'; goto EndX; }
            break;
            case 'o':
               if ((Ch = GetCh(true)) == 'p' && !IsIdCh(NextCh())) { Ch = 't'; goto EndX; }
            break;
         }
      break;
      case 't':
         if ((Ch = GetCh(true)) == 'y' && (Ch = GetCh(true)) == 'p' && (Ch = GetCh(true)) == 'e' && !IsIdCh(NextCh())) { Ch = 't'; goto EndX; }
      break;
      case '#': case '%': LineComment:
         while ((Ch = GetCh(false)) != '\n' && Ch != EOF);
      CheckEOF:
         if (Ch == EOF) goto WrapUp;
      goto ReDo;
      case '/': switch (Ch = GetCh(true)) {
         case '/': goto LineComment;
         case '*':
            ComDepth++;
            while ((Ch = GetCh(false)) != EOF) {
               if (Ch == '/') {
                  do Ch = GetCh(false); while (Ch == '/');
                  if (Ch == EOF) break; else if (Ch == '*') ComDepth++;
               } else if (Ch == '*') {
                  do Ch = GetCh(false); while (Ch == '*');
                  if (Ch == EOF) break; else if (Ch == '/' && --ComDepth == 0) goto ReDo;
               }
            }
         goto CheckEOF;
         default: Ch = UnGetCh(); break;
      }
      goto EndX;
      case '\n': case '\r': case ' ': case '\t': goto ReDo;
      case EOF: WrapUp: yywrap(); return EOF;
      default:
         if (!IsIdCh(Ch)) goto EndX;
      break;
   }
   while (IsIdCh(Ch)) Ch = GetCh(true);
   UnGetCh(), Ch = 'x';
EndX:
   KeepCh();
   return Ch;
}
