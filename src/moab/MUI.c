/*
Package:  moab
Module:   MUI.c
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

extern mlog_t mlog;

extern const char *Service[];





int UIFormatShowAllJobs(

  char *SrcBuffer,
  char *DstBuffer,
  int   FormatMode)

  {
  char  *ptr;
  char   name[MAX_MNAME];
  long   stime;
  long   qtime;
  int    procs;
  long   cpulimit;
  char   tmpQOS[MAX_MNAME];
  int    count;
  int    priority;
  int    state;
 
  long   Now;
 
  char   UserName[MAX_MNAME];
 
  int    UpProcs;
  int    IdleProcs;
 
  int    UpNodes;
  int    IdleNodes;
  int    ActiveNodes;
 
  int    BusyNodes;
  int    BusyProcs;
 
  int    acount;
  int    icount;
  int    ncount;
 
  char   tmp[MAX_MLINE];

  char  *TokPtr;

  switch (FormatMode)
    {
    case mfmHTTP:

      return(UIFormatHShowAllJobs(SrcBuffer,DstBuffer));

      /*NOTREACHED*/

      break;

    default:

      break;
    }  /* END switch(FormatMode) */

  count = 0;
 
  /* get present time */
 
  sscanf(SrcBuffer,"%ld %d %d %d %d %d %d\n",
    &Now,
    &UpProcs,
    &IdleProcs,
    &UpNodes,
    &IdleNodes,
    &ActiveNodes,
    &BusyProcs);
 
  BusyNodes = ActiveNodes;

  BusyProcs = MIN(BusyProcs,UpProcs);
 
  ptr = MUStrTok(SrcBuffer,"\n",&TokPtr);
 
  /* display list of running jobs */
 
  strcpy(DstBuffer,"ACTIVE JOBS--------------------\n");
 
  sprintf(DstBuffer,"%s%18s %8s %10s %5s %11s %20s\n\n",
    DstBuffer,
    "JOBNAME",
    "USERNAME",
    "STATE",
    "PROC",
    "REMAINING",
    "STARTTIME");
 
  /* read all running jobs */
 
  acount = 0;
 
  while ((ptr = MUStrTok(NULL,"\n",&TokPtr)) != NULL)
    {
    if (!strcmp(ptr,"[ENDACTIVE]"))
      break; 
 
    acount++;
    count++;
 
    DBG(3,fUI) DPrint("line: '%s'\n",
      ptr);
 
    /* Format:  <JOBNAME> <USERNAME> <START TIME> <QUEUE TIME> <PROCS> <CPULIMIT> <QOS> <STATE> <PRIO> */
 
    sscanf(ptr,"%s %s %ld %ld %d %ld %s %d %d",
      name,
      UserName,
      &stime,
      &qtime,
      &procs,
      &cpulimit,
      tmpQOS,
      &state,
      &priority);
 
    /* display job */
 
    sprintf(DstBuffer,"%s%18s %8s %10s %5d %11s  %19s",
      DstBuffer,
      name,
      UserName,
      MJobState[state],
      procs,
      MULToTString(cpulimit - (Now - stime)),
      MULToDString((mulong *)&stime));
    }  /* END while (ptr) */
 
  sprintf(tmp,"%d Active Job%c   ",
    acount,
    (acount == 1) ? ' ' : 's');
 
  sprintf(DstBuffer,"%s\n%21s %4d of %4d Processors Active (%.2f%c)\n",
    DstBuffer,
    tmp, 
    BusyProcs,
    UpProcs,
    (UpProcs > 0) ? (double)BusyProcs / UpProcs * 100.0 : 0.0,
    '%');
 
  if (UpProcs != UpNodes)
    {
    sprintf(DstBuffer,"%s%21s %4d of %4d Nodes Active      (%.2f%c)\n",
      DstBuffer,
      " ",
      BusyNodes,
      UpNodes,
      (UpNodes > 0) ? (double)BusyNodes / UpNodes * 100.0 : 0.0,
      '%');
    }
 
  /* display list of idle jobs */
 
  strcat(DstBuffer,"\nIDLE JOBS----------------------\n");
 
  sprintf(DstBuffer,"%s%18s %8s %10s %5s %11s %20s\n\n",
    DstBuffer,
    "JOBNAME",
    "USERNAME",
    "STATE",
    "PROC",
    "WCLIMIT",
    "QUEUETIME");
 
  /* read all idle jobs */
 
  icount = 0;
 
  while ((ptr = MUStrTok(NULL,"\n",&TokPtr)) != NULL)
    {
    if (!strcmp(ptr,"[ENDIDLE]"))
      break; 
 
    count++;
    icount++;
 
    DBG(3,fUI) DPrint("line: '%s'\n",
      ptr);
 
    /* Format:  <JOBNAME> <USERNAME> <START TIME> <QUEUE TIME> <PROCS> <CPULIMIT> <QOS> <STATE> <PRIO> */
 
    sscanf(ptr,"%s %s %ld %ld %d %ld %s %d %d",
      name,
      UserName,
      &stime,
      &qtime,
      &procs,
      &cpulimit,
      tmpQOS,
      &state,
      &priority);
 
    /* display job */
 
    fprintf(stdout,"%18s %8s %10s %5d %11s  %19s",
      name,
      UserName,
      MJobState[state],
      procs,
      MULToTString(cpulimit),
      MULToDString((mulong *)&qtime));
    }
 
  fprintf(stdout,"\n%d Idle Job%c\n",
    icount,
    (icount == 1) ? ' ' : 's');
 
  /* display list of non-queued jobs */ 
 
  strcat(DstBuffer,"\nBLOCKED JOBS-------------------\n");
 
  sprintf(DstBuffer,"%s%18s %8s %10s %5s %11s %20s\n\n",
    DstBuffer,
    "JOBNAME",
    "USERNAME",
    "STATE",
    "PROC",
    "WCLIMIT",
    "QUEUETIME");
 
  /* read all non-queued jobs */
 
  ncount = 0;
 
  while ((ptr = MUStrTok(NULL,"\n",&TokPtr)) != NULL)
    {
    if (!strcmp(ptr,"[ENDNOTQUEUED]"))
      {
      break;
      }
 
    count++;
    ncount++;
 
    DBG(3,fUI) DPrint("line: '%s'\n",
      ptr);
 
    /* Format:  <JOBNAME> <USERNAME> <START TIME> <QUEUE TIME> <PROCS> <CPULIMIT> <QOS> <STATE> <PRIORITY>  */
 
    sscanf(ptr,"%s %s %ld %ld %d %ld %s %d %d",
      name,
      UserName,
      &stime,
      &qtime,
      &procs,
      &cpulimit, 
      tmpQOS,
      &state,
      &priority);
 
    /* display job */
 
    sprintf(DstBuffer,"%s%18s %8s %10s %5d %11s  %19s",
      DstBuffer,
      name,
      UserName,
      MJobState[state],
      procs,
      MULToTString(cpulimit),
      MULToDString((mulong *)&qtime));
    }  /* END while (ptr) */
 
  sprintf(DstBuffer,"%s\nTotal Jobs: %d   Active Jobs: %d   Idle Jobs: %d   Non-Queued Jobs: %d\n",
    DstBuffer,
    count,
    acount,
    icount,
    ncount);
 
  while ((ptr = MUStrTok(NULL,"\n",&TokPtr)) != NULL)
    {
    sprintf(DstBuffer,"%s\n%s\n",
      DstBuffer,
      ptr);
    }
 
  return(SUCCESS);
  }  /* END UIFormatShowAllJobs() */





int UIFormatHShowAllJobs(
 
  char *SrcBuffer,
  char *DstBuffer)
 
  {
  char  *ptr;
  char   name[MAX_MNAME];
  long   stime;
  long   qtime;
  int    procs;
  long   cpulimit;
  char   tmpQOS[MAX_MNAME];
  int    count;
  int    priority;
  int    state;
 
  long   Now;
 
  char   UserName[MAX_MNAME];
 
  int    UpProcs;
  int    IdleProcs;
 
  int    UpNodes;
  int    IdleNodes;
  int    ActiveNodes;
 
  int    BusyNodes;
  int    BusyProcs;
 
  int    acount;
  int    icount;
  int    ncount;
 
  char  *TokPtr;
 
  count = 0;
 
  /* get present time */
 
  sscanf(SrcBuffer,"%ld %d %d %d %d %d %d\n",
    &Now,
    &UpProcs,
    &IdleProcs,
    &UpNodes,
    &IdleNodes,
    &ActiveNodes,
    &BusyProcs);
 
  BusyNodes = ActiveNodes;
 
  /* display list of running jobs */
 
  DstBuffer[0] = '\0';

  if (strlen(SrcBuffer) > 1000)
    strcat(DstBuffer,"<FONT SIZE=-1>");
  else
    strcat(DstBuffer,"<FONT>");

  ptr = MUStrTok(SrcBuffer,"\n",&TokPtr);   

  /* display active jobs */
 
  acount = 0;
 
  while ((ptr = MUStrTok(NULL,"\n",&TokPtr)) != NULL)
    {
    if (!strcmp(ptr,"[ENDACTIVE]"))
      break;

    if (acount == 0)
      {
      strcat(DstBuffer,"<TABLE BORDER=1>");
 
      sprintf(DstBuffer,"%s<TR><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD></TR>",
        DstBuffer,
        "Active Jobs",
        "User Name",
        "Job State",
        "Processors",
        "Time Remaining",
        "Start Time");
      }
 
    acount++;
    count++;
 
    DBG(3,fUI) DPrint("line: '%s'\n",
      ptr);
 
    /* Format:  <JOBNAME> <USERNAME> <START TIME> <QUEUE TIME> <PROCS> <CPULIMIT> <QOS> <STATE> <PRIO> */
 
    sscanf(ptr,"%s %s %ld %ld %d %ld %s %d %d",
      name,
      UserName,
      &stime,
      &qtime, 
      &procs,
      &cpulimit,
      tmpQOS,
      &state,
      &priority);
 
    /* display job */

    sprintf(DstBuffer,"%s<TR><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%d</TD><TD>%s</TD><TD>%s</TD></TR>",  
      DstBuffer,
      name,
      UserName,
      MJobState[state],
      procs,
      MULToTString(cpulimit - (Now - stime)),
      MULToDString((mulong *)&stime));
    }  /* END while (ptr) */

  if (acount > 0)
    {
    strcat(DstBuffer,"</TABLE><p>");
 
    sprintf(DstBuffer,"%s%d Active Job%c<br>",
      DstBuffer,
      acount,
      (acount == 1) ? ' ' : 's');
 
    sprintf(DstBuffer,"%s%d of %d Processors Active (%.2f%c)<br>",
      DstBuffer,
      BusyProcs,
      UpProcs,
      (UpProcs > 0) ? (double)BusyProcs / UpProcs * 100.0 : 0.0,
      '%');
 
    if (UpProcs != UpNodes)
      {
      sprintf(DstBuffer,"%s%d of %d Nodes Active (%.2f%c)<br>",
        DstBuffer,
        BusyNodes,
        UpNodes,
        (UpNodes > 0) ? (double)BusyNodes / UpNodes * 100.0 : 0.0,
        '%');
      }

    strcat(DstBuffer,"<p>");
    }
 
  /* display idle jobs */
 
  icount = 0;
 
  while ((ptr = MUStrTok(NULL,"\n",&TokPtr)) != NULL)
    {
    if (!strcmp(ptr,"[ENDIDLE]"))
      break;

    if (icount == 0)
      {
      strcat(DstBuffer,"<TABLE BORDER=1>");

      sprintf(DstBuffer,"%s<TR><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD></TR>", 
        DstBuffer,
        "Idle Jobs",
        "User Name",
        "Job State",
        "Processors",
        "WallClock Limit",
        "Submission Time");
      }
  
    count++;
    icount++;
 
    DBG(3,fUI) DPrint("line: '%s'\n",
      ptr);
 
    /* Format:  <JOBNAME> <USERNAME> <START TIME> <QUEUE TIME> <PROCS> <CPULIMIT> <QOS> <STATE> <PRIO> */
 
    sscanf(ptr,"%s %s %ld %ld %d %ld %s %d %d",
      name,
      UserName,
      &stime,
      &qtime,
      &procs,
      &cpulimit,
      tmpQOS,
      &state,
      &priority);
 
    /* display job */

    sprintf(DstBuffer,"%s<TR><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%d</TD><TD>%s</TD><TD>%s</TD></TR>",      
      DstBuffer,
      name,
      UserName,
      MJobState[state],
      procs,
      MULToTString(cpulimit),
      MULToDString((mulong *)&qtime));
    }  /* END while (ptr) */

  if (icount > 0)
    {
    strcat(DstBuffer,"</TABLE><p>"); 

    sprintf(DstBuffer,"%s%d Idle Job%c<p>",
      DstBuffer,
      icount,
      (icount == 1) ? ' ' : 's');
    }

  /* display ineligible jobs */

  ncount = 0;
 
  while ((ptr = MUStrTok(NULL,"\n",&TokPtr)) != NULL)
    {
    if (!strcmp(ptr,"[ENDNOTQUEUED]"))
      {
      break;
      }

    if (ncount == 0)
      {
      strcat(DstBuffer,"<TABLE BORDER=1>");

      sprintf(DstBuffer,"%s<TR><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD><TD><B>%s</B></TD></TR>", 
        DstBuffer,
        "Ineligible Jobs",
        "User Name",
        "Job State",
        "Processors",
        "WallClock Limit",
        "Submission Time",
        "Reason");
      }
 
    count++;
    ncount++;
 
    DBG(3,fUI) DPrint("line: '%s'\n",
      ptr);
 
    /* Format:  <JOBNAME> <USERNAME> <START TIME> <QUEUE TIME> <PROCS> <CPULIMIT> <QOS> <STATE> <PRIORITY>  */
 
    sscanf(ptr,"%s %s %ld %ld %d %ld %s %d %d",
      name,
      UserName,
      &stime,
      &qtime,
      &procs,
      &cpulimit,
      tmpQOS,
      &state,
      &priority);
 
    /* display job */

   sprintf(DstBuffer,"%s<TR><TD>%s</TD><TD>%s</TD><TD>%s</TD><TD>%d</TD><TD>%s</TD><TD>%s</TD><TD>%s</TD></TR>",    
      DstBuffer,
      name,
      UserName,
      MJobState[state],
      procs,
      MULToTString(cpulimit),
      MULToDString((mulong *)&qtime),
      "N/A");
    }  /* END while (ptr) */

  if (ncount > 0)
    {
    strcat(DstBuffer,"</TABLE><p>");     
    }
 
  sprintf(DstBuffer,"%s<p>Total Jobs: %d &nbsp; Active Jobs: %d &nbsp; Idle Jobs: %d &nbsp; Ineligible Jobs: %d<p>",
    DstBuffer,
    count,
    acount,
    icount,
    ncount);
 
  while ((ptr = MUStrTok(NULL,"\n",&TokPtr)) != NULL)
    {
    sprintf(DstBuffer,"%s<p>%s<p>",
      DstBuffer,
      ptr);
    }

  strcat(DstBuffer,"</FONT>");         
 
  return(SUCCESS);
  }  /* END UIFormatHShowAllJobs() */

/* END MUI.c */

