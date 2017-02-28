/*
Package:  moab
Module:   MGroup.c
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
 *  int GroupInitialize(G,GroupName)                 *
 *  int GroupFind(G,GroupName)                       *
 *                                                   */

#include "moab.h"
#include "msched-proto.h"  

extern mlog_t    mlog;
 
extern msched_t  MSched;
extern mgcred_t  MGroup[];
extern mckpt_t   MCP;     
extern mclass_t  MClass[];
extern mqos_t    MQOS[];

extern const char *MCredAttr[];
extern const char *MGroupAttr[];
extern const char *MXO[];
extern const char *MJobFlags[];


/*
 * HvB 
*/
extern int FSGroupKeys[];

int MGroupLoadCP(
 
  mgcred_t *GS,  /* I */
  char     *Buf) /* O */
 
  {
  char    tmpHeader[MAX_MNAME];
  char    GName[MAX_MNAME];
 
  char   *ptr;
 
  mgcred_t *G;
 
  long    CkTime;
 
  mxml_t *E = NULL;
 
  const char *FName = "MGroupLoadCP";
 
  DBG(4,fCKPT) DPrint("%s(GS,%s)\n",
    FName,
    (Buf != NULL) ? Buf : "NULL");
 
  if (Buf == NULL)
    {
    return(FAILURE);
    }
 
  /* FORMAT:  <HEADER> <GID> <CKTIME> <GSTRING> */
 
  /* load CP header */
 
  sscanf(Buf,"%s %s %ld",
    tmpHeader,
    GName,
    &CkTime);
 
  if (((long)MSched.Time - CkTime) > MCP.CPExpirationTime)
    {
    return(SUCCESS);
    }
 
  if (GS == NULL)
    {
    if (MGroupAdd(GName,&G) != SUCCESS)
      {
      DBG(5,fCKPT) DPrint("ALERT:    cannot load CP group '%s'\n", 
        GName);
 
      return(FAILURE);
      }
    }
  else
    {
    G = GS;
    }
 
  if ((ptr = strchr(Buf,'<')) == NULL)
    {
    return(FAILURE);
    }
 
  MXMLFromString(&E,ptr,NULL,NULL);
 
  MOFromXML((void *)G,mxoGroup,E);
 
  MXMLDestroyE(&E);
 
  return(SUCCESS);
  }  /* END MGroupLoadCP() */


/*
 * HvB: Get all GroupNames that have a Fairshare target set
*/
int MGroupGetFSGroups()
  {
  int           gindex;
  int		fsindex = 0;
  mgcred_t	*G; 

  const char *FName = "MGroupGetFSGroups";

  DBG(5,fFS) DPrint("%s\n", FName);

  for (gindex = 0;gindex < MAX_MGROUP + MAX_MHBUF;gindex++)
    {
    G = &MGroup[gindex];

    if ((G == NULL) || (G->Name[0] == '\0') || (G->Name[0] == '\1'))
      continue;

    if (!strcmp(G->Name,ALL) || !strcmp(G->Name,"NOGROUP"))
      continue;

    if ( G->F.FSTarget > 0 )
      {
      DBG(5,fFS) DPrint("Found FSGroup %s(%f) with key %d \n", G->Name, G->F.FSTarget, G->Key);
      FSGroupKeys[fsindex++] = G->Key;
      }
    }
  } /* MGroupGetFSGroups */

/* HvB */
int MGroupSecondary(
  char *UName,    /* I */
  mjob_t  *J)     /* O */

  {
  int		i; 
  int		gindex;

  int		ng = 0;
  int		rc;

  gid_t		*groups = NULL;
  struct	group *sec_grp;
  struct	group grp;

  mgcred_t	*GRP_cred; 
 
  const char *FName = "MGroupSecondary";

  DBG(5,fFS) DPrint("%s(%s)\n",
    FName,
    (UName != NULL) ? UName : "NULL"); 
  
  /*
    MGroupGetFSGroups();
  */

  i=0;
  while ( FSGroupKeys[i] != -1 )
    {
    DBG(5,fFS) DPrint("Checking if user(%s)  is member of group(%s)\n", UName, MGroup[FSGroupKeys[i]].Name);
    if ( (sec_grp = getgrnam(MGroup[FSGroupKeys[i]].Name)) != NULL )
      {
      gindex=0;
      while ( sec_grp->gr_mem[gindex] )
        {
        if ( !strcmp(sec_grp->gr_mem[gindex], UName) )
          {
          DBG(5,fFS) DPrint("Setting Group credentials for user %s to %s\n", UName, sec_grp->gr_name);
          if ( MGroupAdd(sec_grp->gr_name, &J->Cred.G) == FAILURE )
            {
            DBG(1,fPBS) DPrint("ERROR:    cannot add secondary group for job %s (Name: %s)\n",
            J->Name,
            sec_grp->gr_name);

            return(FAILURE);
            }
          else
            {
                return(SUCCESS);
            }
          } /* if !strcmp */
        else
          {
          gindex++;
          }
        } /* while sec_grp */
      } /* if sec_grp */ 

    i++;

    } /* while FSGroupKeys[i] */

  return(SUCCESS);

  } /* MGroupSecondary */


int MGroupAdd(
 
  char      *GName,  /* I */
  mgcred_t **GP)     /* O (optional) */
 
  {
  int           gindex;
 
  unsigned long hashkey;
 
  mgcred_t      *Gtmp;

  const char *FName = "MGroupAdd";
 
  DBG(6,fSTRUCT) DPrint("%s(%s,%s)\n",
    FName,
    (GName != NULL) ? "GName" : "NULL",
    (GP != NULL) ? "GP" : "NULL");

  if ((GName == NULL) || (GName[0] == '\0'))
    {
    return(FAILURE);
    }
 
  if (GP != NULL)
    *GP = NULL;
 
  hashkey = MAX(1,MUGetHash(GName) % MAX_MGROUP);
 
  for (gindex = hashkey;gindex < MAX_MGROUP + MAX_MHBUF;gindex++)
    {
    Gtmp = &MGroup[gindex];
 
    /* if group already in hash table */
 
    if (hashkey == Gtmp->Key)
      {
      if (!strcmp(Gtmp->Name,GName))
        {
        if (GP != NULL)
          *GP = Gtmp;
 
        return(SUCCESS);
        }
      }
 
    /* if empty slot found */
 
    if (Gtmp->Key == 0)
      {
      /* setup new record */
 
      if (GP != NULL)
        *GP = Gtmp;
 
      Gtmp->Key = hashkey;
 
      Gtmp->Index = gindex;
 
      MUStrCpy(Gtmp->Name,GName,sizeof(Gtmp->Name));
 
      if (strcmp(GName,ALL) && strcmp(GName,NONE))
        {
        /* update group record */
 
        if (MSched.Mode != msmSim)
          MCPRestore(mcpGroup,GName,(void *)Gtmp);
 
        Gtmp->OID = MUGIDFromName(Gtmp->Name);
 
        DBG(5,fSTRUCT) DPrint("INFO:     group %s added\n",
          GName);
        }
      else
        {
        /* NYI */
        }
 
      return(SUCCESS);
      }
    }    /* END for (gindex) */
 
  /* end of table reached */
 
  DBG(1,fSTRUCT) DPrint("ALERT:    group table overflow.  cannot add %s\n",
    GName);
 
  return(FAILURE);
  }  /* END MGroupAdd() */




int MGroupFind(

  char      *GName,  /* I */
  mgcred_t **GP)     /* O */

  {
  int gindex;

  unsigned long hashkey;      

  mgcred_t *Gtmp;

  const char *FName = "MGroupFind";

  DBG(7,fSTRUCT) DPrint("%s(%s,G)\n",
    FName,
    GName);

  if (GP != NULL)
    *GP = NULL;

  if ((GName == NULL) || 
      (GName[0] == '\0')) 
    {
    return(FAILURE);
    }

  hashkey = MAX(1,MUGetHash(GName) % MAX_MGROUP);   

  for (gindex = hashkey;gindex < MAX_MGROUP + MAX_MHBUF;gindex++)
    {
    Gtmp = &MGroup[gindex];  

    if (Gtmp->Name[0] == '\0')
      {
      if (GP != NULL)
        *GP = Gtmp;

      break;
      }

    if ((hashkey != Gtmp->Key) || (strcmp(Gtmp->Name,GName)))
      {
      continue;
      }

    /* group found */    

    if (GP != NULL)
      *GP = Gtmp;
 
    return(SUCCESS);
    }  /* END for (gindex) */

  return(FAILURE);
  }  /* END MGroupFind() */




int MGroupInitialize(

  mgcred_t *G,
  char     *GName)

  {
  if ((G == NULL) || 
      (GName == NULL) || 
      (GName[0] == '\0'))
    {
    return(FAILURE);
    }

  memset(G,0,sizeof(mgcred_t));

  MUStrCpy(G->Name,GName,sizeof(G->Name));

  return(SUCCESS);
  }  /* END MGroupInitialize() */




int MGroupToString(
 
  mgcred_t *G,
  char     *Buf)
 
  {
  const int CPCList[] = {
    mxoStats,
    -1 };
 
  mxml_t *E = NULL;
 
  if ((G == NULL) || (Buf == NULL))
    {
    return(FAILURE);
    }

  Buf[0] = '\0';     
 
  MCOToXML((void *)G,mxoGroup,&E,NULL,(int *)CPCList,0);    
 
  MXMLToString(E,Buf,MAX_MBUFFER,NULL,TRUE);
 
  MXMLDestroyE(&E);
 
  return(SUCCESS);
  }  /* END MGroupToString() */




int MGroupToXML(
 
  mgcred_t  *G,
  mxml_t  **EP,
  int       *SAList)
 
  {
  int DAList[] = {
    mcaID,
    -1 };
 
  int  aindex;
 
  int *AList;
 
  char tmpString[MAX_MLINE];
 
  if ((G == NULL) || (EP == NULL))
    {
    return(FAILURE);
    }
 
  if (SAList != NULL)
    AList = SAList;
  else
    AList = DAList; 

  MXMLCreateE(EP,(char *)MXO[mxoGroup]);                     
 
  for (aindex = 0;AList[aindex] != -1;aindex++)
    {
    if ((MCredAToString((void *)G,mxoGroup,AList[aindex],tmpString,0) == FAILURE) ||
        (tmpString[0] == '\0'))
      {
      continue;
      }
 
    MXMLSetAttr(*EP,(char *)MCredAttr[AList[aindex]],tmpString,mdfString);
    }  /* END for (aindex) */
 
  return(SUCCESS);
  }  /* END MGroupToXML() */




char *MGroupShow(  

  mgcred_t *G,        /* I */
  char     *SBuffer,  /* O (optional) */
  long     *SBufSize, /* I */
  long      Mode)     /* I */

  {
  static char Line[MAX_MLINE];

  char FlagLine[MAX_MLINE];

  char QALLine[MAX_MLINE];
  char QALChar;

  char GLLine[MAX_MLINE];

  char *Head;

  char *BPtr;
  int   BSpace;

  const char *FName = "MGroupShow";

  DBG(3,fUI) DPrint("%s(%s,Buf,BufSize,%ld)\n",
    FName,
    (G != NULL) ? G->Name : "NULL",
    Mode);

  if (SBuffer != NULL)
    {
    BPtr   = SBuffer;
    BSpace = *SBufSize;
    }
  else
    {
    BPtr   = Line;
    BSpace = sizeof(Line);
    }

  BPtr[0] = '\0';

  Head = BPtr;

  if (G == NULL)
    {
    /* build header */

    /*            NAME PRI FLAG QDEF QLST * PLST TRG LIMITS */

    MUSNPrintF(&BPtr,&BSpace,"%-12s %8s %12s %12s %12s%s %20s %6s %7s\n\n",
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

    MUBMToString(G->F.JobFlags,MJobFlags,':',FlagLine,NONE);

    MUStrCpy(QALLine,MQOSBMToString(G->F.QAL),sizeof(QALLine));

    if (G->F.QALType == qalAND)
      QALChar = '&';
    else if (G->F.QALType == qalONLY)
      QALChar = '^';
    else
      QALChar = ' ';

    MUStrCpy(GLLine,
      MCredShowAttrs(&G->L.AP,G->L.IP,NULL,NULL,NULL,&G->F,0,(1 << mcsLimits)),sizeof(GLLine));

    /*            NAME PRIO FLAG QDEF QLST * PLST FSTARG LIMITS */

    MUSNPrintF(&BPtr,&BSpace,"%-12s %8ld %12s %12s %12s%c %20s %6.2lf %7s\n",
      G->Name,
      G->F.Priority,
      FlagLine,
      ((mqos_t *)G->F.QDef) != NULL ?
        ((mqos_t *)G->F.QDef)->Name  :
        NONE,
      (QALLine[0] != '\0') ? QALLine : NONE,
      QALChar,
      (G->F.PAL[0] == 0) ?
        NONE :
        MUListAttrs(ePartition,G->F.PAL[0]),
      G->F.FSTarget,
      (GLLine[0] != '\0') ? GLLine : NONE);

    /* add group attributes */

    if (G->L.APC != NULL)
      {
      int cindex;

      for (cindex = 0;cindex < MAX_MCLASS;cindex++)
        {
        if (G->L.APC[cindex].SLimit[mptMaxProc][0] > 0)
          {
          MUSNPrintF(&BPtr,&BSpace,"  MAXPROC[CLASS:%s]=%d,%d\n",
            MClass[cindex].Name,
            G->L.APC[cindex].SLimit[mptMaxProc][0],
            G->L.APC[cindex].HLimit[mptMaxProc][0]);
          }
        }  /* END for (cindex) */  
      }    /* END if (G->L.APC != NULL) */

    if (G->L.APQ != NULL)
      {
      int cindex;

      for (cindex = 0;cindex < MAX_MQOS;cindex++)
        {
        if (G->L.APQ[cindex].SLimit[mptMaxProc][0] > 0)
          {
          MUSNPrintF(&BPtr,&BSpace,"  MAXPROC[QOS:%s]=%d,%d\n",
            MQOS[cindex].Name,
            G->L.APQ[cindex].SLimit[mptMaxProc][0],
            G->L.APQ[cindex].HLimit[mptMaxProc][0]);
          }
        }  /* END for (cindex) */
      }    /* END if (G->L.APQ != NULL) */

    if (Mode & (1 << mcmVerbose))
      {
      char tmpLine[MAX_MLINE];

      /* display additional attributes */

      MCredConfigLShow(
        (void *)G,
        mxoGroup,
        TRUE,
        -1,
        tmpLine);

      if (tmpLine[0] != '\0')
        {
        MUSNPrintF(&BPtr,&BSpace,"  %s\n",
          tmpLine);
        }
      }    /* END if (Mode == Verbose) */
    }      /* END else (G == NULL) */

  return(Head);
  }  /* END MGroupShow() */




int MGroupProcessConfig(

  mgcred_t *G,     /* I */
  char     *Value) /* I */

  {
  char *ptr;
  char *TokPtr;

  int   aindex;

  char  ValLine[MAX_MNAME];

  const char *FName = "MGroupProcessConfig";

  DBG(5,fCONFIG) DPrint("%s(%s,%s)\n",
    FName,
    (G != NULL) ? G->Name : "NULL",
    (Value != NULL) ? Value : "NULL");

  if ((G == NULL) ||
      (Value == NULL) ||
      (Value[0] == '\0'))
    {
    return(FAILURE);
    }

  /* process value line */

  ptr = MUStrTok(Value," \t\n",&TokPtr);

  while(ptr != NULL)
    {
    /* parse name-value pairs */

    if (MUGetPair(
          ptr,
          (const char **)MGroupAttr,
          &aindex,
          NULL,
          TRUE,
          NULL,
          ValLine,
          MAX_MNAME) == FAILURE)
      {
      /* cannot parse value pair */

      ptr = MUStrTok(NULL," \t\n",&TokPtr);

      continue;
      }

    switch(aindex)
      {
      case mgaClassWeight:

        G->ClassWeight = (int)strtol(Value,NULL,0);

        break;

      default:

        /* not handled */

        return(FAILURE);

        /*NOTREACHED*/

        break;
      }  /* END switch(AIndex) */

    ptr = MUStrTok(NULL," \t\n",&TokPtr);
    }  /* END while (ptr != NULL) */

  return(SUCCESS);
  }  /* END MGroupProcessConfig() */


/* END MGroup.c */

