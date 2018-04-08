/***************************************************************************//**
@file         main.c
@author       Stephen Brennan
@date         Thursday,  8 January 2015
@brief        LSH (Libstephen SHell)
*******************************************************************************/
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "funciones.h"

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  /* Load config files, if any.*/

  /* Run command loop.*/
  lsh_loop();

  /*Perform any shutdown/cleanup.*/

  return EXIT_SUCCESS;
}
