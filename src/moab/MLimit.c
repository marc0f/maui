/*
Package:  moab
Module:   MLimit.c
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
 *  int MLimitEnforceAll(PLimit)             *
 *                                           */


#include "moab.h"
#include "msched-proto.h"  

extern mlog_t      mlog;

extern msched_t    MSched;
extern mjob_t     *MJob[];
extern mattrlist_t MAList;

extern const char *MResourceType[];
extern const char *MPolicyAction[];





int MLimitEnforceAll(

  mpar_t *P) /* I */

  {
  long    JobWCX;

  mjob_t *J;
  mreq_t *RQ;

  mbool_t ResourceLimitsExceeded;
  mbool_t JobExceedsLimits;

  int     VRes = -1;
  int     VLimit = -1;
  int     VVal = -1;

  int     rc;

  char    tmpMsg[MAX_MLINE];

  const char *FName = "MLimitEnforceAll";

  DBG(4,fSCHED) DPrint("%s(%s)\n",
    FName,
    MAList[ePartition][P->Index]);

  for (J = MJob[0]->Next;(J != NULL) && (J != MJob[0]);J = J->Next)
    {
    if ((J->State != mjsStarting) &&
        (J->State != mjsRunning))
      {
      continue;
      }

    /* enforce wallclock limits */

    JobWCX = MSched.JobMaxOverrun;
 
    if ((J->Cred.C != NULL) && (J->Cred.C->F.Overrun > 0))
      JobWCX = J->Cred.C->F.Overrun;
 
    if ((JobWCX >= 0) &&
        (J->WCLimit > 0) &&
        (MSched.Time > J->StartTime) &&
       ((unsigned long)(MSched.Time - J->StartTime) > (J->WCLimit + J->SWallTime + JobWCX)))
      {
      DBG(2,fCORE) DPrint("ALERT:    job '%s' in state '%s' has exceeded its wallclock limit (%ld+S:%ld) by %s (job will be cancelled)\n",
        J->Name,
        MJobState[J->State],
        J->WCLimit,
        J->SWallTime,
        MULToTString((MSched.Time - J->StartTime) - (J->WCLimit + J->SWallTime)));
 
      sprintf(tmpMsg,"JOBWCVIOLATION:  job '%s' in state '%s' has exceeded its wallclock limit (%ld) by %s (job will be cancelled)  job start time: %s",
        J->Name,
        MJobState[J->State],
        J->WCLimit,
        MULToTString(MSched.Time - J->StartTime - J->WCLimit),
        MULToDString(&J->StartTime));
 
      MSysRegEvent(tmpMsg,0,0,1);

      if (MSched.WCViolAction == mwcvaPreempt)
        {
        if (MJobPreempt(J,NULL,-1,NULL,NULL) == FAILURE)
          {
          DBG(2,fCORE) DPrint("ALERT:    cannot preempt job '%s' (job exceeded wallclock limit)\n",
            J->Name);
          }
        }
      else
        {
        if (MRMJobCancel(J,"MOAB_INFO:  job exceeded wallclock limit\n",NULL) == FAILURE)
          {
          /* extend job wallclock by JobWCX */
 
          /* NYI */
          }
        }
      }    /* END if ((JobWCX >= 0) && ...) */

    /* enforce CRes utilization limits */

    ResourceLimitsExceeded = FALSE;
    JobExceedsLimits = FALSE;

    RQ = J->Req[0];  /* FIXME */

    if ((P->ResourceLimitPolicy[mrProc] != mrlpNONE) &&
        (RQ->URes.Procs > 100 * RQ->DRes.Procs))
      {
      DBG(3,fSCHED) DPrint("INFO:     job %s exceeds requested proc limit (%.2lf > %.2lf)\n",
        J->Name,
        (double)RQ->URes.Procs / 100.0,
        (double)RQ->DRes.Procs);

      VRes   = mrProc;
      VLimit = 100 * RQ->DRes.Procs;
      VVal   = RQ->URes.Procs;

      ResourceLimitsExceeded = TRUE;
      JobExceedsLimits       = TRUE;
      }
    else if ((P->ResourceLimitPolicy[mrMem] != mrlpNONE) &&
        (RQ->DRes.Mem > 0) &&
        (RQ->URes.Mem > RQ->DRes.Mem))
      {
      DBG(3,fSCHED) DPrint("INFO:     job %s exceeds requested memory limit (%d > %d)\n",
        J->Name,
        RQ->URes.Mem,
        RQ->DRes.Mem);

      VRes   = mrMem;
      VLimit = RQ->DRes.Mem;
      VVal   = RQ->URes.Mem;

      ResourceLimitsExceeded = TRUE;
      JobExceedsLimits       = TRUE;
      }
    else if ((P->ResourceLimitPolicy[mrSwap] != mrlpNONE) &&
        (RQ->DRes.Swap > 0) &&		     
        (RQ->URes.Swap > RQ->DRes.Swap))
      {
      DBG(3,fSCHED) DPrint("INFO:     job %s exceeds requested swap limit (%d > %d)\n",
        J->Name,
        RQ->URes.Swap,
        RQ->DRes.Swap);

      VRes   = mrSwap;
      VLimit = RQ->DRes.Swap;
      VVal   = RQ->URes.Swap;

      ResourceLimitsExceeded = TRUE;
      JobExceedsLimits       = TRUE;
      }
    else if ((P->ResourceLimitPolicy[mrDisk] != mrlpNONE) &&
        (RQ->DRes.Disk > 0) &&
        (RQ->URes.Disk > RQ->DRes.Disk))
      {
      DBG(3,fSCHED) DPrint("INFO:     job %s exceeds requested disk limit (%d > %d)\n",
        J->Name,
        RQ->URes.Disk,
        RQ->DRes.Disk);

      VRes   = mrDisk;
      VLimit = RQ->DRes.Disk;
      VVal   = RQ->URes.Disk;

      ResourceLimitsExceeded = TRUE;
      JobExceedsLimits       = TRUE;
      }

    if (ResourceLimitsExceeded == FALSE)
      {
      continue;
      }

    /* job is using more resources than requested */

    J->RULVTime += (mulong)((MSched.Interval + 50) / 100);
  
    switch (P->ResourceLimitPolicy[VRes])
      {
      case mrlpAlways:

        /* check limited resources */

        break;

      case mrlpExtendedViolation:

        /* determine length of violation */

        if (J->RULVTime < P->ResourceLimitMaxViolationTime[VRes])
          {
          /* ignore violation */

          ResourceLimitsExceeded = FALSE;
          }
         
        break;

      case mrlpBlockedWorkloadOnly:

        /* determine if eligible job is blocked */

        /* does job reservation exist at Now + MSched.Iteration for node utilized by job? */

        /* NYI */

        ResourceLimitsExceeded = FALSE;

        break;

      default:

        DBG(1,fSCHED) DPrint("ALERT:    unexpected limit violation policy %d\n",
          P->ResourceLimitPolicy[VRes]);

        ResourceLimitsExceeded = FALSE;

        break;
      }  /* END switch (P->ResourceUtilizationPolicy) */

    if (ResourceLimitsExceeded == FALSE)
      {
      continue;
      }

    /* job violates resource utilization policy */

    sprintf(tmpMsg,"JOBRESVIOLATION:  job '%s' in state '%s' has exceeded %s resource limit (%d > %d) (action %s will be taken)  job start time: %s",
      J->Name,
      MJobState[J->State],
      MResourceType[VRes],
      VVal,
      VLimit,
      MPolicyAction[P->ResourceLimitViolationAction[VRes]],
      MULToDString(&J->StartTime));

    MSysRegEvent(tmpMsg,0,0,1);

    switch(P->ResourceLimitViolationAction[VRes])
      {
      case mrlaRequeue:

        rc = MRMJobRequeue(J,NULL,NULL);

        break;

      case mrlaCancel:

        rc = MRMJobCancel(J,"job violates resource utilization policies",NULL);

        break;

      case mrlaSuspend:

        if ((rc = MRMJobSuspend(J,NULL,NULL)) == SUCCESS)
          {
          J->RMinTime = MSched.Time + P->AdminMinSTime;
          }
         
        break;

      default:

        rc = FAILURE;

        DBG(1,fSCHED) DPrint("ALERT:    unexpected limit violation action %d\n",
          P->ResourceLimitViolationAction[VRes]);

        break;
      }  /* END switch(P->ResourceLimitViolationAction) */

    DBG(1,fSCHED) DPrint("ALERT:    limit violation action %s %s\n",
      MPolicyAction[P->ResourceLimitViolationAction[VRes]],
      (rc == SUCCESS) ? "succeeded" : "failed");

    if (JobExceedsLimits == FALSE)
      {
      /* clear job violation time */

      J->RULVTime = 0;
      }
    }    /* END for (jindex) */
 
  return(SUCCESS);
  }  /* END MLimitEnforceAll() */ 

/* END MLimit.c */

