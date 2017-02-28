/*
Package:  moab
Module:   mauth.c
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

#define MUNAMETOK  "AUTH="

void MAAuthorize(char *);

mlog_t mlog;

#include "MLog.c"
#include "../mcom/MSec.c"
#include "MFile.c"
#include "MGUtil.c"




int main(

  int    argc,
  char **argv)

  {
  char  KeyFile[MAX_MLINE];

  char  CSBuf[MAX_MLINE];

  char *Digest;

  char *Buf;
  char *ptr;

  int   i;

  if (argv[1] == NULL)
    {
    exit(1);
    }
   
  Buf = argv[1];

  /* determine key file */

  if ((ptr = getenv(MSCHED_ENVHOMEVAR)) != NULL) 
    { 
    sprintf(KeyFile,"%s/%s",
      ptr,
      MSCHED_KEYFILE);
    }
  else 
    { 
    sprintf(KeyFile,"%s/%s",
      MBUILD_HOMEDIR,
      MSCHED_KEYFILE);
    }

  /* check authorization */

  MAAuthorize(Buf);

  /* open keyfile */

  if ((Digest = MFULoad(KeyFile,1,macmRead,NULL,NULL)) == NULL)
    {
    fprintf(stderr, "ERROR:  cannot open keyfile '%s' for reading: %s\n", 
      KeyFile,
      strerror(errno));

    exit(errno);
    }

  for (i = strlen(Digest) - 1;i > 0;i--)
    {
    if (!isspace(Digest[i]))
      break;

    Digest[i] = '\0';
    }  /* END for (i) */

  /* compute keyed hash for data and key */

  MSecGetChecksum(
    Buf,
    strlen(Buf),
    CSBuf,
    NULL,
    mcsaHMAC,
    Digest);

  fprintf(stdout,"%s\n",
    CSBuf);

  return(0);
  }  /* END main() */





void MAAuthorize(
 
  char *Buf)

  {
  char *ptr;

  int   i;

  struct passwd *passwd_s;

  if ((ptr = strstr(Buf,MUNAMETOK)) == NULL)
    {
    fprintf(stderr,"ERROR:  data string must contain %s<user>\n",
      MUNAMETOK);

    exit(1);
    }

  if ((passwd_s = getpwuid(getuid())) == NULL)
    {
    fprintf(stderr,"ERROR:  cannot determine current user\n");

    exit(1);
    }

  ptr += strlen(MUNAMETOK);

  for (i = 0;i < strlen(ptr);i++)
    {
    if (isspace(ptr[i]))
      break;
    }  /* END for (i) */

  if (strncmp(passwd_s->pw_name,ptr,i))
    {
    fprintf(stderr,"ERROR:  %s is unauthorized to create a checksum for %s\n",
      passwd_s->pw_name, 
      ptr);

    exit(1);
    }

  return;
  }  /* END MAAuthorize() */

