/*
Package:  moab
Module:   MLocal.c
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
        
/* Contains:                                               *
 *                                                         *
 *  int MLocalJobInit(J)                                   *
 *  int MLocalNodeInit(N)                                  *
 *  int MLocalJobCheckNRes(J,N,Time)                       *
 *  int MLocalCheckFairnessPolicy(J,StartTime,Message)     *
 *  int MLocalGetNodePriority(J,N)                         *
 *  int MLocalInitialize()                                 *
 *  int MLocalJobDistributeTasks(J,R,NodeList,TaskMap)     *
 *  int MLocalQueueScheduleIJobs(J)                        *
 *                                                         */

  
#include "moab.h"
#include "msched-proto.h"  

extern msched_t     MSched;
extern mnode_t     *MNode[];
extern mstat_t      MStat;
extern mattrlist_t  MAList;
extern mpar_t       MPar[];
extern mjob_t      *MJob[];
extern mlog_t       mlog;




/* #include "../../contrib/checkreq/AussieJobCheckNRes.c" */

int MLocalJobCheckNRes(

  mjob_t  *J,         /* I */
  mnode_t *N,         /* I */
  long     StartTime) /* I */

  {
  const char *FName = "MLocalJobCheckNRes";

  int rc = SUCCESS;

  DBG(8,fSCHED) DPrint("%s(%s,%s,%ld)\n",
    FName,
    J->Name,
    N->Name,
    StartTime);

/*
  rc = ContribAussieJobCheckNRes(J,N,StartTime);
*/

  return(rc);
  }  /* END MLocalJobCheckNRes */




/* #include "../../contrib/jobinit/AussieJobInit.c" */

int MLocalJobInit(

  mjob_t *J)  /* I */

  {
/*
  ContribAussieJobInit(J); 
*/

  return(SUCCESS);
  }  /* END MLocalJobInitialize() */




int MLocalNodeInit(

  mnode_t *N)

  {
  return(SUCCESS);
  }  /* END MLocalNodeInit() */




/* #include "../../contrib/fairness/JobLength.c" */
/* #include "../../contrib/fairness/ASCBackgroundJobPolicy.c" */

int MLocalCheckFairnessPolicy(

  mjob_t *J,
  long    StartTime,
  char   *Message)

  {
  const char *FName = "MLocalCheckFairnessPolicy";

  DBG(6,fSCHED) DPrint("%s(%s,%ld,Message)\n",
    FName,
    (J != NULL) ? J->Name : "NULL",
    (unsigned long)StartTime);

/*
  if (ContribJobLengthFairnessPolicy(J,StartTime,Message) == FAILURE)
    {
    return(FAILURE);
    }
*/

/*
  if (ContribASCBackgroundJobPolicy(J,StartTime,Message) == FAILURE)
    {
    return(FAILURE);
    }
*/

  /* all local policies passed */

  return(SUCCESS);
  }  /* END MLocalFairnessPolicy() */





/* #include "../../contrib/nodeallocation/PNNLGetNodePriority.c" */

int MLocalGetNodePriority(

  mjob_t  *J,
  mnode_t *N)

  {
  int Priority = 0;

/*
  Priority = ContribPNNLGetNodePriority(J,N));
*/

  return(Priority);
  }  /* END MLocalGetNodePriority() */





/* #include "../../contrib/nodeallocation/OSCProximityNodeAlloc.c" */
 
int MLocalJobAllocateResources(
 
  mjob_t *J,                /* I:  job allocating nodes                           */
  mreq_t *RQ,               /* I:  req allocating nodes                           */        
  mnalloc_t NodeList[],     /* I:  eligible nodes                                 */
  mulong  StartTime,        /* I                                                  */
  int     RQIndex,          /* I:  index of job req to evaluate                   */
  int     MinTPN[],         /* I:  min tasks per node allowed                     */
  int     MaxTPN[],         /* I:  max tasks per node allowed                     */
  char    NodeMap[],        /* I:  array of node alloc states                     */
  int     AffinityLevel,    /* I:  current reservation affinity level to evaluate */
  int     NodeIndex[],      /* I/OUT:  index of next node to find in BestList     */
  mnalloc_t *BestList[MAX_MREQ_PER_JOB], /* I/OUT:    list of selected nodes      */
  int     TaskCount[],      /* I/OUT:  total tasks allocated to job req           */
  int     NodeCount[])      /* I/OUT:  total nodes allocated to job req           */
 
  {
/*
  if (ContribOSCProximityNodeAlloc(
        J,
        RQ,
        NodeList,
        RQIndex,
        MinTPN,
        MaxTPN,
        NodeMap,
        AffinityLevel,
        NodeIndex,
        BestList,
        TaskCount,
        NodeCount) == FAILURE)
    {
    return(FAILURE);
    }
*/
 
  return(FAILURE);  
  }  /* END MLocalJobAllocateResources() */




/* #include "../../contrib/appsim/SCHSMSim.c" */  

int MLocalInitialize()

  {
/*
  if (ContribSCHSMSimInitialize() == FAILURE)
    {
    return(FAILURE);
    }
*/

  return(SUCCESS);
  }  /* END MLocalInitialize() */




int MLocalJobDistributeTasks(
 
  mjob_t    *J,
  mrm_t     *R,
  mnalloc_t *NodeList, /* OUT: nodelist with taskcount information */
  short     *TaskMap)  /* OUT: task distribution list              */
 
  {
  /* NYI */

  return(FAILURE);
  }  /* END MLocalJobDistributeTasks() */




/* #include "../../contrib/sched/XXX.c" */

int MLocalQueueScheduleIJobs(

  int    *Q,
  mpar_t *P)

  {
  mjob_t *J;

  int     jindex;

  if ((Q == NULL) || (P == NULL))
    {
    return(FAILURE);
    }

  /* NOTE:  insert call to scheduling algorithm here */

  for (jindex = 0;Q[jindex] != -1;jindex++)
    {
    J = MJob[Q[jindex]];

    /* NYI */

    DBG(7,fSCHED) DPrint("INFO:     checking job '%s'\n",
      J->Name);
    }  /* END for (jindex) */
   
  return(FAILURE);
  }  /* END MLocalQueueScheduleIJobs() */



/* END MLocal.c */

