/*
Package:  moab
Module:   MXI.c
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
#include "msched-proto.h"  

extern mlog_t      mlog;

extern mjob_t     *MJob[];
extern mnode_t    *MNode[];

extern mqos_t      MQOS[];
extern mrm_t       MRM[];
extern mam_t       MAM[];

extern mattrlist_t MAList;
extern mrange_t    MRange[];
extern msched_t    MSched;
extern mstat_t     MStat;
extern msim_t      MSim;

extern mgcred_t   *MUser[];
extern mgcred_t    MGroup[];
extern mgcred_t    MAcct[];
extern mres_t     *MRes[];
extern sres_t      SRes[];
extern sres_t      OSRes[];

extern mpar_t      MPar[];

extern long        CREndTime;
extern char        CurrentHostName[];

/* prototypes */

extern int XInitializeMInterface(void *,char *,int *,char **,char *);

extern int LL2ShowError(int,mjob_t *);
extern int LL2FreeData(mrm_t *);


int XInitialize(

  mx_t  *X,
  char  *Version,
  int   *ArgC,
  char **ArgV,
  char  *HomeDir)

  {
# include "MXConfig.c"

# ifdef __MX
  /* enable XInitialize when MG2/MSU are linked in */

  /* XInitializeMInterface(X,Version,ArgC,ArgV,HomeDir); */
# else
  memset(X,0,sizeof(mx_t));
# endif /* __MX */

  return(SUCCESS);
  }  /* XInitialize() */




int XUIJobCtl()

  {  
  const char *FName = "XUIJobCtl";

  DBG(1,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XUIJobCtl() */



int XUIMetaCtl()

  {  
  const char *FName = "XMetaCtl";

  DBG(1,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XUIMetaCtl() */




int XUIResCtl()

  {  
  const char *FName = "XUIResCtl";

  DBG(1,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XUIResCtl() */




int XRMInitialize(

  void  *X,
  mrm_t *R)

  {
  const char *FName = "XRMInitialize";
 
  DBG(1,fALL) DPrint("INFO:     %s not supported\n",
    FName);
 
  return(FAILURE);
  }  /* END XRMInitialize() */




int XPBSNMGetData(

  void    *X,
  mnode_t *N,
  mrm_t   *R)
 
  {
  const char *FName = "XPBSNMGetData";
 
  DBG(1,fALL) DPrint("INFO:     %s not supported\n",
    FName);
 
  return(FAILURE);
  }  /* END XPBSNMGetData() */





int XRMJobResume()

  {  
  const char *FName = "XRMJobResume";

  DBG(1,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XRMJobResume() */




int XRMJobSuspend()

  {  
  const char *FName = "XRMJobSuspend";

  DBG(1,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XRMJobSuspend() */





int XUpdateState()

  {
  const char *FName = "XUpdateState";

  DBG(1,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XUpdateState() */




int XShowConfig(

  void *X,
  char *Buffer)

  {
  /* do nothing */

  return(SUCCESS);
  }  /* END XShowConfig() */




int XAllocMachinePrio()

  {
  const char *FName = "XAllocMachinePrio";

  DBG(4,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XAllocMachinePrio() */



int XAllocLoadBased()

  {
  const char *FName = "XAllocLoadBased";

  DBG(4,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XAllocLoadBased() */



int XLL2JobLoad(

  void   *X,
  mjob_t *J,
  void   *LLStep)

  {
  const char *FName = "XLL2JobLoad";

  DBG(6,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XLL2JobLoad() */



int XLL2JobUpdate(

  void   *X,
  mjob_t *J,
  void   *LLStep)

  {
  const char *FName = "XLL2JobUpdate";

  DBG(6,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XLL2JobUpdate() */




int XLL2NodeLoad(

  void    *X,
  mnode_t *N,
  void    *LLMachine)

  {
  const char *FName = "XLL2NodeLoad";

  DBG(6,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XLL2NodeLoad() */



int XLL2NodeUpdate(

  void    *X,
  mnode_t *N,
  void    *LLMachine)

  {
  const char *FName = "XLL2NodeUpdate";

  DBG(6,fALL) DPrint("INFO:     %s not supported\n",
    FName);

  return(FAILURE);
  }  /* END XLL2NodeUpdate() */




int XRMResetState(

  void  *X,
  mrm_t *SpecR)

  {
  const char *FName = "XRMResetState";

  int rmindex;

  mrm_t *R;

  DBG(8,fALL) DPrint("%s(%s)\n",
    FName,
    (SpecR != NULL) ? SpecR->Name : "NULL");

  for (rmindex = 0;rmindex < MAX_MRM;rmindex++)
    {
    R = &MRM[rmindex];

    switch (R->Type)
      {
      case mrmtLL:

        LL2FreeData(R);

        break;

      default:

        break;
      }  /* END switch(tmpR->Type) */
    }    /* END for (rmindex)      */

  return(FAILURE);
  }  /* END XRMResetState() */




int XRMVerifyData(

  void  *X,
  mrm_t *R,
  char  *DataType)

  {
  return(FAILURE);
  }  /* END XRMVerifyData() */



int XPBSInitialize(

  void *X,
  mrm_t *R)

  {
  return(FAILURE);
  }  /* END XPBSInitialize() */



int XUIHandler(

  void      *X,
  msocket_t *S,
  char      *CSKey,
  int        Mode)

  {
  return(FAILURE);
  }  /* END XUIHandler() */



int XGetClientInfo(

  void      *X,
  msocket_t *S,
  char      *CInfo)

  {
  strcpy(S->CSKey,MSched.DefaultCSKey);
  S->Version      = 0;
 
  return(SUCCESS);
  }  /* END XGetClientInfo() */



int XJobProcessWikiAttr(

  void   *X,
  mjob_t *J,
  char   *Tok)

  {
  return(FAILURE);
  }  /* END XJobProcessWikiAttr() */



int XJobDestroy(

  void    *X,
  mjob_t **J,
  int      SC)

  {
  return(SUCCESS);
  }  /* END XJobDestroy() */


/* END MXI.c */

