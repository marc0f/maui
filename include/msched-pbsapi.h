/*
Package:  moab
Module:   msched-pbsapi.h
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

#ifndef __PBSAPI
#define __PBSAPI

/* structures */

struct attrl {
  struct attrl *next;
  char         *name;
  char         *resource;
  char         *value;
  int           op;    /* not used */
  };


struct batch_status {
  struct batch_status *next;
  char                *name;
  struct attrl        *attribs;
  char                *text;
  };

/* prototypes */

struct batch_status *pbs_statnode();
struct batch_status *pbs_statjob();
struct batch_status *pbs_statque(int, char*, attrl*, char*);

int                  pbs_statfree(batch_status*);

int                  pbs_runjob(int, char*, char*, char*);
int                  pbs_alterjob(int, char*, attrl*, char*);
int                  pbs_deljob(int, char*, char*);
int                  pbs_stagein(int,char*,char*,char*);
int                  pbs_rerunjob(int, char*, char*);

char                *pbs_geterrmsg(int);

int                  openrm(char *,int);
int                  closerm(int);

int                  pbs_connect();
int                  pbs_disconnect();

int                  addreq(int,char*);
char                *getreq(int);


/* defines */

#ifndef ND_free

#define ND_free                 "free"
#define ND_offline              "offline"
#define ND_down                 "down"
#define ND_reserve              "reserve"
#define ND_job_exclusive        "job-exclusive"
#define ND_job_sharing          "job-sharing"
#define ND_busy                 "busy"
#define ND_state_unknown        "state-unknown"
#define ND_timeshared           "time-shared"
#define ND_cluster              "cluster"

#endif /* ND_free */

#ifndef ATTR_NODE_state

#define ATTR_NODE_state         "state"
#define ATTR_NODE_np            "np"
#define ATTR_NODE_properties    "properties"
#define ATTR_NODE_ntype         "ntype"
#define ATTR_NODE_jobs          "jobs"

#define ATTR_a "Execution_Time"
#define ATTR_c "Checkpoint"
#define ATTR_e "Error_Path"
#define ATTR_g "group_list"
#define ATTR_h "Hold_Types"
#define ATTR_j "Join_Path"
#define ATTR_k "Keep_Files"
#define ATTR_l "Resource_List"
#define ATTR_m "Mail_Points"
#define ATTR_o "Output_Path"
#define ATTR_p "Priority"
#define ATTR_q "destination"
#define ATTR_r "Rerunable"
#define ATTR_u "User_List"
#define ATTR_v "Variable_List"
#define ATTR_A "Account_Name"
#define ATTR_M "Mail_Users"
#define ATTR_N "Job_Name"
#define ATTR_S "Shell_Path_List"

#define ATTR_depend   "depend"
#define ATTR_inter    "interactive"
#define ATTR_stagein  "stagein"
#define ATTR_stageout "stageout"
#define ATTR_ctime      "ctime"
#define ATTR_exechost   "exec_host"
#define ATTR_mtime      "mtime"
#define ATTR_qtime      "qtime"
#define ATTR_session    "session_id"
#define ATTR_euser      "euser"
#define ATTR_egroup     "egroup"
#define ATTR_hashname   "hashname"
#define ATTR_hopcount   "hop_count"
#define ATTR_security   "security"
#define ATTR_sched_hint "sched_hint"
#define ATTR_substate   "substate"
#define ATTR_name       "Job_Name"
#define ATTR_owner      "Job_Owner"
#define ATTR_used       "resources_used"
#define ATTR_state      "job_state"
#define ATTR_queue      "queue"
#define ATTR_server     "server"
#define ATTR_maxrun     "max_running"
#define ATTR_total      "total_jobs"
#define ATTR_comment    "comment"
#define ATTR_cookie     "cookie"
#define ATTR_qrank      "queue_rank"
#define ATTR_altid      "alt_id"
#define ATTR_etime      "etime"
#define ATTR_aclhten    "acl_host_enable"
#define ATTR_aclhost    "acl_hosts"

#endif /* ATTR_NODE_state */

enum batch_op { SET, UNSET, INCR, DECR,
                EQ, NE, GE, GT, LE, LT, DFLT
};

#endif /* __PBSAPI */

/* END maui-pbsapi.h */

