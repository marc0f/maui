/*
Package:  moab
Module:   mg2.h
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

/* NOTE:  requires library 'msu' and include file 'msu.h' */

#if !defined(__G2_H) 
#define __G2_H

#define MG2_VERSION "4.2.0p0"

#if defined(__MTEST)
#define SUEMLog fprintf
#endif /* __MTEST */


/* sync w/MG2MCList[] */

enum MG2MCListEnum {
  mg2mclNONE = 0,
  mg2mclHostList,
  mg2mclState,
  mg2mclSJID };


enum G2MetaCmdEnum {
  g2mcNONE = 0,
  g2mcInitialize,
  g2mcCommit,
  g2mcList,
  g2mcRsvCreate,
  g2mcRemove,
  g2mcQuery,
  g2mcSubmit,
  g2mcModify,
  g2mcResetStats,
  g2mcRegister };

/* Callback types (sync w/MG2CBType,MG2DEF_CBMASK) */

enum MG2CallBackEnum {
  mg2cbNONE = 0,
  mg2cbRsvCreate,
  mg2cbRsvStart,
  mg2cbRsvEnd,
  mg2cbRsvDestroy,
  mg2cbJobCreate,
  mg2cbJobStart,
  mg2cbJobEnd,
  mg2cbJobDestroy };

#if !defined(__G2INTERNAL_H)

#include "msu.h" 

enum { g2ptNONE = 0, g2ptMoab, g2ptSilver };

enum { g2isNONE = 0, g2isFile };

#define __MINTERFACE /* FIXME */

#ifdef __MINTERFACE
#if !defined(__MX)
#define __MX
#endif /* __MX */

#include "moab.h"
#include "moab-proto.h"

typedef struct
  {
  char        *Name;

  mpar_t      *MPar;
  mnode_t    **MNode;
  msched_t    *Sched;
  mfsc_t      *FS;
  mjob_t     **Job;
  mres_t     **Res;
  mlog_t      *dlog;
  mrange_t    *MRange;
  mrm_t       *MRM;
  mam_t       *AM;
  mattrlist_t *AttrList;
  long        *CREndTime;
  char        *CurrentHostName;
  mulong      *PresentTime;

  int         (*G2RsvFind)();
  int         (*G2ResSetAttr)();
  int         (*G2AcctFind)();
  int         (*G2JobFind)();
  int         (*G2JobAddCreds)();
  int         (*G2JobGetResourceAvailability)();
  int         (*G2ReservationCancel)();
  int         (*G2RMJobCancel)();
  int         (*G2WikiJobLoad)();
  int         (*G2JobAllocateNodes)();
  int         (*G2JobDistributeTasks)();
  int         (*G2JobNameAdjust)();
  int         (*G2ReservationCreate)();
  int         (*G2SimJobSubmit)();
  int         (*G2RMJobSubmit)();
  } m_base_t;

#else /* __MINTERFACE */

typdef struct
  {
  char       *Name;
  } m_base_t;

#endif /* __MINTERFACE */


enum G2OEnum {
  g2oNONE,
  g2oAccount,
  g2oClass,
  g2oGroup,
  g2oJob,
  g2oNode,
  g2oQOS,
  g2oReq,
  g2oRes,
  g2oSystem,
  g2oUser };

/* suballocation policies */

enum {
  G2sapNONE = 0,
  G2sapFirstFit,
  G2sapBestFit,
  G2sapWorstFit,
  G2sapBestCRFit,
  G2sapWorstCRFit,
  G2sapBalanceAFit,
  G2sapBalanceCFit,
  G2sapBalanceARFit,
  G2sapBalanceARRFit,
  G2sapBalanceARSFit };

typedef struct
  {
  int            Type;
  int            ChargeType;
 
  unsigned long  Flags;
 
  double HourOfDayChargeFactor[24];
  double DayOfWeekChargeFactor[7];
  double QOSChargeFactor[100];
  double NodeTypeChargeFactor[100];
  } g2am_t;

typedef struct {
  su_t *su;

  m_base_t *m;

  int     SubAllocPolicy;          
  int     RMShowErrMsg;
  int     RMMaxNMThreadCount;

  g2am_t *AM;
 
  sufilebuf_t *GlobalCfgFBuf;
  sufilebuf_t *PrivateCfgFBuf;

  int   MetaUseAccountMasq;
  char  MetaAccountMasqBaseName[MAX_SUNAME];
  int   MetaAccountMasqCount;
  } G2_t;

typedef struct {
  char *UserName;
  char *GroupName;
  char *AccountName;
  } g2cred_t;

typedef struct {
  char *Arch;
  char *Features;
  int   NodeMem;
  char  NodeMemCmp;
  int   NodeProcs;
  char  NodeProcsCmp;
  char *OS;
  } g2rspec_t;

typedef struct {
  int   Disk;
  int   Mem;
  char *Network;
  int   Procs;
  int   Swap;
  } g2rreq_t;

typedef struct {
  int       Count;
  int       TPN;
  int       TPNCmp;
  g2rspec_t ResSpec;
  g2rreq_t  ResReq;
  } g2task_t;

typedef struct {
  char       *Name;
  char       *Class;
  g2cred_t    Cred;
  char       *QOSRequested;
  long        WallTime;

  int         TaskCount;
  g2task_t   *Task[16];
  suattr_t  **JobAttr;

  char       *SystemID;
  char       *SubmitScript;
  } g2job_t;

typedef struct {
  char *Name;
  } g2qos_t;

typedef struct 
  {
  char *Name;
  } g2rm_t;
 
typedef struct 
  {
  char *Name;
  } g2ocfg_t;

typedef struct
  {
  char *Name;
  } g2res_t;

typedef struct
  {
  char *Name;
  } g2tcon_t;

#define MAX_G2ACL                32

#define MAX_G2RANGE_PER_RESOURCE 32

#define MAX_G2XMLATTR  64
#define DEFAULT_G2XMLICCOUNT   16

typedef struct g2xml_s {
  char *Name;
  char *Val;

  int   ACount;
  int   ASize;

  int   CCount;
  int   CSize;

  char **AName;
  char **AVal;
 
  struct g2xml_s **C;
  } g2xml_t;


/* silver structure attributes */

/* sync w/enum SResAttr (mg2-internal.h) */

enum SResAttrEnum {
  sraNONE = 0,
  sraArchList,
  sraAttr,
  sraBandwidth,
  sraCfgNodeCount,
  sraCfgProcCount,
  sraClassList,
  sraDataDir,
  sraDataServer,
  sraDirectJobStageEnabled,
  sraFeature,
  sraFlags,
  sraFullName,
  sraKey,
  sraLocalTime,
  sraNetList,
  sraOSList,
  sraRsvOverlap,
  sraRM,
  sraSource,
  sraTimeout };



/* G2 parameter keywords */

#define G2_SUBALLOC_KEYWORD   "G2SUBALLOCPOLICY"

#endif /* !__G2INTERNAL_H */
#endif /* !__G2_H */

/* END G2.h */
