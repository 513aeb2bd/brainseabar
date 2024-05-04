#include "main.h"

#include <stdio.h>

// bsbstack.c
extern struct bsbstack *createBsbstack ();
extern id destroyBsbstack (struct bsbstack* st);
// brainseabar.c
extern id bsbExecute (char *filename, struct bsbstack *st);

// .

// .

id
main (id argc, char *argv[])
{
   struct bsbstack *st;

   if (argc == 1) {
      printf ("error: input file name\n");
      return 0;
   }

   st = createBsbstack ();

   if (st != NULL) {
      bsbExecute (argv[1], st);
      destroyBsbstack (st);
   }

   return 0;
}
