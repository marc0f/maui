/*
Package:  moab
Module:   MAppSim.c
Version:  3.3.1

Copyright:

  Copyright (C) 1999-2010 Cluster Resources, Inc

  All Rights Reserved

-----------------------------------------------------------------------------
Moab Workload Manager - End User License

This product was created by Cluster Resources, Inc. and is protected by
copyright and patent law.

Copyright (C) 1999-2010 Cluster Resources, Inc., all rights reserved.
Moab Workload Manager, Moab Cluster Scheduler and Moab Scheduler
are trademarks of Cluster Resources, Inc.

This SOFTWARE is bound by an 'End User Subscription License' (LICENSE) from
Cluster Resources Inc.  The conditions of the 'End User Subscription
License' (LICENSE) include, but are not limited to, the conditions described
below.

THE SOFTWARE IS PROVIDED AS IS, AND CLUSTER RESOURCES, INC. (CRI) AND ALL
CONTRIBUTING PARTIES DISCLAIM ALL WARRANTIES RELATING TO THE SOFTWARE,
WHETHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  NEITHER
CRI NOR ANYONE INVOLVED IN THE CREATION, PRODUCTION, OR DELIVERY OF THE
SOFTWARE SHALL BE LIABLE FOR ANY INDIRECT, CONSEQUENTIAL, OR INCIDENTAL
DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE SOFTWARE EVEN IF CRI
HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES OR CLAIMS.  IN NO EVENT
SHALL CRI'S LIABILITY FOR ANY DAMAGES EXCEED THE CONSIDERATION PAID FOR THE
LICENSE TO USE THE SOFTWARE, REGARDLESS OF THE FORM OF CLAIM.  THE PERSON OR
ENTITY USING THE SOFTWARE BEARS ALL RISK AS TO THE QUALITY AND PERFORMANCE
OF THE SOFTWARE.

By installing or using this SOFTWARE you are accepting a non-exclusive 'End
User Subscription License' (LICENSE) from Cluster Resources Inc. and are
bound to abide by the following conditions:

1.  Inclusion of Notice and Disclaimer

  All copies of the SOFTWARE, whether or not for redistribution and whether
or not in source code or in binary form must include a conspicuous and
appropriate publication of the above copyright notice and disclaimer.

2. Usage

  Binary forms of this SOFTWARE may be used by any 'End User' organization
pursuant to the conditions of this and other associated LICENSES for the
duration of the subscription license granted.  An 'End User' organization is
defined as an organization that is using this SOFTWARE on their own systems
and is not commercially redistributing, modifying, supporting, or providing
other services specific to this SOFTWARE to other organizations for profit.

3.  Modifications

  'End User' may solicit the services of Cluster Resources Inc. of Cluster
Resources Inc. to modify or provide services for this SOFTWARE. Available
services include but are not limited to technical support, training,
consultation or optimization services. Any unauthorized partner that desires
to become an 'Authorized Distribution and Services Partner' of Cluster
Resources, Inc. may contact us at support.com.  'End User'
organizations that desire services from Cluster Resources Inc., or an
'Authorized Distribution and Services Partner' may contact us using the same
email listed above.

4.  Distribution

  No redistribution of this product is permitted without express prior
written permission.

5. Communications about and Endorsement of SOFTWARE and Products/Software
Derived from the SOFTWARE

  The name 'Moab Scheduling System', 'Moab Cluster Scheduler', 'Moab Scheduler',
'Moab Workload Manager' or any of its variants must not otherwise be used to
endorse or to promote products derived from the SOFTWARE without prior written
permission from CRI.

  Products derived from or incorporating the SOFTWARE in whole or in part
shall not contain as part of the product's name any form of the terms 'Cluster
Resources, Inc.', 'CRI', 'Moab', 'Moab Scheduling System', 'Moab Cluster
Scheduler', 'Moab Scheduler', 'Moab Workload Manager', or 'Supercluster
Development Group' unless prior written permission has been received from
Cluster Resources, Inc.

  All advertising materials for products that use or incorporate features of
the SOFTWARE must display the following acknowledgement: 'This product
includes software developed by Cluster Resources, Inc. for use in the Moab
Scheduling System.'

6.  Acceptance of this LICENSE

  It is not required that you accept this LICENSE; however, if you do not
accept the terms of this LICENSE, you are prohibited by law from installing,
using, modifying or distributing the SOFTWARE or any of its derivative works.
Therefore, by installing, using, modifying or distributing the SOFTWARE (or
any of its derivative works), you have agreed to this LICENSE and have
accepted all its terms and conditions.

  If any portion of this LICENSE is held invalid or unenforceable under any
particular circumstance, the balance of the LICENSE will continue to apply.

------------------------------------------------------------------------------
*/
        
/* Contains:                                 *
 *                                           */


#include "moab.h"
#include "moab-proto.h"

extern mlog_t   mlog;
extern mjob_t  *MJob[];
extern mpar_t   MPar[];
extern msched_t MSched;

#ifdef __MAPPSIM
# include "../contrib/appsim/LocalStage.c"
# include "../contrib/appsim/Net1.c"
#endif /* __MAPPSIM */

/* prototypes */

int MASDefault(mjob_t *,int,void *,void **);
int MASDefaultCreate(mjob_t *,char *);
int MASDefaultShow(mjob_t *,char *);
int MASDefaultDestroy(mjob_t *);
int MASDefaultUpdate(mjob_t *);
int MASDefaultConfig(mjob_t *,char *);



/* structures */

/* #include "../contrib/appsim/Benchmark.c" */

typedef struct {
  char  *Name;
  int  (*Func)();
  int    Type;
  int    Default;
  } asim_t;

asim_t SimDriver[] = {
  { DEFAULT,      (int (*)())MASDefault,    msdApplication, TRUE },
#ifdef __MAPPSIM
  { "LocalStage", (int (*)())MASLocalStage, msdApplication, FALSE },
  { "Net1",       (int (*)())MASNet1,       msdResource,    TRUE },
#endif /* __MAPPSIM */     
  { NULL,         NULL,                 0,              FALSE }, 
  };

typedef struct {
  char *Name;
  int   ASState;
  } masdefdata_t;



const char *MASDefAttributeType[] = {
  NONE,
  "STATE",
  NULL };

enum MASDefAttrEnum {
  masdefaNONE = 0,
  masdefaState };





int MGResFind(

  char    *ResName,
  int      ResType,
  xres_t **RPtr)

  {
  int rindex;

  xres_t *R;

  for (rindex = 0;rindex < MAX_MNODE;rindex++)
    {
    R = &MPar[0].XRes[rindex];

    if (R->Name[0] == '\1')
      continue;

    if (R->Name[0] == '\0')
      break;

    if ((ResType != -1) && (ResType != R->Type))
      {
      continue;
      }

    if ((ResName != NULL) && 
        (ResName[0] == '\0') && 
        !strcmp(R->Name,ResName))
      {
      continue;
      }

    if (RPtr != NULL)
      {
      *RPtr = R;
      }

    return(SUCCESS);
    }  /* END for (rindex) */  

  return(FAILURE);
  }  /* END MGResFind() */





int MASGetDriver(

  void   **D,
  char    *DName,
  int      DType)

  {
  int findex;

  if (D == NULL)
    {
    return(FAILURE);
    }

  *D = NULL;

  if (DType == 0)
    {
    return(FAILURE);
    }

  /* determine sim driver to use */

  for (findex = 0;SimDriver[findex].Name != NULL;findex++)
    {
    if ((DType != -1) && (SimDriver[findex].Type != DType) )
      {
      continue;
      }

    if ((SimDriver[findex].Default == TRUE) && (*D == NULL))
      {
      /* load default if found */

      *D = (void *)SimDriver[findex].Func;
      }

    if ((DName == NULL) || (DName[0] == '\0')) 
      {
      /* driver name not specified, only default used */

      continue;
      }
      
    if (!strcmp(DName,SimDriver[findex].Name))
      {
      /* match found */

      *D = (void *)SimDriver[findex].Func;  
     
      break;
      }
    }    /* END for (findex) */

  if (*D == NULL)
    {
    /* default driver not located */
 
    return(FAILURE);
    }

  return(SUCCESS);
  }  /* END MASGetDriver() */



 
char *MASGetName(

  void *D)

  {
  int findex;

  if (D != NULL)
    { 
    for (findex = 0;SimDriver[findex].Name != NULL;findex++)
      {
      if (D == (void *)SimDriver[findex].Func)
        break;
      }

    if (SimDriver[findex].Name == NULL)
      findex = 0;
    }
  else
    {
    findex = 0;
    }

  return(SimDriver[findex].Name);
  }  /* END MASGetName() */




int MASDefault(

  mjob_t  *J,
  int      CmdIndex,
  void    *IData,
  void   **OData)

  {
  int rc;

  if (J == NULL)
    return(FAILURE);

  switch(CmdIndex)
    {
    case mascCreate:

      rc = MASDefaultCreate(J,(char *)IData);

      break;

    case mascUpdate:

      rc = MASDefaultUpdate(J);

      break;

    case mascDestroy:

      rc = MASDefaultDestroy(J);

      break;

    case mascConfig:

      rc = MASDefaultConfig(J,(char *)IData);

      break;

    case mascShow:

      rc = MASDefaultShow(J,(char *)OData);

      break;

    default:

      rc = FAILURE;

      break;
    }  /* END switch(CmdIndex) */

  return(rc);
  }  /* END MASDefault() */




int MASDefaultCreate(

  mjob_t *J,
  char   *ConfigString)

  {
  char *ptr;
  char *TokPtr;

  char *ptr2;
  char *TokPtr2;

  char  ValLine[MAX_MLINE];
  
  int   aindex;

  mreq_t *RQ;

  masdefdata_t *D;

  if (J == NULL)
    return(FAILURE);

  /* NOTE:  by default, each job uses 100% of all dedicated resources */          

  /* set 'RQ->URes.Procs' */
  /* set 'RQ->URes.Mem'   */

  /* create AS data structure */

  if (J->ASData == NULL)
    {
    J->ASData = calloc(1,sizeof(masdefdata_t));
    }

  D = (masdefdata_t *)J->ASData;

  if (ConfigString != NULL)
    {
    /* process config data */

    /* FORMAT:  INPUT=<INPUTDATANAME>:<INPUTDATASIZE>;OUTPUT=<OUTPUTDATANAME>:<OUTPUTDATASIZE> */

    ptr = MUStrTok(ConfigString,"; \t\n",&TokPtr);

    while (ptr != NULL)
      {
      if (MUGetPair(
            ptr,
            (const char **)MASDefAttributeType,
            &aindex,
	    NULL,
            TRUE,
            NULL,
            ValLine,
            MAX_MNAME) == FAILURE)
        {
        /* cannot parse value pair */
 
        ptr = MUStrTok(NULL,"; \t\n",&TokPtr);
 
        continue;
        }

      switch(aindex)
        {
        default:
 
          break;
        }
      }  /* END while (ptr != NULL) */

    ptr = MUStrTok(NULL,"; \t\n",&TokPtr);     
    }    /* END if (ConfigString != NULL) */

  /* clear resource usage */

  RQ = J->Req[0];

  memset(&RQ->URes,0,sizeof(RQ->URes));

  return(SUCCESS);
  }  /* END MASDefaultCreate() */





int MASDefaultShow(
 
  mjob_t *J,
  char   *Buffer)
 
  {
  if ((J == NULL) || (Buffer == NULL))
    {
    return(FAILURE);
    }
 
  Buffer[0] = '\0';
 
  return(SUCCESS);
  }  /* END MASDefaultShow() */




int MASDefaultConfig(
 
  mjob_t *J,
  char   *ConfigString)
 
  {
  char *ptr;
  char *TokPtr;
 
  char *ptr2;
  char *TokPtr2;
 
  char  ValLine[MAX_MLINE];
 
  int   aindex;
  int   index;
 
  mreq_t *RQ;
 
  masdefdata_t *D;
 
  if ((J == NULL) || (ConfigString == NULL))
    {
    return(FAILURE);
    }
 
  D = (masdefdata_t *)J->ASData;      
 
  if (D == NULL)
    {
    return(FAILURE);
    }

  /* process config data */
 
  /* FORMAT:  STATE=<STATE>; */
 
  ptr = MUStrTok(ConfigString,"; \t\n",&TokPtr);
 
  while (ptr != NULL)
    {
    if (MUGetPair(
          ptr,
          (const char **)MASDefAttributeType,
          &aindex,
	  NULL,
          TRUE,
          NULL,
          ValLine,
          MAX_MNAME) == FAILURE)
      {
      /* cannot parse value pair */
 
      ptr = MUStrTok(NULL,"; \t\n",&TokPtr);
 
      continue;
      }
 
    switch(aindex)
      {
      case masdefaState:
 
        index = MUGetIndex(ValLine,MJobState,0,0);
 
        switch(index)
          {
          case mjsStarting:
          case mjsRunning:
 
            if ((D->ASState != mjsStarting) && (D->ASState != mjsRunning))
              {
              D->ASState = index;
              }
 
            break;
 
          default:
 
            break;
          }
 
        break;
 
      default:
 
        break;
      }  /* END switch(aindex) */
 
    ptr = MUStrTok(NULL,"; \t\n",&TokPtr);
    }  /* END while (ptr != NULL) */
 
  return(SUCCESS);
  }  /* END MASDefaultConfig() */




int MASDefaultDestroy(
 
  mjob_t *J)
 
  {
  masdefdata_t *D;
 
  J->ASFunc = NULL;
 
  /* clear AS Data */
 
  if (J->ASData != NULL)
    {
    D = (masdefdata_t *)J->ASData;
 
    MUFree((char **)J->ASData);
    }
 
  return(SUCCESS);
  }  /* END MASDefaultDestroy() */





int MASDefaultUpdate(

  mjob_t       *J)

  {
  long Now;

  mreq_t *RQ;

  masdefdata_t *D;

  if (J == NULL)
    return(FAILURE);

  D = (masdefdata_t *)J->ASData;

  if (D == NULL)
    return(FAILURE);

  RQ = J->Req[0];

  /* default app sim jobs utilize 100% of dedicated resources */

  switch (J->State)
    {
    case mjsRunning:
    case mjsStarting:

      /* job is active */          

      /* job data is static */

      if ((J->State != D->ASState) || 
          (RQ->URes.Procs != RQ->DRes.Procs * 100))
        {
        RQ->URes.Procs = RQ->DRes.Procs * 100;
        RQ->URes.Mem   = RQ->DRes.Mem;
        RQ->URes.Swap  = RQ->DRes.Swap;
        RQ->URes.Disk  = RQ->DRes.Disk;

        RQ->LURes.Procs = RQ->DRes.Procs * 100;
        RQ->LURes.Mem   = RQ->DRes.Mem;
        RQ->LURes.Swap  = RQ->DRes.Swap;
        RQ->LURes.Disk  = RQ->DRes.Disk;

        D->ASState = J->State;
        }

      break;
       
    case mjsIdle:

      /* job is idle */

      /* job data is static */

      if (J->State != D->ASState)      
        {
        memset(&RQ->URes,0,sizeof(RQ->URes));

        D->ASState = J->State;    
        }
 
      break;
 
    case mjsCompleted:
    case mjsRemoved:

      /* job is completed */

      /* job data is static */

      if (J->State != D->ASState)
        {
        memset(&RQ->URes,0,sizeof(RQ->URes));

        D->ASState = J->State;   
        }

      break;
 
    default:

      break;
    }  /* END switch(J->State) */

  return(SUCCESS);
  }  /* END MASDefaultUpdate() */



/* END MAppSim.c */


