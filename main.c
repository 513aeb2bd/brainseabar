#include <stdio.h>

#include "bsbmain.h"


// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN


// bsbstack.c
extern struct bsbstack* createBsbstack ();
extern int destroyBsbstack (struct bsbstack* st);
// brainseabar.c
extern int bsbExecute (char* filename, struct bsbstack* st);



// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN
// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN



int main (int argv, char** argc) {
  struct bsbstack* st;

  if (argv == 1) {
    printf ("error: input file name\n");
    return 0;
  }

  st = createBsbstack ();

  if (st != NULL) {
    bsbExecute (argc[1], st);
    destroyBsbstack (st);
  }

  return 0;
}
