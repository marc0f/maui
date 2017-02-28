/*
Package:  moab
Module:   moab-wiki.h
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
        
#ifndef __MWIKI_H__
#define __MWIKI_H__

#include "msched-common.h"

#define WIKI_MAX_JOB           512
#define WIKI_MAX_NODE          128
#define WIKI_MAX_FRAG_PER_JOB    8
#define WIKI_MAX_NODE_PER_JOB  256

#define DEFAULT_CONFIGFILE       "/etc/rm.cfg"

#define DEFAULT_WMTCPPORT    30002
#define DEFAULT_WMUDPPORT    30003
#define DEFAULT_JMTCPPORT    30004
#define DEFAULT_JMUDPPORT    30005
#define DEFAULT_NMTCPPORT    30006
#define DEFAULT_TMTCPPORT    30007

#define DEFAULT_WMTIMEOUT    10000000
#define DEFAULT_JMTIMEOUT    10000000
#define DEFAULT_NMTIMEOUT    10000000
#define DEFAULT_TMTIMEOUT    10000000

#define DEFAULT_JOBMANCOMMAND  "/usr/local/bin/jobman"
#define DEFAULT_NODEMANCOMMAND "/usr/local/bin/nodeman"
#define DEFAULT_TASKMANCOMMAND "/usr/local/bin/taskman"
#define DEFAULT_RSHCOMMAND     "rsh"

#define DEFAULT_LOCALFILESYSTEM "/tmp"

#define MAX_HIER_CHILD          32
#define MAX_CHILD_PER_LEVEL    128

#define MAX_WIKINODEATTR        22
#define MAX_WIKIJOBATTR         38  

enum { 
  sigAlive = 0
};

enum {
  cmdWMNone = 0,
  cmdWMSubmitJob,
  cmdWMGetJobs,
  cmdWMGetNodes,
  cmdWMUpdateJob,
  cmdWMStartJob,
  cmdWMCancelJob,
  cmdWMSuspendJob,
  cmdWMResumeJob,
  cmdWMCheckPointJob,
  cmdWMMigrateJob,
  cmdWMPurgeJob,
  cmdWMStartTask,
  cmdWMKillTask,
  cmdWMSuspendTask,
  cmdWMResumeTask,
  cmdWMGetTaskStatus,
  cmdWMGetNodeStatus,
  cmdWMGetJobStatus,
  cmdWMUpdateTaskStatus,
  cmdWMUpdateNodeStatus,
  cmdWMUpdateJobStatus,
  cmdWMAllocateNode,
  cmdWMStartNodeman,
  cmdWMSpawnTaskman,
  cmdWMSpawnJobman,
  cmdWMReconfig,
  cmdWMShutdown,
  cmdWMDiagnose,
  cmdWMReparent
  };

enum {
  woNone = 0,
  woNodeStatus,
  woJobStatus
  };

enum {
  wnaNone = 0,
  wnaUpdateTime,
  wnaState,
  wnaOS,
  wnaArch,
  wnaCacheSize,
  wnaL2CacheSize,
  wnaConfMemory,
  wnaAvailMemory,
  wnaConfSwap,
  wnaAvailSwap,
  wnaConfLocalDisk,
  wnaAvailLocalDisk,
  wnaConfProcCount,
  wnaAvailProcCount,
  wnaConfNetworkList,
  wnaAvailNetworkList,
  wnaCPULoad,
  wnaMaxTasks
  };

enum {
  weNone = 0,
  weNoReport,
  weNoParent
  };

enum {
  wsNone = 0,
  wsJob,
  wsNode
  };

typedef struct {
  char  Name[MAX_MNAME];

  long  MTime;

  char  State[MAX_MNAME];

  char  OS[MAX_MNAME];
  char  Arch[MAX_MNAME];

  int   CacheSize;
  int   L2CacheSize;

  int   ConfMemory;
  int   AvailMemory;

  int   ConfSwap;
  int   AvailSwap;

  int   ConfLocalDisk;
  int   AvailLocalDisk;

  int   ConfProcCount;
  int   AvailProcCount;

  char  ConfNetworkList[MAX_MLINE];
  char  AvailNetworkList[MAX_MLINE];

  double CPULoad;

  int   MaxTasks;
  } wiki_node_t;

typedef struct {
  char Arch[MAX_MNAME];
  char OS[MAX_MNAME];
  char Network[MAX_MNAME];
  char Features[MAX_MNAME];
  
  int  ProcCount;
  int  ProcCountCmp;
  int  Memory;
  int  MemCmp;
  int  Disk;
  int  DiskCmp;
  int  Swap;
  int  SwapCmp;

  char IWD[MAX_PATH_LEN];
  char Args[MAX_MNAME];
  char Env[MAX_MNAME];
  char Input[MAX_PATH_LEN];
  char Output[MAX_PATH_LEN];
  char Error[MAX_PATH_LEN];
  char Exec[MAX_PATH_LEN];
  char Shell[MAX_PATH_LEN];
  
  time_t QueueTime;
  time_t StartTime;
  time_t CompletionTime;

  int   Status;

  char  JobMonServer[MAX_MNAME];
  int   JobMonPort;

  char  HostName[MAX_MNODE_PER_FRAG + 1][MAX_MNAME];
  char  HostStatus[MAX_MNODE_PER_FRAG + 1];

  short TaskList[MAX_MTASK_PER_FRAG + 1];

  int   TaskCount;

  char Buf[5 * MAX_MNAME];
  } wiki_frag_t;

typedef struct {
  char Name[MAX_MNAME];

  long MTime;

  int  Status;

  long StateMTime;

  int  Version;

  int  WCLimit;

  int  UID;
  int  GID;
  char Account[MAX_MNAME];

  char PartitionList[MAX_MNAME];

  wiki_frag_t Frag[WIKI_MAX_FRAG_PER_JOB];
  int         FragCount;

  char        NodeList[MAX_MNODE_PER_JOB + 1][MAX_MNAME];
  short       TaskMap[MAX_MTASK_PER_JOB + 1];

  char        JobmanHost[MAX_MNAME];
  int         JobmanPort;
  } wiki_job_t;

typedef struct {
  char Name[MAX_MNAME];

  char Exec[MAX_PATH_LEN];

  char Shell[MAX_PATH_LEN];

  char Args[MAX_MNAME];

  char Input[MAX_PATH_LEN];
  char Output[MAX_PATH_LEN];
  char Error[MAX_PATH_LEN];

  char IWD[MAX_PATH_LEN];

  char Env[MAX_MLINE];

  int  UID;
  int  GID;

  int  TID;
  int  PID;
  int  SID;
} wiki_task_t;

typedef struct {
  char HostName[MAX_MNAME];
  int  MsgType;
  int  Port;
  } wiki_jobstatus_t;

typedef struct {
  char Buf[MAX_MLINE];
  } wiki_stat_t;

enum MWNodeAttrEnum {
  mwnaNONE = 0,
  mwnaUpdateTime,
  mwnaState,
  mwnaOS,
  mwnaArch,
  mwnaCMemory,
  mwnaAMemory,
  mwnaCSwap,
  mwnaASwap,
  mwnaCDisk,
  mwnaADisk,
  mwnaCProc, 
  mwnaAProc,
  mwnaCNet,
  mwnaANet,
  mwnaCRes,
  mwnaARes,
  mwnaCPULoad,
  mwnaCClass,
  mwnaAClass,
  mwnaFeature,
  mwnaPartition,
  mwnaEvent,
  mwnaCurrentTask,
  mwnaMaxTask,
  mwnaSpeed,
  mwnaName
  };

enum MWJobAttrEnum {
  mwjaNONE = 0,
  mwjaUpdateTime,
  mwjaState,
  mwjaWCLimit,
  mwjaTasks,
  mwjaNodes,
  mwjaGeometry,
  mwjaQueueTime,
  mwjaStartDate,
  mwjaStartTime,
  mwjaCompletionTime,
  mwjaUName,
  mwjaGName,
  mwjaAccount,
  mwjaRFeatures,
  mwjaRNetwork,
  mwjaDNetwork,
  mwjaRClass,
  mwjaROpsys,
  mwjaRArch,
  mwjaRMem,
  mwjaRMemCmp,
  mwjaDMem,
  mwjaRDisk,
  mwjaRDiskCmp,
  mwjaDDisk,
  mwjaRSwap,
  mwjaRSwapCmp,
  mwjaDSwap,
  mwjaPartitionList,
  mwjaExec,
  mwjaArgs,
  mwjaIWD,
  mwjaComment,
  mwjaRejectionCount,
  mwjaRejectionMessage,
  mwjaRejectionCode,
  mwjaEvent,
  mwjaTaskList,
  mwjaTaskPerNode,
  mwjaQOS,
  mwjaEndDate,
  mwjaDProcs,
  mwjaHostList,
  mwjaSuspendTime,
  mwjaResAccess,
  mwjaName,
  mwjaEnv,
  mwjaInput,
  mwjaOutput,
  mwjaError,
  mwjaFlags,
  mwjaSubmitString,
  mwjaSubmitType
  };

#endif /* __MWIKI_H__ */

