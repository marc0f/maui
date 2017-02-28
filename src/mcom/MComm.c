/*
Package:  moab
Module:   MComm.c
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
#include "moab-proto.h"

extern const char *SUMKType[];

extern mlog_t mlog;




void MSUCommParserInit(

  sucomm_parser_t *parser)   /* I (modified) */

  {
  if (parser == NULL)
    {
    return;
    }

  parser->length  = 0;
  parser->state   = 0;
  parser->msgSize = 0;
  
  return;
  }  /* END MSUCommParserInit() */





int MSUCommParse(

  su_t            *su,
  sucomm_parser_t *Parser,  /* I */
  void            *data,
  size_t           size,
  mbool_t         *IsDone)  /* O (modified) */

  {

  *IsDone = FALSE;

  if (Parser == NULL)
    {
    return(FAILURE);
    }

  if (Parser->length + size >= MMAX_BUFFER) /* buffer overflow */
    {
    return(FAILURE);
    }

  memcpy(Parser->buffer + Parser->length, data, size);
  Parser->length += size;

  /* perform mcomm specific parsing */

  switch (Parser->state)
    {
    case 0: /* initial */

      if (Parser->length < 9)
        {
        return(SUCCESS);
        }

      Parser->buffer[8] = '\0';
      Parser->msgSize = strtoul(Parser->buffer, NULL, 10);

      if ((Parser->msgSize == 0) || (Parser->msgSize >= MMAX_BUFFER))
        {
        return(FAILURE);
        }

      memmove(Parser->buffer,Parser->buffer + 9,Parser->length - 9);

      Parser->length -= 9;

      Parser->state = 1;

      /* fall through */

    case 1: /* length known */

      if (Parser->length >= (int)Parser->msgSize) 
        {
        *IsDone = TRUE;

        Parser->buffer[Parser->length] = '\0';
        }

    return(SUCCESS);
    }  /* END switch (Parser->state) */

  return(FAILURE);
  }  /* END MSUCommParse() */




int MSUCommWrapMessage(

  su_t       *su,
  const char *cmd,
  const char *args,
  suclcred_t *Cred,
  char       *buffer,
  size_t      size)

  {
  char msg[MMAX_BUFFER];
  char cksum[MMAX_LINE];
  time_t Now;
  char backup;


  time(&Now);

  snprintf(msg,MMAX_BUFFER,"%s%ld ID=%s %s%s%s %s%s %s%s %s%s",
    SUMKType[sumkTimeStamp],
    (long)Now,
    su->Env->UserName,
    SUMKType[sumkData],
    SUMKType[sumkCommand],
    cmd,
    SUMKType[sumkAuth],
    (su->Env->UserName != NULL) ? su->Env->UserName : NONE,
    SUMKType[sumkClient],
    (su->Name != NULL) ? su->Name : NONE,
    SUMKType[sumkArgs],
    (args != NULL) ? args : "");

  MSecGetChecksum(
    msg,
    strlen(msg),
    cksum,
    NULL,
    mcsaDES,
    Cred->EncryptionData.CSKey);

  snprintf(buffer + 9,size - 9,"%s%s %s",
    SUMKType[sumkCheckSum],
    cksum,
    msg);

  backup = buffer[9];
  sprintf(buffer, "%08ld\n", (long)strlen(buffer+9));
  buffer[9] = backup;

  return(strlen(buffer));
  }  /* END MSUCommWrapMessage() */




int MSUCommMessageIsValid(

  su_t       *su,
  char       *msg,    /* I */
  suclcred_t *Cred)

  {
  char  *cksum = strstr(msg,SUMKType[sumkCheckSum]);
  char  *ts = strstr(msg,SUMKType[sumkTimeStamp]);
  char  *data = strstr(msg,SUMKType[sumkData]);
  char  *cksumEnd;
  char  *tsEnd;
  char   CKSum[MMAX_LINE];
  time_t TSVal;
  time_t Now;

  if (cksum == NULL)
    {
    MDB(1,fSOCK) MLog("ALERT:    cannot locate checksum '%s'\n",
      msg);

    return(FAILURE);
    }

  if (ts == NULL)
    {
    MDB(1,fSOCK) MLog("ALERT:    cannot locate timestamp '%s'\n",
      msg);

    return(FAILURE);
    }

  if (data == NULL)
    {
    MDB(1,fSOCK) MLog("ALERT:    cannot locate data '%s'\n",
      msg);

    return(FAILURE);
    }

  cksum += strlen(SUMKType[sumkCheckSum]);
  cksumEnd = strchr(cksum,' ');

  if (cksumEnd)
    *cksumEnd = 0;

  MSecGetChecksum(
    ts,
    strlen(ts),
    CKSum,
    NULL,
    mcsaDES,
    Cred->EncryptionData.CSKey);

  if (strcmp(cksum,CKSum) != 0)
    {
    MDB(1,fSOCK) MLog("ALERT:    checksum does not match (%s:%s)\n",
      cksum,
      CKSum);

    if (cksumEnd)
      *cksumEnd = ' ';

    return(FAILURE);
    }

  if (cksumEnd)
    *cksumEnd = ' ';

  ts += strlen(SUMKType[sumkTimeStamp]);
  tsEnd = strchr(ts, ' ');

  if (tsEnd)
    *tsEnd = 0;

  TSVal = strtoul(ts, NULL, 10);

  if (tsEnd)
    *tsEnd = ' ';

  time(&Now);

  if (((Now - TSVal) > 3600) || ((Now - TSVal) < -3600))
    {
    MDB(1,fSOCK) MLog("ALERT:    timestamp does not match (%ld:%ld)\n",
      (unsigned long)Now,
      (unsigned long)TSVal);

    return(FAILURE);
    }

  return(SUCCESS);
  }  /* END MSUCommMessageIsValid() */


/* END MComm.c */

