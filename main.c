#include "main.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

// TODO: proc-bsbcode.c: proc-bsbcode: add dynamic stack size
// TODO: modify source code files in ./src/

// #####################################################################
// #####################################################################

extern id prep_bsbcode (char *filename, ib **o_inst, ib ***o_paren);   // prep-bsbcode.c
extern id proc_bsbcode (ib *inst, ib **paren);   // proc-bsbcode.c

// #####################################################################
// #####################################################################

id
main (id argc, char *argv[])
{
   ib **paren;
   ib *inst;
   char *filename;

   if (argc == 1) {
      printf ("error: input file name\n");
      return 0;
   }

   paren = NULL;
   inst = NULL;
   filename = argv[1];

   if (!prep_bsbcode (filename, &inst, &paren))  return 0;

   if (!proc_bsbcode (inst, paren)) {
      printf ("stack overflow/underflow detected\n");
   }

   free (inst);
   free (paren);
   return 0;
}   // endfunc: id main
