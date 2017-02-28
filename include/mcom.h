/*
Package:  moab
Module:   mcom.h
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

#ifndef __MCOM_H
#define __MCOM_H

/* core defines */

#ifndef NULL
#  define NULL (void *)0
#endif /* NULL */

#ifndef MIN
#  define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif /* MIN */

#ifndef MAX
#  define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif /* MAX */

#ifndef TRUE
#  define TRUE 1
#endif /* TRUE */

#ifndef FALSE
#  define FALSE 0
#endif /* FALSE */

#ifndef NONE
#  define NONE "[NONE]"
#endif /* NONE */

#ifndef ALL
#  define ALL "[ALL]"
#endif /* ALL */

#ifndef DEFAULT
#  define DEFAULT "[DEFAULT]"
#endif /* DEFAULT */

#ifndef SUCCESS
# define SUCCESS 1
#endif /* SUCCESS */

#ifndef FAILURE
# define FAILURE 0
#endif /* FAILURE */

#ifndef MMAX_NAME
#define MMAX_NAME     64
#endif /* MMAX_NAME */

#ifndef MMAX_LINE
#define MMAX_LINE     1024
#endif /* MMAX_LINE */

#ifndef MMAX_BUFFER
#define MMAX_BUFFER   65536
#endif /* MMAX_BUFFER */

#ifndef MCONST_CKEY
#define MCONST_CKEY "hello"
#endif /* MCONST_CKEY */

#ifndef mbool_t
# define mbool_t unsigned char
#endif /* mbool_t */

#define  CRYPTHEAD "KGV"

/* enumerations */

/* sync w/MDFormat */

enum MDataFormatEnum {
  mdfNONE = 0,
  mdfString,
  mdfInt,
  mdfLong,
  mdfDouble,
  mdfStringArray,
  mdfIntArray,
  mdfLongArray,
  mdfDoubleArray,
  mdfOther,
  mdfLAST };

enum MSocketProtocolEnum {
  mspNONE = 0,
  mspSingleUseTCP,
  mspHalfSocket,
  mspHTTPClient,
  mspHTTP,
  mspS3Challenge };

enum MWireProtocolEnum {
  mwpNONE = 0,
  mwpAVP,
  mwpXML,
  mwpHTML,
  mwpS32 };


/* sync w/MS3Action[] */

enum MS3ActionEnum {
  msssaNONE,
  msssaCancel,
  msssaCreate,
  msssaDestroy,
  msssaInitialize,
  msssaList,
  msssaModify,
  msssaNotify,
  msssaQuery,
  msssaStart,
  msssaLAST };


/* sync w/MCSAlgoType[] */

enum MChecksumAlgoEnum {
  mcsaNONE = 0,
  mcsaDES,
  mcsaHMAC,
  mcsaHMAC64,
  mcsaMD5,
  mcsaPasswd,
  mcsaRemote };


/* sync w/MS3CName[] */

enum MPeerServiceEnum {
  mpstNONE = 0,
  mpstNM,    /* system monitor     */
  mpstQM,    /* queue manager      */
  mpstSC,    /* scheduler          */
  mpstMS,    /* meta scheduler     */
  mpstPM,    /* process manager    */
  mpstAM,    /* allocation manager */
  mpstEM,    /* event manager      */
  mpstSD,    /* service directory  */
  mpstWWW }; /* web                */


/* sync w/MS3VName[] */

enum MS3VEnum {
  msssV0_2 = 0,
  msssV3_0,
  msssV4_0 };


/* const defines */

#define MMAX_SBUFFER        65536

#ifndef MMSG_BUFFER
  #define MMSG_BUFFER       (MMAX_SBUFFER << 5)
#endif /* MMSG_BUFFER */
 
#define MCONST_S3XPATH     3
#define MCONST_S3URI       "SSSRMAP3"

#define MMAX_S3ATTR        256
#define MMAX_S3VERS        4
#define MMAX_S3JACTION     64

/* default defines */

#define MDEF_CSALGO         mcsaDES
#define MMAX_SOCKETWAIT     5000000

#define MMAX_XMLATTR        64
#define MDEF_XMLICCOUNT     16

/* structures */

typedef struct mxml_s {
  char *Name;
  char *Val;

  int   ACount;
  int   ASize;

  int   CCount;
  int   CSize;

  char **AName;
  char **AVal;

  struct mxml_s **C;
  } mxml_t;


/* failure codes */

/* sync w/MFC[] */

enum MSFC {
  msfENone          = 0,     /* success */
  msfGWarning       = 100,   /* general warning */
  msfEGWireProtocol = 200,   /* general wireprotocol/network failure */
  msfEBind          = 218,   /* cannot bind socket */
  msfEGConnect      = 220,   /* general connection failure */
  msfCannotConnect  = 222,   /* cannot connect */
  msfCannotSend     = 224,   /* cannot send data */
  msfCannotRecv     = 226,   /* cannot receive data */
  msfConnRejected   = 230,   /* connection rejected */
  msfETimedOut      = 232,   /* connection timed out */
  msfEFraming       = 240,   /* general framing failure */
  msfEEOF           = 246,   /* unexpected end of file */
  msfEGMessage      = 300,   /* general message format error */
  msfENoObject      = 311,   /* no object specified in request */
  msfEGSecurity     = 400,   /* general security failure */
  msfESecClientSig  = 422,   /* security - signature creation failed at client */
  msfESecServerAuth = 424,   /* security - server auth failure */
  msfESecClientAuth = 442,   /* security - client auth failure */
  msfEGEvent        = 500,   /* general event failure */
  msfEGServer       = 700,   /* general server error */
  msfEGServerBus    = 720,   /* general server business logic failure */
  msfEGClient       = 800,   /* general client error */
  msfECInternal     = 820,   /* client internal error */
  msfECResUnavail   = 830,   /* client resource unavailable */
  msfECPolicy       = 840,   /* client policy failure */
  msfEGMisc         = 900,   /* general miscellaneous error */
  msfUnknownError   = 999 }; /* unknown failure */


/* sync w/MSockAttr[] */

enum MSocketAttrEnum {
  msockaNONE = 0,
  msockaLocalHost,
  msockaLocalPort,
  msockaRemoteHost,
  msockaRemotePort,
  msockaLAST };

#ifdef __M32COMPAT

#define mlog_t dlog_t

#endif /* __M32COMPAT */

#endif /* __MCOM_H */

/* END mcom.h */


