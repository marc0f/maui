/*
Package:  moab
Module:   MClient.c
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

#include "moab.h"
#include "msched-proto.h"

extern const char *MClientAttr[];
extern const char *MService[];




int MClientLoadConfig()

  {
  char *ptr;

  char  tmpLine[MAX_MLINE];

  while (MCfgGetSVal(
          MSched.PvtConfigBuffer,
          &ptr,
          MParam[pClientCfg],
          NULL,
          NULL,
          tmpLine,
          sizeof(tmpLine),
          0,
          NULL) == SUCCESS)
    {
    MClientProcessConfig(tmpLine);
    }  /* END while MCfgGetSVal() == SUCCESS) */

  return(SUCCESS);
  }  /* END MClientLoadConfig() */




int MClientProcessConfig(

  char *Value)  /* I */

  {
  char *ptr;
  char *TokPtr;

  int   aindex;

  char  ValLine[MAX_MLINE];

  char  AttrArray[MAX_MNAME];

  if (Value == NULL)
    {
    return(FAILURE);
    }

  ptr = MUStrTokE(Value," \t\n",&TokPtr);

  while(ptr != NULL)
    {
    /* parse name-value pairs */

    /* FORMAT:  <ATTR>=<VALUE>[,<VALUE>] */

    if (MUGetPair(
          ptr,
          (const char **)MClientAttr,
          &aindex,
          AttrArray,
          TRUE,
          NULL,
          ValLine,
          MAX_MNAME) == FAILURE)
      {
      /* cannot parse value pair */

      ptr = MUStrTokE(NULL," \t\n",&TokPtr);

      continue;
      }

    switch (aindex)
      {
      case mcltaTimeout:

        /* NYI */

        break;

      case mcltaCSAlgo:

        /* NYI */

        break;

      default:

        /* attribute not supported */

        return(FAILURE);

        /*NOTREACHED*/

        break;
      }  /* END switch (aindex) */

    ptr = MUStrTok(NULL," \t\n",&TokPtr);
    }    /* END while (ptr != NULL) */

  return(FAILURE);
  }  /* END MClientProcessConfig() */





int MCSendRequest(

  msocket_t *S)  /* I */

  {
  const char *FName = "MCSendRequest";

  DBG(2,fUI) DPrint("%s(%s)\n",
    FName,
    (S != NULL) ? "S" : "NULL");

  if (S == NULL)
    {
    return(FAILURE);
    }

  if (S->SBufSize == 0)
    S->SBufSize = (long)strlen(S->SBuffer);

  if (MSUSendData(S,S->Timeout,TRUE,FALSE) == FAILURE)
    {
    DBG(0,fSOCK) DPrint("ERROR:    cannot send request to server %s:%d (server may not be running)\n",
      S->RemoteHost,
      S->RemotePort);

    MSUDisconnect(S);

    return(FAILURE);
    }
  else
    {
    DBG(1,fUI) DPrint("INFO:     message sent to server\n");

    DBG(3,fUI) DPrint("INFO:     message sent: '%s'\n",
      (S->SBuffer != NULL) ? S->SBuffer : "NULL");
    }

  if (MSURecvData(S,S->Timeout,TRUE,NULL,NULL) == FAILURE)
    {
    fprintf(stderr,"ERROR:    lost connection to server\n");

    return(FAILURE);
    }

  DBG(3,fUI) DPrint("INFO:     message received\n");

  DBG(4,fUI) DPrint("INFO:     received message '%s' from server\n",
    S->RBuffer);

  return(SUCCESS);
  }  /* END MCSendRequest() */




int MCDoCommand(

  char *HostName,   /* I */
  int   Port,       /* I */
  int   CIndex,     /* I */
  char *CmdString,  /* I */
  char *Response)   /* O */

  {
  static msocket_t S;

  char   tmpLine[MAX_MLINE];

  /* initialize socket */

  memset(&S,0,sizeof(S));

  MUStrCpy(S.RemoteHost,HostName,sizeof(S.RemoteHost));
  S.RemotePort = Port;

  strcpy(S.CSKey,MSched.DefaultCSKey);

  S.CSAlgo         = MSched.DefaultCSAlgo;

  S.SocketProtocol = 0;
  S.SBuffer        = tmpLine;

  S.Timeout        = 5;

  if (MSUConnect(&S,FALSE,NULL) == FAILURE)
    {
    DBG(0,fSOCK) DPrint("ERROR:    cannot connect to '%s' port %d\n",
      S.RemoteHost,
      S.RemotePort);

    return(FAILURE);
    }

  sprintf(S.SBuffer,"%s%s %s%s %s%s\n",
    MCKeyword[mckCommand],
    MService[CIndex],
    MCKeyword[mckAuth],
    MUUIDToName(MOSGetEUID()),
    MCKeyword[mckArgs],
    CmdString);

  S.SBufSize = (long)strlen(S.SBuffer);

  if (MCSendRequest(&S) == FAILURE)
    {
    S.RBuffer[0] = '\0';

    return(FAILURE);
    }

  MUStrCpy(Response,S.RBuffer,MAX_MLINE);

  MSUDisconnect(&S);

  return(SUCCESS);
  }  /* END MCDoCommand() */


/* END MClient.c */
