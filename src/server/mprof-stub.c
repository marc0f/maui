/*
Package:  maui
Module:   mprof-stub.c
Version:  3.3.1

Copyright:

  Copyright (C) 1999-2010 Cluster Resources, Inc

  All Rights Reserved

Disclaimer:

  

*/

#include "mclient-stub.c"

        
int JobPReserve(

  mjob_t *J,
  int     PIndex,
  int    *ResCount)

  {
  return(SUCCESS);
  }

int UHProcessRequest(

  msocket_t *S,
  char     *RBuffer)

  {
  return(SUCCESS);
  }  /* END UHProcessRequest() */

int UIQueueShowAllJobs(char *S,long *SS,mpar_t *P,char *U) { return(SUCCESS); }

/* END mprof-stub.c */

