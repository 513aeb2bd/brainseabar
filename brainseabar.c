#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>


// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN


#define SYM_PUSH1  '1'
#define SYM_POP  '0'
#define SYM_MOVETO_ST1  0x27
#define SYM_MOVETO_ST0  0x22
#define SYM_COPY  'I'
#define SYM_EXCHANGE  'O'
#define SYM_ADD  'l'
#define SYM_NAND  '|'
#define SYM_INPUT  'i'
#define SYM_PRINT_ASCII  'j'
#define SYM_PRINT_NUM  'J'
#define SYM_PAREN_OPEN  '['
#define SYM_PAREN_CLOSE  ']'
#define SYM_COMMENT  '#'

#ifdef DEBUG

#define STACK_NUM  5

#endif

typedef uint8_t  ui8;
typedef int32_t  i32;
typedef uint32_t  ui32;
typedef uint64_t  ui64;

// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN

i32 checkSource (ui8* buffer);
i32 interpret (ui8* buffer);



// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN
// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN



int main (ui32 argv, ui8** argc) {
  i32 file;
  ui64 len_file, len_try;
  ui8* buf;

  if (argv == 1) {
    printf ("error: input file name\n");
    return 0;
  }

  file = open (argc[1], O_RDONLY);

  if (file < 0) {
    printf ("error: couldn't open file\n");
    return 0;
  }

  len_file = 0;
  buf = (ui8*)malloc (512);

  for (;;) {
    len_try = read (file, buf + len_file, 512);

    if (len_try < 512) {
      break;
    }

    len_file += 512;
    buf = (ui8*)realloc (buf, len_file + 512);
  }

  len_file += len_try;
  buf[len_file] = '\0';

  if (!checkSource (buf)) {
    if (interpret (buf) < 0) {
      printf ("\nerror occurred\n");
    }
  }

  free (buf);
  return 0;
}



// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN
// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN



i32 checkSource (ui8* buf) {
  i32 num_paren_open, comment_open;

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
  if (num_paren_open || comment_open) {
    return -1;
  }

  return 0;
}



// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN
// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN



i32 interpret (ui8* buffer) {
  ui8* st0, * st1;
  ui8 cont, rg0, comment_open;
  // how much to jump if condition is true
  // array index is equal to parenthesis id
  ui64* table_jump;
  // parenthesis id after jump
  ui64* table_parenid;
  // used for making table_jump and table_parenid
  ui64* stack_addr, * stack_paren;
  ui64 sp0, sp1, siz_st0, siz_st1;
  i32 ret, paren;

#ifdef DEBUG

  ui64 dsp;

#endif

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
    table_jump = (ui64*)malloc (paren << 3);
    table_parenid = (ui64*)malloc (paren << 3);
    stack_addr = (ui64*)malloc (paren << 2);
    stack_paren = (ui64*)malloc (paren << 2);

    paren = sp0 = sp1 = 0;
    comment_open = 0;

    // each parenthesis has own paren_id and paren points the id
    // sp0 points the address of current operator
    // sp1 points the top of paren stack
    for (;;) {
      if (buffer[sp0] == '\0') {
        break;
      }

      if (!comment_open) {
        if (buffer[sp0] == SYM_PAREN_OPEN) {
          // push the address of current operator and paren_id
          stack_addr[sp1] = sp0;
          stack_paren[sp1] = paren;
          table_parenid[paren] = paren;
          sp1 += 1;
          paren += 1;
        }
        else if (buffer[sp0] == SYM_PAREN_CLOSE) {
          // pop and calculate address difference
          sp1 -= 1;
          table_jump[stack_paren[sp1]] = sp0 - stack_addr[sp1];
          table_jump[paren] = sp0 - stack_addr[sp1];
          table_parenid[paren] = table_parenid[stack_paren[sp1]];
          table_parenid[stack_paren[sp1]] = paren;
          paren += 1;
        }
      }

      comment_open ^= buffer[sp0] == SYM_COMMENT;
      sp0 += 1;
    }

    free (stack_addr);
    free (stack_paren);
  }

  // MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN

  // begin interpretation
  ret = -1;
  paren = 0;
  comment_open = 0;

  siz_st1 = siz_st0 = 64;
  sp1 = sp0 = 0;
  st0 = (ui8*)malloc (siz_st0);
  st1 = (ui8*)malloc (siz_st1);

  for (;;) {
    if (*buffer == '\0') {
      ret = 0;
      break;
    }

    cont = *buffer;    // get current operator
    buffer += 1;    // point next operator
    comment_open ^= cont == SYM_COMMENT;    // toggle comment open

    if (comment_open || cont == SYM_COMMENT) {
      continue;
    }

    // skip whitespaces
    if (cont == ' ' || cont == '\n' || cont == '\t') {
      continue;
    }

    // if the top of st0 is zero, then jump forwards to matching ']'
    if (cont == SYM_PAREN_OPEN) {
      if (!sp0) {
        break;
      }

      if (!st0[sp0]) {
        buffer += table_jump[paren];
        paren = table_parenid[paren] + 1;

#ifdef DEBUG

        printf ("jumped forward.\n");

#endif

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

      if (st0[sp0]) {
        buffer -= table_jump[paren];
        paren = table_parenid[paren] + 1;

#ifdef DEBUG

        printf ("jumped backward.\n");

#endif

      }
      else {
        paren += 1;
      }

      continue;
    }

    // push 1 to st0
    if (cont == SYM_PUSH1) {
      sp0 += 1;
      st0[sp0] = 1;
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

      sp1 += 1;
      st1[sp1] = st0[sp0];
      sp0 -= 1;
    }
    // pop from st1 and push to st0
    else if (cont == SYM_MOVETO_ST0) {
      if (!sp1) {
        break;
      }

      sp0 += 1;
      st0[sp0] = st1[sp1];
      sp1 -= 1;
    }
    // copy the top of st0 and push to st0
    else if (cont == SYM_COPY) {
      if (!sp0) {
        break;
      }

      sp0 += 1;
      st0[sp0] = st0[sp0 - 1];
    }
    // exchange the tops of st0 and st1
    else if (cont == SYM_EXCHANGE) {
      if (sp0 < 2) {
        break;
      }

      rg0 = st0[sp0];
      st0[sp0] = st0[sp0 - 1];
      st0[sp0 - 1] = rg0;
    }
    // pop two items of st0 and push arithmetic add of two
    else if (cont == SYM_ADD) {
      if (sp0 < 2) {
        break;
      }

      st0[sp0 - 1] += st0[sp0];
      sp0 -= 1;
    }
    // pop two items of st0 and push logical nand of two
    else if (cont == SYM_NAND) {
      if (sp0 < 2) {
        break;
      }

      st0[sp0 - 1] = ~(st0[sp0 - 1] & st0[sp0]);
      sp0 -= 1;
    }
    // input 1 byte and push to st0
    else if (cont == SYM_INPUT) {
      printf (">> ");
      scanf ("%hhd", &rg0);
      getc (stdin);
      sp0 += 1;
      st0[sp0] = rg0;
    }
    // print top of st0 in ascii
    else if (cont == SYM_PRINT_ASCII) {
      if (!sp0) {
        break;
      }

      printf ("%c", st0[sp0]);
    }
    // print top of st0 in number
    else if (cont == SYM_PRINT_NUM) {
      if (!sp0) {
        break;
      }

      printf ("%u ", st0[sp0]);
    }
    // not an operator
    else {
      continue;
    }

    // MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN

    if (siz_st0 - sp0 == 1) {
      if (!(siz_st0 & 0xfffff)) {
        break;
      }

      siz_st0 <<= 1;
      st0 = (ui8*)realloc (st0, siz_st0);
    }

    if (siz_st1 - sp1 == 1) {
      if (!(siz_st0 & 0xfffff)) {
        break;
      }

      siz_st1 <<= 1;
      st0 = (ui8*)realloc (st1, siz_st1);
    }

#ifdef DEBUG

    printf ("\t%c | ", cont);
    dsp = STACK_NUM;

    for (;;) {
      if (!dsp) {
        break;
      }

      dsp -= 1;

      if (sp0 <= dsp) {
        printf ("  . ");
      }
      else {
        printf ("%3d ", st0[sp0 - dsp]);
      }
    }

    printf ("| ");
    dsp = STACK_NUM;

    for (;;) {
      if (!dsp) {
        break;
      }

      dsp -= 1;

      if (sp1 + dsp < STACK_NUM) {
        printf ("  . ");
      }
      else {
        printf ("%3d ", st1[sp1 + dsp - (STACK_NUM - 1)]);
      }
    }

    printf ("|\n");

#endif

  }

  if (table_jump != NULL) {
    free (table_jump);
    free (table_parenid);
  }

  free (st0);
  free (st1);
  return ret;
}
