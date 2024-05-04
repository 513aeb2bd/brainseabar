#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define SYM_PUSH1         '1'
#define SYM_POP           '0'
#define SYM_MOVETO_ST1    '\''
#define SYM_MOVETO_ST0    '\"'
#define SYM_COPY          'I'
#define SYM_EXCHANGE      'O'
#define SYM_ADD           'l'
#define SYM_NAND          '|'
#define SYM_INPUT         'i'
#define SYM_PRINT_ASCII   'j'
#define SYM_PRINT_NUM     'J'
#define SYM_PAREN_OPEN    '['
#define SYM_PAREN_CLOSE   ']'
#define SYM_COMMENT       '#'

#if defined DEBUG
#   define STACK_NUM  5
#endif

// bsbstack.c
extern struct bsbstack* createBsbstack ();
extern id destroyBsbstack (struct bsbstack *st);
extern id checkBsbstackSize (struct bsbstack *st);

id bsbExecute (char *filename, struct bsbstack *st);
id checkSource (ub *buffer);
id interpret (ub *buffer, struct bsbstack *st);

// .

// .

// execute source code in the file
// return 0 on success
id
bsbExecute (char *filename, struct bsbstack *st)
{
   ub *bufbsb;
   uq len_file;
   id fdbsb;

   fdbsb = open ((const char *)filename, O_RDONLY);

   if (fdbsb < 0) {
      printf ("error: couldn't open file\n");
      return -1;
   }

   // copy source code from file to memory
   len_file = lseek (fdbsb, 0, SEEK_END);
   lseek (fdbsb, 0, SEEK_SET);
   bufbsb = malloc (len_file + 2);
   read (fdbsb, bufbsb, len_file);
   bufbsb[len_file + 1] = '\0';

   // validate soure code
   if (checkSource (bufbsb)) {
      return -1;
   }

   // check if bsbstack exists
   if (st == NULL) {
      return -1;
   }

   if (interpret (bufbsb, st)) {
      printf ("error occurred\n");
   }

   free (bufbsb);
   return 0;
}   // endfunc: id bsb-excute

// .

// .

// check parenthesis and comment sign match
// return 0 if no error, -1 if error detected
id
checkSource (ub *buf)
{
   id num_paren_open, comment_open;

   num_paren_open = comment_open = 0;

   for (;;) {
      if (*buf == '\0') {
         break;
      }

      comment_open ^= *buf == SYM_COMMENT;
      buf += 1;

      if (comment_open) {
         continue;
      }

      if (*buf == SYM_PAREN_OPEN) {
         num_paren_open += 1;
      }
      else if (*buf == SYM_PAREN_CLOSE) {
         num_paren_open -= 1;
      }

      // paren_close can not be ahead of paren_open
      if (num_paren_open < 0) {
         return -1;
      }
   }

   // there must be no paren open or comment open
   return -(num_paren_open || comment_open);
}   // endfunc: id check-source

// .

// .

id
interpret (ub *buffer, struct bsbstack *st)
{
   ub *st0, *st1;
   ub cont, rg0, comment_open;
   // how much to jump if condition is true
   // array index is equal to parenthesis id
   uq *table_jump;
   // parenthesis id after jump
   uq *table_parenid;
   // used for making table_jump and table_parenid
   uq *stack_addr, *stack_paren;
   uq sp0, sp1;
   id ret, paren;

#if defined DEBUG
   uq dsp;
#endif   // def DEBUG

   // count total parentheses
   paren = sp0 = 0;
   table_jump = NULL;

   for (;;) {
      if (buffer[sp0] == '\0') {
         break;
      }

      if (!comment_open) {
         paren += buffer[sp0] == SYM_PAREN_OPEN || buffer[sp0] == SYM_PAREN_CLOSE;
      }

      comment_open ^= buffer[sp0] == SYM_COMMENT;
      sp0 += 1;
   }

   // calculate table_jump and table_parenid
   if (paren) {
      table_jump = malloc (paren << 3);
      table_parenid = malloc (paren << 3);
      stack_addr = malloc (paren << 2);
      stack_paren = malloc (paren << 2);

      paren = sp0 = sp1 = 0;
      comment_open = 0;

      // each parenthesis has own paren_id and paren points the id
      // sp0 points the address of current operator
      // sp1 points the top of paren stack
      for (;;) {
         if (buffer[sp0] == '\0') {
            break;
         }

         cont = buffer[sp0];
         sp0 += 1;
         comment_open ^= cont == SYM_COMMENT;

         // skip comment
         if (comment_open || cont == SYM_COMMENT) {
            continue;
         }

         // skip whitespace
         if (cont == ' ' || cont == '\t' || cont == '\n') {
            continue;
         }

         if (cont == SYM_PAREN_OPEN) {
            // push the address of current operator and paren_id
            stack_addr[sp1] = sp0;
            stack_paren[sp1] = paren;
            table_parenid[paren] = paren;
            sp1 += 1;
            paren += 1;
         }
         else if (cont == SYM_PAREN_CLOSE) {
            // pop and calculate address difference
            sp1 -= 1;
            table_jump[stack_paren[sp1]] = sp0 - stack_addr[sp1];
            table_jump[paren] = sp0 - stack_addr[sp1];
            table_parenid[paren] = table_parenid[stack_paren[sp1]];
            table_parenid[stack_paren[sp1]] = paren;
            paren += 1;
         }
      }

      free (stack_addr);
      free (stack_paren);
   }

   // MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN

   // begin interpretation
   ret = -1;
   paren = 0;
   comment_open = 0;
   // get current bsbstack info
   st0 = st->st0;
   st1 = st->st1;
   sp0 = st->sp0;
   sp1 = st->sp1;

   for (;;) {
      // until buffer end
      if (*buffer == '\0') {
         ret = 0;
         break;
      }

      cont = *buffer;    // get current operator
      buffer += 1;    // point next operator
      comment_open ^= cont == SYM_COMMENT;    // toggle comment open

      // skip comment
      if (comment_open || cont == SYM_COMMENT) {
#if defined DEBUG
         printf ("%c", cont);

         if (!comment_open) {
            printf ("\n");
         }
#endif   // def DEBUG
         continue;
      }

      // skip whitespace
      if (cont == ' ' || cont == '\n' || cont == '\t') {
         continue;
      }

      // if the top of st0 is zero, then jump forwards to matching ']'
      if (cont == SYM_PAREN_OPEN) {
         if (!sp0) {
            break;
         }

         if (!st0[sp0 - 1]) {
            buffer += table_jump[paren];
            paren = table_parenid[paren] + 1;

#if defined DEBUG
            printf ("jumped forward.\n");
#endif   // def DEBUG

         }
         else {
            paren += 1;
         }

         continue;
      }

      // if the top of st0 is non-zero, then jump backwards to matching '['
      if (cont == SYM_PAREN_CLOSE) {
         if (!sp0) {
            break;
         }

         if (st0[sp0 - 1]) {
            buffer -= table_jump[paren];
            paren = table_parenid[paren] + 1;

#if defined DEBUG
            printf ("jumped backward.\n");
#endif   // def DEBUG

         }
         else {
            paren += 1;
         }

         continue;
      }

      // push 1 to st0
      if (cont == SYM_PUSH1) {
         st0[sp0] = 1;
         sp0 += 1;
      }
      // pop from st0;
      else if (cont == SYM_POP) {
         if (!sp0) {
            break;
         }

         sp0 -= 1;
      }
      // pop from st0 and push to st1
      else if (cont == SYM_MOVETO_ST1) {
         if (!sp0) {
            break;
         }

         sp0 -= 1;
         st1[sp1] = st0[sp0];
         sp1 += 1;
      }
      // pop from st1 and push to st0
      else if (cont == SYM_MOVETO_ST0) {
         if (!sp1) {
            break;
         }

         sp1 -= 1;
         st0[sp0] = st1[sp1];
         sp0 += 1;
      }
      // copy the top of st0 and push to st0
      else if (cont == SYM_COPY) {
         if (!sp0) {
            break;
         }

         st0[sp0] = st0[sp0 - 1];
         sp0 += 1;
      }
      // exchange the tops of st0 and st1
      else if (cont == SYM_EXCHANGE) {
         if (sp0 < 2) {
            break;
         }

         rg0 = st0[sp0 - 1];
         st0[sp0 - 1] = st0[sp0 - 2];
         st0[sp0 - 2] = rg0;
      }
      // pop two items of st0 and push arithmetic add of two
      else if (cont == SYM_ADD) {
         if (sp0 < 2) {
            break;
         }

         sp0 -= 1;
         st0[sp0 - 1] += st0[sp0];
      }
      // pop two items of st0 and push logical nand of two
      else if (cont == SYM_NAND) {
         if (sp0 < 2) {
            break;
         }

         sp0 -= 1;
         st0[sp0 - 1] = ~(st0[sp0 - 1] & st0[sp0]);
      }
      // input 1 byte and push to st0
      else if (cont == SYM_INPUT) {
         printf (">> ");
         scanf ("%hhd", &rg0);
         getc (stdin);  // act as fflush
         st0[sp0] = rg0;
         sp0 += 1;
      }
      // print top of st0 in ascii
      else if (cont == SYM_PRINT_ASCII) {
         if (!sp0) {
            break;
         }

         printf ("%c", st0[sp0 - 1]);
      }
      // print top of st0 in number
      else if (cont == SYM_PRINT_NUM) {
         if (!sp0) {
            break;
         }

         printf ("%u ", st0[sp0 - 1]);
      }
      // not an operator
      else {
         continue;
      }

      // MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN

      // update bsbstack info
      st->sp0 = sp0;
      st->sp1 = sp1;
      checkBsbstackSize (st);

#if defined DEBUG
      printf ("\t%c | ", cont);
      dsp = STACK_NUM;

      for (;;) {
         if (!dsp) {
            break;
         }

         if (sp0 < dsp) {
            printf ("  . ");
         }
         else {
            printf ("%3d ", st0[sp0 - dsp]);
         }

         dsp -= 1;
      }

      printf ("| ");

      for (dsp = STACK_NUM - 1; 0 <= dsp; dsp -= 1) {
         if (sp1 + dsp < STACK_NUM) {
            printf ("  . ");
         }
         else {
            printf ("%3d ", st1[sp1 + dsp - STACK_NUM]);
         }
      }   // endfor: for each stack

      printf ("|\n");
#endif   // def DEBUG

   }   // endfor: until buffer end

   if (table_jump != NULL) {
      free (table_jump);
      free (table_parenid);
   }

   return ret;
}   // endfunc: id interpret
