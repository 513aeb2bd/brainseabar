#include <stdlib.h>

#include "bsbmain.h"

// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN

struct bsbstack* createBsbstack ();
i32 destroyBsbstack (struct bsbstack* st);
i32 checkBsbstackSize (struct bsbstack* st);



// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN
// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN



struct bsbstack* createBsbstack () {
  struct bsbstack* st = (struct bsbstack*)malloc (sizeof (struct bsbstack));

  if (st == NULL) {
    return NULL;
  }

  st->sp1 = st->sp0 = 0;
  st->spmax1 = st->spmax0 = STACKSIZE_INIT;
  st->st0 = (ui8*)malloc (STACKSIZE_INIT);

  if (st->st0 == NULL) {
    free (st);
    return NULL;
  }

  st->st1 = (ui8*)malloc (STACKSIZE_INIT);

  if (st->st1 == NULL) {
    free (st);
    return NULL;
  }

  return st;
}



// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN
// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN



i32 destroyBsbstack (struct bsbstack* st) {
  if (st == NULL) {
    return 0;
  }

  if (st->st0 != NULL) {
    free (st->st0);
  }

  if (st->st1 != NULL) {
    free (st->st1);
  }

  free (st);
  return 0;
}



// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN
// MNMNMN MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMNM MNMNMN
// MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN MNMNMNMN



i32 checkBsbstackSize (struct bsbstack* st) {
  if (st == NULL) {
    return -1;
  }

  if (st->sp0 == st->spmax0) {
    if (st->sp0 == STACKSIZE_MAX) {
      return -1;
    }

    st->spmax0 <<= 1;
    st->st0 = (ui8*)realloc (st->st0, st->spmax0);
  }

  if (st->sp1 == st->spmax1) {
    if (st->sp1 == STACKSIZE_MAX) {
      return -1;
    }

    st->spmax1 <<= 1;
    st->st1 = (ui8*)realloc (st->st1, st->spmax1);
  }

  return 0;
}
