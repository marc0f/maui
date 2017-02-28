/*
Package:  moab
Module:   MAcct.c
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
 
/* Contains:                                         *
 *  int MAcctInitialize(A,AName)                     *
 *  int MAcctFind(A,AccountName)                     *
 *                                                   */

#include "moab.h"
#include "msched-proto.h"

extern mlog_t    mlog;
 
extern msched_t  MSched;
extern mgcred_t   MAcct[];
extern mckpt_t   MCP;     

extern const char *MCredAttr[];
extern const char *MXO[];
extern const char *MJobFlags[];





int MAcctLoadCP(
 
  mgcred_t *AS,
  char    *Buf)
 
  {
  char    tmpHeader[MAX_MNAME];
  char    AName[MAX_MNAME];
 
  char   *ptr;
 
  mgcred_t *A;
 
  long    CkTime;
 
  mxml_t *E = NULL;
 
  const char *FName = "MAcctLoadCP";
 
  DBG(4,fCKPT) DPrint("%s(AS,%s)\n",
    FName,
    (Buf != NULL) ? Buf : "NULL");
 
  if (Buf == NULL)
    return(FAILURE);
 
  /* FORMAT:  <HEADER> <GID> <CKTIME> <GSTRING> */
 
  /* load CP header */
 
  sscanf(Buf,"%s %s %ld",
    tmpHeader,
    AName,
    &CkTime);
 
  if (((long)MSched.Time - CkTime) > MCP.CPExpirationTime) 
    return(SUCCESS);
 
  if (AS == NULL)
    {
    if (MAcctAdd(AName,&A) != SUCCESS)
      {
      DBG(5,fCKPT) DPrint("ALERT:    cannot load CP account '%s'\n",
        AName);
 
      return(FAILURE);
      }
    }
  else
    {
    A = AS;
    }
 
  if ((ptr = strchr(Buf,'<')) == NULL)
    {
    return(FAILURE);
    }
 
  MXMLFromString(&E,ptr,NULL,NULL);
 
  MOFromXML((void *)A,mxoAcct,E);
 
  MXMLDestroyE(&E);
 
  return(SUCCESS);
  }  /* END MAcctLoadCP() */ 
 
 
 
 
int MAcctAdd(
 
  char     *AName,  /* I */
  mgcred_t **A)      /* O (optional) */
 
  {
  int   aindex;
 
  int   Key;
 
  mgcred_t *Atmp;

  const char *FName = "MAcctAdd";

  DBG(5,fSTRUCT) DPrint("%s(%s,A)\n",
    FName,
    (AName != NULL) ? AName : "NULL");
 
  if (A != NULL)
    *A = NULL;

  if ((AName == NULL) || (AName[0] == '\0'))
    {
    return(FAILURE);
    }
 
  Key = (int)(MUGetHash(AName) % MAX_MACCT);
 
  for (aindex = Key;aindex < MAX_MACCT + MAX_MHBUF;aindex++)
    {
    Atmp = &MAcct[aindex];
 
    /* if account already in table */
 
    if (!strcmp(Atmp->Name,AName))
      {
      if (A != NULL)
        *A = Atmp;
 
      return(SUCCESS);
      }
 
    /* if empty slot found */
 
    if (Atmp->Name[0] == '\0')
      { 
      /* setup new record */
 
      if (A != NULL)
        *A = Atmp;
 
      if (!strcmp(AName,NONE))
        Atmp->Key = 0;
      else
        Atmp->Key = Key;
 
      MUStrCpy(Atmp->Name,AName,sizeof(Atmp->Name));
 
      Atmp->Index = aindex;
 
      if (strcmp(AName,ALL) && strcmp(AName,NONE))
        {
        /* update account record */
 
        if (MSched.Mode != msmSim)
          MCPRestore(mcpAcct,Atmp->Name,(void *)Atmp);
 
        DBG(5,fSTRUCT) DPrint("INFO:     account %s added\n",
          AName);
        }
      else
        {
        /* do nothing */
        }
 
      return(SUCCESS);
      }
    }    /* END for (aindex) */
 
  /* end of table reached */
 
  DBG(1,fSTRUCT) DPrint("ALERT:    account table overflow  (cannot add account %s)\n",
    AName); 
 
  return(FAILURE);
  }  /* END MAcctAdd() */





int MAcctFind(

  char     *AccountName,
  mgcred_t **A)

  {
  /* If found, return success with A pointing to Account.     */
  /* If not found, return failure with A pointing to          */
  /* first free Account if available, A set to NULL otherwise */

  int  aindex;
  int  Key;

  char AName[MAX_MNAME];    

  DBG(5,fSTRUCT) DPrint("MAcctFind(%s,A)\n",
    (AccountName == NULL) ? "NULL" : AccountName);

  if (A != NULL)
    *A = NULL;

  if ((AccountName == NULL) || (AccountName[0] == '\0'))
    strcpy(AName,NONE);
  else
    MUStrCpy(AName,AccountName,sizeof(AName));

  Key = (int)(MUGetHash(AName) % MAX_MACCT);   

  for (aindex = Key;aindex < MAX_MACCT + MAX_MHBUF;aindex++)
    {
    if (MAcct[aindex].Name[0] == '\0')
      {
      if (A != NULL)
        *A = &MAcct[aindex];

      break;
      }

    if (strcmp(MAcct[aindex].Name,AName) != 0)
      continue;

    /* Account found */

    if (A != NULL)
      *A = &MAcct[aindex];

    return(SUCCESS);
    }  /* END for (aindex) */

  return(FAILURE);
  }  /* END MAcctFind() */




int MAcctInitialize(

  mgcred_t *A,
  char    *AName)

  {
  if ((A == NULL) || 
      (AName == NULL) || 
      (AName[0] == '\0'))
    {
    return(FAILURE);
    }

  memset(A,0,sizeof(mgcred_t));

  MUStrCpy(A->Name,AName,sizeof(A->Name));

  return(SUCCESS);
  }  /* END MAcctInitialize() */




int MAcctToXML(
 
  mgcred_t  *A,
  mxml_t **EP,
  int      *SAList)
 
  {
  int DAList[] = {
    mcaID,
    -1 };
 
  int  aindex;
 
  int *AList;
 
  char tmpString[MAX_MLINE];
 
  if ((A == NULL) || (EP == NULL))
    return(FAILURE);
 
  if (SAList != NULL)
    AList = SAList;
  else
    AList = DAList; 

  MXMLCreateE(EP,(char *)MXO[mxoAcct]);         
 
  for (aindex = 0;AList[aindex] != -1;aindex++)
    {
    if ((MCredAToString((void *)A,mxoAcct,AList[aindex],tmpString,0) == FAILURE) ||
        (tmpString[0] == '\0'))
      {
      continue;
      }
 
    MXMLSetAttr(*EP,(char *)MCredAttr[AList[aindex]],tmpString,mdfString);
    }  /* END for (aindex) */
 
  return(SUCCESS);
  }  /* END MAcctToXML() */




int MAcctToString(
 
  mgcred_t *A,
  char    *Buf)
 
  {
  const int CPCList[] = {
    mxoStats,
    -1 };
 
  mxml_t *E = NULL;
 
  if ((A == NULL) || (Buf == NULL))
    return(FAILURE);

  Buf[0] = '\0';     
 
  MCOToXML((void *)A,mxoAcct,&E,NULL,(int *)CPCList,0);    
 
  MXMLToString(E,Buf,MAX_MBUFFER,NULL,TRUE);
 
  MXMLDestroyE(&E);
 
  return(SUCCESS);
  }  /* END MAcctToString() */




char *MAcctShow(

  mgcred_t *A,       /* I */
  char     *Buf,     /* O */
  long     *BufSize,
  long      Mode)

  {
  static char Line[MAX_MLINE];

  char FlagLine[MAX_MLINE];

  char QALLine[MAX_MLINE];
  char QALChar;

  char ALLine[MAX_MLINE];

  char *ptr;

  const char *FName = "MAcctShow";

  DBG(3,fUI) DPrint("%s(%s,Buf,BufSize,%ld)\n",
    FName,
    (A != NULL) ? A->Name : "NULL",
    Mode);

  ptr = (Buf != NULL) ? Buf : Line;

  if (A == NULL)
    {
    /* build header */

    /*            NAME PRI FLAG QDEF QLST * PLST TRG LIMITS */

    sprintf(ptr,"%-12s %8s %12s %12s %12s%s %20s %6s %7s\n\n",
      "Name",
      "Priority",
      "Flags",
      "QDef",
      "QOSList",
      "*",
      "PartitionList",
      "Target",
      "Limits");
    }
  else
    {
    /* build job info line */

    MUBMToString(A->F.JobFlags,MJobFlags,':',FlagLine,NONE);

    MUStrCpy(QALLine,MQOSBMToString(A->F.QAL),sizeof(QALLine));

    if (A->F.QALType == qalAND)
      QALChar = '&';
    else if (A->F.QALType == qalONLY)
      QALChar = '^';
    else
      QALChar = ' ';

    MUStrCpy(ALLine,
      MCredShowAttrs(&A->L.AP,A->L.IP,NULL,NULL,NULL,&A->F,0,(1 << mcsLimits)),sizeof(ALLine));

    /*            NAME PRIO FLAG QDEF QLST * PLST FSTARG LIMITS */

    sprintf(ptr,"%-12s %8ld %12s %12s %12s%c %20s %6.2lf %7s\n",
      A->Name,
      A->F.Priority,
      FlagLine,
      ((mqos_t *)A->F.QDef) != NULL ?
        ((mqos_t *)A->F.QDef)->Name  :
        NONE,
      (QALLine[0] != '\0') ? QALLine : NONE,
      QALChar,
      (A->F.PAL[0] == 0) ?
        NONE :
        MUListAttrs(ePartition,A->F.PAL[0]),
      A->F.FSTarget,
      (ALLine[0] != '\0') ? ALLine : NONE);
    }  /* END else (A == NULL) */

  return(ptr);
  }  /* END MAcctShow() */

/* END MAcct.c */

