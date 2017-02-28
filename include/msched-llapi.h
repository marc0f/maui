/*
Package:  moab
Module:   msched-llapi.h
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

#ifndef __LLAPI
#define __LLAPI

#define LL_JOB_VERSION   0
#define LL_PROC_VERSION  0
#define LL_CONTROL_VERSION  22

enum { LL_CM, QUERY_ALL, JOBS, MACHINES };
enum DataFilter { ALL_DATA, STATUS_LINE, Q_LINE };

enum Usage { SHARED, NOT_SHARED };
enum HoldType { NO_HOLD, HOLDTYPE_USER, HOLDTYPE_SYSTEM, HOLDTYPE_USERSYS };

enum {
  STATE_IDLE,STATE_PENDING,STATE_STARTING,STATE_REMOVED,STATE_REJECT_PENDING,STATE_COMPLETE_PENDING,STATE_VACATE_PENDING,STATE_REMOVE_PENDING,STATE_RUNNING,STATE_HOLD,STATE_DEFERRED,STATE_REJECTED,STATE_UNEXPANDED,STATE_NOTQUEUED,STATE_COMPLETED,STATE_VACATED,STATE_NOTRUN,STATE_SUBMISSION_ERR,STATE_TERMINATED,STATE_CANCELED,STATE_PREEMPTED,STATE_PREEMPT_PENDING,STATE_RESUME_PENDING };

enum {
  LL_MachineVirtualMemory,LL_MachineRealMemory,LL_MachineCPUs,LL_MachineDisk,LL_MachineMaxTasks,LL_MachinePoolList,LL_MachinePoolListSize,LL_MachineOperatingSystem,LL_MachineArchitecture,LL_MachineLoadAverage,LL_MachineSpeed,LL_MachineAdapterList,LL_MachineFeatureList,LL_MachineConfiguredClassList,LL_MachineAvailableClassList,LL_JobSubmitHost,LL_StepHoldType,LL_StepHostList,LL_StepState,LL_StepDispatchTime,LL_StepTaskInstanceCount,LL_StepCompletionDate,LL_StepCompletionCode,LL_StepMessages,LL_MachineName,LL_MachineStartdState,LL_StepGetFirstMachine,LL_StepGetNextMachine,LL_JobCredential,LL_CredentialUserName,LL_CredentialGroupName,LL_StepAccountNumber,LL_StepGetFirstNode,LL_NodeMinInstances,LL_NodeInitiatorCount,LL_StepGetNextNode,LL_NodeGetFirstTask,LL_NodeRequirements,LL_StepComment,LL_JobStepType,LL_JobSubmitTime,LL_StepStartCount,LL_StepStartDate,LL_StepExecSize,LL_StepImageSize,LL_StepIwd,LL_TaskExecutable,LL_StepWallClockLimitHard,LL_StepJobClass,LL_StepNodeUsage,LL_JobGetFirstStep,LL_StepID,LL_JobGetNextStep,LL_TaskIsMaster,LL_TaskGetFirstTaskInstance,LL_TaskInstanceMachineName,LL_TaskGetNextTaskInstance,LL_NodeGetNextTask };

enum { API_OK = 0,API_INVALID_INPUT,API_CANT_MALLOC,API_CANT_CONNECT,API_CONFIG_ERR,API_CANT_TRANSMIT,API_CANT_AUTH,API_CANT_FIND_PROC,API_WRNG_PROC_VERSION,API_WRNG_PROC_STATE,API_MACH_NOT_AVAIL,API_CANT_FIND_RUNCLASS,API_REQ_NOT_MET,API_WRNG_MACH_NO,API_LL_SCH_ON,API_MACH_DUP };

enum LL_control_op {
   LL_CONTROL_RECYCLE, LL_CONTROL_RECONFIG, LL_CONTROL_START,
   LL_CONTROL_STOP, LL_CONTROL_DRAIN, LL_CONTROL_DRAIN_STARTD,
   LL_CONTROL_DRAIN_SCHEDD, LL_CONTROL_PURGE_SCHEDD, LL_CONTROL_FLUSH,
   LL_CONTROL_SUSPEND, LL_CONTROL_RESUME, LL_CONTROL_RESUME_STARTD,
   LL_CONTROL_RESUME_SCHEDD, LL_CONTROL_FAVOR_JOB, LL_CONTROL_UNFAVOR_JOB,
   LL_CONTROL_FAVOR_USER, LL_CONTROL_UNFAVOR_USER,
   LL_CONTROL_HOLD_USER, LL_CONTROL_HOLD_SYSTEM, LL_CONTROL_HOLD_RELEASE,
   LL_CONTROL_PRIO_ABS, LL_CONTROL_PRIO_ADJ };

/* structures */

typedef void   LL_element;
typedef void * LL_node;

typedef struct {
  int           cluster;
  int           proc;
  char          *from_host;     /* name of the schedd host */
  } LL_STEP_ID;

typedef struct {
  LL_STEP_ID    id;             /* step id */
  int           q_date;         /* UNIX time step was submitted */
  int           status;         /* Running, unexpanded, completed,.. */

  /* The following are valid after scheduling has occured */

  int           num_processors; /* actual number of assigned processors */
  char          **processor_list;/* list of processors on which to run step */

  /* The following are inputs needed to actually start an executable */

  char          *cmd;           /* a.out file */
  char          *args;          /* command line args */
  char          *env;           /* environment */
  char          *in;            /* file for stdin */
  char          *out;           /* file for stdout */
  char          *err;           /* file for stderr */
  char          *iwd;           /* Initial working directory */
  char          *notify_user;   /* target to be used when sending mail */
  char          *shell;         /* shell to be used */
  char          *tracker;       /* user's step tracking exit */
  char          *tracker_arg;   /* argument to tracking exit */
  int           notification;   /* Notification options */
  int           image_size;     /* Size of the virtual image in K */
  int           exec_size;      /* size of the executable */
} LL_job_step;


typedef struct LL_job {
  int           version_num;    /* LL_JOB_VERSION */
  char          *job_name;      /* job name */
  char          *owner;         /* login of person submitting job */
  char          *groupname;     /* group name of owner's login group */
  uid_t         uid;            /* unix userid of submitter */
  gid_t         gid;            /* unix group id of submitter */
  char          *submit_host;   /* Host of job submission */
  int           steps;          /* number of steps in job */
  LL_job_step   **step_list;    /* ptr to array of ptrs to job steps */
  } LL_job;


typedef struct {
  int             version_num;
  LL_STEP_ID      StepId;
  char           *msg;
  }  LL_terminate_job_info;


typedef struct {
  int             version_num;
  LL_STEP_ID      StepId;
  char          **nodeList;
  }  LL_start_job_info;


/* API prototypes */
 
LL_element *ll_get_objs(LL_element *,int,void *,int *,int *);
LL_element *ll_next_obj(LL_element *);
int ll_free_objs(LL_element *);
int llfree_job_info(LL_job *,int);
LL_element *ll_query(int);
int ll_set_request(LL_element *,int,void *,int);
int ll_deallocate(LL_element *);
int ll_get_data(LL_element *,int,void *);
int ll_start_job(LL_start_job_info *);
int ll_terminate_job(LL_terminate_job_info *);
int ll_control(void);
int llsubmit(char *,void *,void *,LL_job *,int);

#endif /* __LLAPI */

/* END msched-llapi.h */

