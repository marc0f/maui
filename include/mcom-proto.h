/*
Package:  moab
Module:   mcom-proto.h
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

/* socket util object */

int MSUInitialize(msocket_t *,char *,int,long,long);
int MSUIPCInitialize(void);
int MSUListen(msocket_t *);
int MSUConnect(msocket_t *,mbool_t,char *);
int MSUDisconnect(msocket_t *);
int MSUClose(msocket_t *);
int MSUFree(msocket_t *);
int MSUAcceptClient(msocket_t *,msocket_t *,char *,int);
int MSUSendData(msocket_t *,long,mbool_t,mbool_t);
int MSUSendPacket(int,char *,long,long,enum MStatusCodeEnum *);
int MSURecvData(msocket_t *,long,mbool_t,enum MStatusCodeEnum *,char *);
int MSURecvPacket(int,char **,long,char *,long,enum MStatusCodeEnum *);
int MSUSelectWrite(int,unsigned long);
int MSUSelectRead(int,unsigned long);
int MSUCreate(msocket_t **);
int MSUAdjustSBuffer(msocket_t *,int,mbool_t);
int MSUSetAttr(msocket_t *,enum MSocketAttrEnum,void *);
int MSUDup(msocket_t **,msocket_t *);

int MUISCreateFrame(msocket_t *,mbool_t,mbool_t);



/* sec object */

int MSecGetChecksum(char *,int,char *,char *,enum MChecksumAlgoEnum,char *);
int MSecGetChecksum2(char *,int,char *,int,char *,char *,enum MChecksumAlgoEnum,char *);
int MSecTestChecksum(char *);
int MSecBufTo64BitEncoding(char *,int,char *);
int MSecCompBufTo64BitEncoding(char *,int,char *);
int MSecComp64BitToBufDecoding(char *,int,char *,int *);
int MSecBufToHexEncoding(char *,int,char *);
int MSecCompressionGetMatch(unsigned char *,unsigned int,unsigned int,int *,unsigned int *,int *);
int MSecCompress(unsigned char *,unsigned int,unsigned char *,char *);
int MSecDecompress(unsigned char *,unsigned int,unsigned char *,unsigned int,unsigned char **,char *);
int MSecEncryption(char *,char *,int);



/* XML object */

int MXMLCreateE(mxml_t **,char *);
int MXMLDestroyE(mxml_t **);
int MXMLExtractE(mxml_t *,mxml_t *,mxml_t **);
int MXMLMergeE(mxml_t *,mxml_t *,char);
int MXMLSetAttr(mxml_t *,char *,void *,enum MDataFormatEnum);
int MXMLAppendAttr(mxml_t *,char *,char *,char);
int MXMLSetVal(mxml_t *,void *,enum MDataFormatEnum);
int MXMLAddE(mxml_t *,mxml_t *);
int MXMLSetChild(mxml_t *,char *,mxml_t **);
int MXMLToString(mxml_t *,char *,int,char **,mbool_t);
int MXMLGetAttr(mxml_t *,char *,int *,char *,int);
int MXMLGetAttrF(mxml_t *,char *,int *,void *,enum MDataFormatEnum,int);
int MXMLGetChild(mxml_t *,char *,int *,mxml_t **);
int MXMLGetChildCI(mxml_t *,char *,int *,mxml_t **);
int MXMLFromString(mxml_t **,char *,char **,char *);
int MXMLDupE(mxml_t *,mxml_t **);
mbool_t MXMLStringIsValid(char *);
int MXMLToXString(mxml_t *,char **,int *,int,char **,mbool_t);



/* sss interface object */

int MS3LoadModule(mrmfunc_t *);
int MS3DoCommand(mpsi_t *,char *,char **,mxml_t **,int *,char *);
int MS3Setup(int);
int MS3InitializeLocalQueue(mrm_t *,char *);
int MS3AddLocalJob(mrm_t *,char *);
int MS3RemoveLocalJob(mrm_t *,char *);



/* sss convenience functions */

int MS3AddSet(mxml_t *,char *,char *,mxml_t **);
int MS3AddWhere(mxml_t *,char *,char *,mxml_t **);
int MS3GetSet(mxml_t *,mxml_t **,int *,char *,int,char *,int);
int MS3GetWhere(mxml_t *,mxml_t **,int *,char *,int,char *,int);
int MS3SetObject(mxml_t *,char *,char *);
int MS3GetObject(mxml_t *,char *);
int MS3AddGet(mxml_t *,char *,mxml_t **);
int MS3GetGet(mxml_t *,mxml_t **,int *,char *,int);
int MS3SetStatus(mxml_t *,char *,enum MSFC,char *);
int MS3CheckStatus(mxml_t *,enum MSFC *,char *);

/* sss object functions */

int MS3JobToXML(mjob_t *,mxml_t **,enum MJobAttrEnum *,enum MReqAttrEnum *,char *);

/* END mcom-proto.h */

