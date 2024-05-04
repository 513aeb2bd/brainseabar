#include "main.h"

#include <stdlib.h>

struct bsbstack* createBsbstack ();
id destroyBsbstack (struct bsbstack *st);
id checkBsbstackSize (struct bsbstack *st);

// .

// .

struct bsbstack *
createBsbstack ()
{
   struct bsbstack *st;

   st = malloc (sizeof *st);

   if (st == NULL)  return NULL;

   st->sp1 = st->sp0 = 0;
   st->spmax1 = st->spmax0 = STACKSIZE_INIT;
   st->st0 = malloc (STACKSIZE_INIT);

   if (st->st0 == NULL)  goto bran_error_exit;

   st->st1 = malloc (STACKSIZE_INIT);

   if (st->st1 == NULL)  goto bran_error_exit;

   return st;

bran_error_exit:

   free (st);
   return NULL;
}   // endfunc: struce bsbstack *create-bsbstack

// .

// .

id
destroyBsbstack (struct bsbstack *st)
{
   if (st == NULL)  return 0;

   if (st->st0 != NULL) {
      free (st->st0);
   }

   if (st->st1 != NULL) {
      free (st->st1);
   }

   free (st);
   return 0;
}   // endfunc: id destroy-bsbstack

// .

// .

id
checkBsbstackSize (struct bsbstack *st)
{
   if (st == NULL)  return -1;

   if (st->sp0 == st->spmax0) {
      if (st->sp0 == STACKSIZE_MAX)  return -1;

      st->spmax0 <<= 1;
      st->st0 = realloc (st->st0, st->spmax0);
   }

   if (st->sp1 == st->spmax1) {
      if (st->sp1 == STACKSIZE_MAX)  return -1;

      st->spmax1 <<= 1;
      st->st1 = realloc (st->st1, st->spmax1);
   }

   return 0;
}   // endfunc: id check-bsbstack-size
