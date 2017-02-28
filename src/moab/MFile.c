/*
Package:  moab
Module:   MFile.c
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
        
/* Contains:                                          *
 *                                                    *
 *  char *MFULoad(FileName,BlockSize,AccessMode,BlockCount,SC) *
 *  int __MFUGetCachedFile(FileName,Buffer,BufSize)   *
 *  int __MFUCacheFile(FileName,Buffer,BufSize)       *
 *                                                    */


#include "moab.h"
#include "msched-proto.h"



extern mlog_t mlog;

mulong    *STime;

mfcache_t  MFileCache[MAX_FILECACHE];   
mfindex_t  MIndexState[MAX_FILECACHE];

/* local prototypes */

int __MFUGetCachedFile(char *,char **,int *);
int __MFUCacheFile(char *,char *,int); 




int MFUCacheInitialize(

  mulong *SysTime) /* I */

  {
  int index;

  const char *FName = "MFUCacheInitialize";

  DBG(5,fCORE) DPrint("%s(SysTime)\n",
    FName);

  STime = SysTime;

  for (index = 0;index < MAX_FILECACHE;index++)
    {
    if (MFileCache[index].Buffer != NULL)
      free(MFileCache[index].Buffer);
    }  /* END for (index) */

  memset(MFileCache,0,sizeof(MFileCache));
  memset(MIndexState,0,sizeof(MIndexState));

  return(SUCCESS);
  }  /* END MFUCacheInitialize() */




char *MFULoad(

  char *FileName,
  int   BlockSize,
  int   AccessMode,
  int  *BlockCount,
  int  *SC)

  {
  FILE        *dfp = NULL;

  char        *ptr;
  char        *buf;
  int          BufSize;

  int          count;

  int          ReadCount;

  struct stat  sbuf;

  const char *FName = "MFULoad";

  DBG(5,fCORE) DPrint("%s(%s,%d,%s,BlockCount,SC)\n",
    FName,
    (FileName != NULL) ? FileName : "NULL",
    BlockSize,
    (AccessMode == macmRead) ? "READ" : "WRITE");

  if (SC != NULL)
    *SC = mscNoError;

  /* check if file is cached */

  if ((FileName != NULL) &&
      (FileName[0] != '\0') &&
      (__MFUGetCachedFile(FileName,&buf,&BufSize) == SUCCESS))
    {
    if (BlockCount != NULL)
      *BlockCount = BufSize / BlockSize;

    if (AccessMode == macmRead)
      {
      /* use cached data */

      return(buf);
      }
    else
      {
      /* copy cached data */
      if ((ptr = (char *)calloc(BufSize + 1, 1)) == NULL)
        {
        MOSSyslog(LOG_ERR,"cannot calloc buffer for file '%s', errno: %d (%s)",
          FileName,
          errno,
          strerror(errno));

        DBG(2,fCORE) DPrint("ERROR:    cannot calloc file buffer for file '%s', errno: %d (%s)\n",
          FileName,
          errno,
          strerror(errno));

        if (SC != NULL)
          *SC = mscNoEnt;

        return(NULL);
        }
      else
        {
        /* fill calloc'd buffer */

        memcpy(ptr,buf,BufSize + 1);

        DBG(9,fCORE) DPrint("INFO:     cached file data copied from %p to %p\n",
          buf,
          ptr);

        return(ptr);
        }
      }   /* else (AccessMode == macmRead) */

    /* always return */
    }     /* END if (__MFUGetCachedFile() == SUCCESS) */

  /* initialize block size to legal value if not previously set */

  if (BlockSize < 1)
    BlockSize = 1;

  if (FileName != NULL)
    {
    if (stat(FileName,&sbuf) == -1)
      {
      DBG(2,fCORE) DPrint("ERROR:    cannot stat file '%s', errno: %d (%s)\n",
        FileName,
        errno,
        strerror(errno));

      if (SC != NULL)
        *SC = mscNoEnt;

      return(NULL);
      }

    ReadCount = sbuf.st_size / BlockSize;

    if ((dfp = fopen(FileName,"r")) == NULL)
      {
      DBG(2,fCORE) DPrint("ERROR:    cannot open file '%s', errno: %d (%s)\n",
        FileName,
        errno,
        strerror(errno));

      if (SC != NULL)
        *SC = mscNoEnt;
 
      return(NULL);
      }
    }
  else
    {
    dfp = stdin;

    ReadCount = MAX_FBUFFER / BlockSize;
    }

  BufSize = ReadCount * BlockSize;

  DBG(5,fCORE) DPrint("INFO:     new file '%s' opened with %d bytes (ReadCount = %d)\n",
    FileName,
    BufSize,
    ReadCount);
  if ((ptr = (char *)calloc(BufSize + 1,1)) == NULL)
    {
    MOSSyslog(LOG_ERR,"cannot calloc %d byte buffer for file '%s', errno: %d (%s)",
      BufSize,
      FileName,
      errno,
      strerror(errno));

    DBG(2,fCORE) DPrint("ERROR:    cannot calloc file buffer for file '%s', errno: %d (%s)\n",
      FileName,
      errno,
      strerror(errno));

    if (SC != NULL)
      *SC = mscNoMemory;

    if (dfp != stdin)
      fclose(dfp);
 
    return(NULL);
    }

  if ((count = (int)fread(ptr,ReadCount,BlockSize,dfp)) < 0)
    {
    MOSSyslog(LOG_ERR,"cannot read file '%s', errno: %d (%s)",
      FileName,
      errno,
      strerror(errno));

    DBG(2,fCORE) DPrint("ERROR:    cannot read file '%s', errno: %d (%s)\n",
      FileName,
      errno,
      strerror(errno));

    free(ptr);

    if (SC != NULL)
      *SC = mscNoEnt;

    if (dfp != stdin)
      fclose(dfp);

    return(NULL);
    }

  if (dfp != stdin)
    fclose(dfp);

  if (BlockCount != NULL)
    *BlockCount = count;

  ptr[BufSize] = '\0';

  __MFUCacheFile(FileName,ptr,BufSize);

  if (AccessMode == macmRead)
    {
    /* use cached data */

    free(ptr);

    if (__MFUGetCachedFile(FileName,&buf,&BufSize) == SUCCESS)
      {
      if (BlockCount != NULL)
        *BlockCount = BufSize / BlockSize;

      return(buf);
      }
    else
      {
      if (SC != NULL)
        *SC = mscNoEnt;

      return(NULL);
      } 
    }    /* END if (AccessMode == macmRead) */

  return(ptr);
  }  /* END MFULoad() */





int __MFUGetCachedFile(

  char  *FileName,  /* I */
  char **Buffer,    /* O */
  int   *BufSize)   /* I */

  {
  int  index;
  long mtime;

  const char *FName = "__MFUGetCachedFile";

  DBG(5,fSTRUCT) DPrint("%s(%s,Buffer,BufSize)\n",
    FName,
    (FileName != NULL) ? FileName : "NULL");

  if (Buffer == NULL)
    {
    return(FAILURE);
    }

  *Buffer = NULL;

  if ((FileName == NULL) || (FileName[0] == '\0'))
    {
    return(FAILURE);
    }

  for (index = 0;index < MAX_FILECACHE;index++)
    {
    if (MFileCache[index].FileName[0] == '\0')
      continue;

    if (strcmp(MFileCache[index].FileName,FileName))
      continue;

    /* cache entry located */

    if (MFileCache[index].ProgTimeStamp == 
       ((STime != NULL) ? *STime : 0))
      {
      if (MFileCache[index].Buffer == NULL)
        {
        DBG(5,fSTRUCT) DPrint("ALERT:    filecache buffer is empty for file '%s'\n",
          FileName);

        return(FAILURE);
        }

      *Buffer  = MFileCache[index].Buffer;
      *BufSize = MFileCache[index].BufSize;
 
      DBG(5,fSTRUCT) DPrint("INFO:     cached file '%s' located in slot %d (%d Bytes)\n",
        FileName,
        index,
        (int)strlen(*Buffer));

      return(SUCCESS);
      }

    if (MFUGetInfo(FileName,&mtime,NULL,NULL) == FAILURE)
      {
      DBG(5,fSTRUCT) DPrint("ALERT:    cannot determine modify time for file '%s'\n",
        FileName);

      return(FAILURE);
      }

    if (mtime > MFileCache[index].FileTimeStamp)
      {
      DBG(5,fSTRUCT) DPrint("INFO:     file '%s' has been modified\n",
        FileName);

      return(FAILURE);
      }

    /* file cache is current */

    *Buffer  = MFileCache[index].Buffer;
    *BufSize = MFileCache[index].BufSize;

    return(SUCCESS);
    }     /* END for (index) */

  /* file cache not found */

  DBG(6,fCORE) DPrint("INFO:     file '%s' not cached\n",
    FileName);

  return(FAILURE);
  }  /* END __MFUGetCachedFile() */





int __MFUCacheFile(

  char *FileName,  /* I */
  char *Buffer,    /* I */
  int   BufSize)   /* I */

  {
  int index;

  time_t now;

  const char *FName = "__MFUCacheFile";

  DBG(5,fSTRUCT) DPrint("%s(%s,Buffer,%d)\n",
    FName,
    FileName,
    BufSize);

  /* look for existing cache entry */

  for (index = 0;index < MAX_FILECACHE;index++)
    {
    if (MFileCache[index].FileName[0] == '\0')
      continue;

    if (strcmp(MFileCache[index].FileName,FileName))
      continue;

    if (MFileCache[index].Buffer != NULL)
      {
      free(MFileCache[index].Buffer);

      MFileCache[index].Buffer = NULL;
      }
    if ((MFileCache[index].Buffer = (char *)calloc(BufSize + 1, 1)) == NULL)
      {
      MOSSyslog(LOG_ERR,"cannot calloc cache buffer for file '%s', errno: %d (%s)",
        FileName,
        errno,
        strerror(errno));

      DBG(2,fSTRUCT) DPrint("ERROR:    cannot calloc file buffer for file '%s', errno: %d (%s)\n",
        FileName,
        errno,
        strerror(errno));

      return(FAILURE);
      }

    memcpy(MFileCache[index].Buffer,Buffer,BufSize);

    MFileCache[index].Buffer[BufSize] = '\0';

    MFileCache[index].BufSize         = BufSize;

    MFileCache[index].ProgTimeStamp   = (STime != NULL) ? *STime : 0;

    time(&now);

    MFileCache[index].FileTimeStamp = now;

    DBG(5,fSTRUCT) DPrint("INFO:     file '%s' cached in slot %d (%d bytes)\n",
      FileName,
      index,
      BufSize);

    return(SUCCESS);
    }  /* END for (index) */

  /* create new cache entry */

  for (index = 0;index < MAX_FILECACHE;index++)
    {
    if (MFileCache[index].FileName[0] == '\0')
      {
      /* calloc space for buffer */

      if ((MFileCache[index].Buffer = (char *)calloc(BufSize + 1, 1)) == NULL)
        {
        MOSSyslog(LOG_ERR,"cannot calloc cache buffer for file '%s', errno: %d (%s)",
          FileName,
          errno,
          strerror(errno));

        DBG(2,fSTRUCT) DPrint("ERROR:    cannot calloc cache buffer for file '%s', errno: %d (%s)\n",
          FileName,
          errno,
          strerror(errno));

        return(FAILURE);
        }

      strcpy(MFileCache[index].FileName,FileName);

      memcpy(MFileCache[index].Buffer,Buffer,BufSize);

      MFileCache[index].Buffer[BufSize] = '\0';

      MFileCache[index].BufSize         = BufSize;

      MFileCache[index].ProgTimeStamp   = (STime != NULL) ? *STime : 0;

      time(&now);

      MFileCache[index].FileTimeStamp = now;

      DBG(5,fSTRUCT) DPrint("INFO:     new file '%s' cached in slot %d (%d bytes)\n",
        FileName,
        index,
        BufSize);

      return(SUCCESS);
      }
    }    /* END for index */

  DBG(1,fSTRUCT) DPrint("ALERT:    file cache overflow while attempting to cache file '%s'\n",
    FileName);

  return(FAILURE);
  }  /* END __MFUCacheFile() */





int MFUIndexInitialize(

  char *FileName,   /* I */
  int   RecordSize, /* I */
  int  *SC)         /* O */

  {
  char IndexFileName[MAX_PATH_LEN];
  char DataFileName[MAX_PATH_LEN];

  FILE *ifp;

  int   RecordCount;
  int   count;
  int   index;

  struct stat sbuf;

  rindex_t rindex;

  rindex_t *Index;

  const char *FName = "MFUIndexInitialize";

  DBG(1,fSTRUCT) DPrint("%s(%s,%d,SC)\n",
    FName,
    FileName,
    RecordSize);

  if (SC != NULL)
    *SC = mscNoError;

  sprintf(IndexFileName,"%s.ind",
    FileName);

  sprintf(DataFileName,"%s.dat",
    FileName);

  /* check index file */

  if (stat(IndexFileName,&sbuf) == -1)
    {
    if (errno != ENOENT)
      {
      DBG(2,fCORE) DPrint("INFO:     cannot stat file '%s', errno: %d (%s)\n",
        IndexFileName,
        errno,
        strerror(errno));

      if (SC != NULL)
        *SC = mscSysFailure;

      return(FAILURE);
      }

    /* create new index file */

    if ((ifp = fopen(IndexFileName,"w+")) == NULL)
      {
      DBG(2,fCORE) DPrint("WARNING:  cannot open index file '%s', errno: %d (%s)\n",
        IndexFileName,
        errno,
        strerror(errno));

      if (SC != NULL)
        *SC = mscSysFailure;

      return(FAILURE);
      }

    rindex.Offset = RecordSize;

    RecordCount = 1;

    sprintf(rindex.Key,"%08d",
      RecordCount);

    if (fwrite(&rindex,sizeof(rindex_t),1,ifp) < 1)
      {
      DBG(2,fCORE) DPrint("WARNING:  cannot write to index file '%s', errno: %d (%s)\n",
        IndexFileName,
        errno,
        strerror(errno));

      fclose(ifp);

      if (SC != NULL)
        *SC = mscSysFailure;

      return(FAILURE);
      }

    fclose(ifp);
    }
  else
    {
    if ((Index = (rindex_t *)MFULoad(IndexFileName,sizeof(rindex_t),macmRead,&count,NULL)) == NULL)
      {
      DBG(2,fCORE) DPrint("WARNING:  cannot load file '%s', errno: %d (%s)\n",
        IndexFileName,
        errno,
        strerror(errno));

      if (SC != NULL)
        *SC = mscNoEnt;

      return(FAILURE);
      }

    RecordCount = (int)strtol(Index[0].Key,NULL,0);
    }  /* END else (stat(IndexFileName,&sbuf) == -1) */ 

  for (index = 0;index < MAX_FILECACHE;index++)
    {
    if (!strcmp(MIndexState[index].FileName,FileName))
      {
      MIndexState[index].Index = RecordCount;

      break;
      }

    if (MIndexState[index].FileName[0] == '\0')
      {
      strcpy(MIndexState[index].FileName,FileName);

      MIndexState[index].Index = RecordCount;

      break;
      }
    }    /* END for (index) */
  
  MFUCacheInvalidate(IndexFileName);
 
  /* check data file */
 
  if (stat(DataFileName,&sbuf) == -1)
    {
    if (errno != ENOENT)
      {
      DBG(2,fCORE) DPrint("INFO:     cannot stat file '%s', errno: %d (%s)\n",
        DataFileName,
        errno,
        strerror(errno));

      if (SC != NULL)
        *SC = mscSysFailure;

      return(FAILURE);
      }

    /* create new data file */

    if ((ifp = fopen(DataFileName,"w+")) == NULL)
      {
      fprintf(stderr,"WARNING:  cannot open file '%s', errno: %d (%s)\n",
        DataFileName,
        errno,
        strerror(errno));

      if (SC != NULL)
        *SC = mscSysFailure;

      return(FAILURE);
      }

    fclose(ifp);
    }

  MFUCacheInvalidate(DataFileName);

  return(SUCCESS);
  }  /* END MFUIndexInitialize() */   





int MFUGetRecord(

  char  *FileName,
  char  *Key,
  char **Buffer,
  int   *RecordCount,
  int   *SC)

  {
  char  IndexFileName[MAX_PATH_LEN];
  char  DataFileName[MAX_PATH_LEN];

  char *dptr;

  rindex_t *Index;

  int       index;

  int       dcount;
  int       icount;

  int       rcount;

  char     *head;

  int       RecSize;
 
  const char *FName = "MFUGetRecord";

  DBG(3,fCORE) DPrint("%s(%s,%s,Buffer,RecordCount,SC)\n",
    FName,
    FileName,
    Key);

  if (SC != NULL)
    *SC = mscNoError;

  sprintf(IndexFileName,"%s.ind",
    FileName);

  sprintf(DataFileName,"%s.dat",
    FileName);

  if ((Index = (rindex_t *)MFULoad(IndexFileName,sizeof(rindex_t),macmRead,&icount,SC)) == FAILURE)
    {
    DBG(2,fCORE) DPrint("ALERT:    cannot load '%s' index file\n",
      IndexFileName);

    return(FAILURE);
    }

  RecSize = Index[0].Offset;

  if ((dptr = MFULoad(DataFileName,RecSize,macmRead,&dcount,SC)) == FAILURE)
    {
    DBG(2,fCORE) DPrint("ALERT:    cannot load '%s' index file\n",
      DataFileName);

    return(FAILURE);
    }

  rcount = 0;

  for (index = 1;index < icount;index++)
    {
    if (Index[index].Key[0] == '\0')
      continue;

    if (Key == NULL)
      {
      rcount++;
      }
    else if (Key[0] == '\0')
      {
      rcount++;
      }
    else if (!strcmp(Index[index].Key,Key))
      {
      if ((*Buffer = (char *)calloc(RecSize, 1)) == NULL)
        {
        MOSSyslog(LOG_ERR,"cannot calloc buffer for record '%s', errno: %d (%s)",
          Key,
          errno,
          strerror(errno));

        DBG(2,fCORE) DPrint("ERROR:    cannot calloc %d buffer for record '%s', errno: %d (%s)\n",
          RecSize,
          Key,
          errno,
          strerror(errno));

        if (SC != NULL)
          *SC = mscNoMemory;

        return(FAILURE);
        }

      memcpy(*Buffer,&dptr[Index[index].Offset],RecSize);

      *RecordCount = 1;
     
      return(SUCCESS);
      }
    }    /* END for (index) */

  /* all records requested */

  if (rcount == 0)
    {
    *Buffer = NULL;

    *RecordCount = 0;

    return(SUCCESS);
    }

  if ((*Buffer = (char *)calloc(rcount, RecSize)) == NULL)
    {
    MOSSyslog(LOG_ERR,"cannot calloc buffer for all records, errno: %d (%s)",
      errno,
      strerror(errno));

    DBG(2,fCORE) DPrint("ERROR:    cannot calloc %d byte buffer for all records, errno: %d (%s)\n",
      rcount * RecSize,
      errno,
      strerror(errno));

    if (SC != NULL)
      *SC = mscNoMemory;

    return(FAILURE);
    }

  head = *Buffer;

  for (index = 1;index < icount;index++)
    {
    if (Index[index].Key[0] == '\0')
      continue;

    memcpy(head,&dptr[Index[index].Offset],RecSize);

    head += RecSize;
    }
 
  *RecordCount = rcount;
 
  return(SUCCESS);
  }  /* END MFUGetRecord() */





int MFUPutRecord(

  char *FileName,
  char *Key,
  char *Buffer,
  int  *SC)

  {
  char  IndexFileName[MAX_PATH_LEN];
  char  DataFileName[MAX_PATH_LEN];

  FILE *ifp;
  FILE *dfp;

  rindex_t *Index;

  rindex_t  HeadIndex;
  rindex_t  NewIndex;

  int       index;

  int       bestindex;

  int       count;

  int       RecSize;
  int       Offset;

  int       Counter;

  const char *FName = "MFUPutRecord";

  DBG(3,fCORE) DPrint("%s(%s,%s,Buffer,SC)\n",
    FName,
    FileName,
    Key);

  if (SC != NULL)
    *SC = mscNoError;

  sprintf(IndexFileName,"%s.ind",
    FileName);

  sprintf(DataFileName,"%s.dat",
    FileName);

  if ((Index = (rindex_t *)MFULoad(IndexFileName,sizeof(rindex_t),macmRead,&count,NULL)) == FAILURE)
    {
    DBG(2,fCORE) DPrint("ALERT:    cannot load '%s' index file\n",
      IndexFileName);

    return(FAILURE);
    }

  DBG(5,fCORE) DPrint("INFO:     index file '%s' loaded with %d records\n",
    IndexFileName,
    count - 1);

  bestindex = -1;

  RecSize = Index[0].Offset;

  for (index = 1;index < count;index++)
    {
    if ((Index[index].Key[0] == '\0') && (bestindex == -1))
      bestindex = index;

    if (!strcmp(Index[index].Key,Key))
      {
      bestindex = index;

      break;
      }
    }

  if (bestindex == -1)
    bestindex = count;

  Offset = (bestindex - 1) * RecSize;

  strncpy(NewIndex.Key,Key,sizeof(NewIndex.Key));
  NewIndex.Key[sizeof(NewIndex.Key) - 1] = '\0';

  NewIndex.Offset = Offset;

  /* write data record */

  if ((dfp = fopen(DataFileName,"rw+")) == NULL)
    {
    DBG(1,fCORE) DPrint("ALERT:    cannot open file '%s', errno: %d (%s)\n",
      DataFileName,
      errno,
      strerror(errno));

    return(FAILURE);
    }

  if (fseek(dfp,Offset,SEEK_SET) == -1)
    {
    DBG(1,fCORE) DPrint("ALERT:    cannot seek to offset %d in file '%s', errno: %d (%s)\n",
      Offset,
      DataFileName,
      errno,
      strerror(errno));

    fclose(dfp);

    return(FAILURE);
    }

  if (fwrite(Buffer,RecSize,1,dfp) < 1)
    {
    DBG(1,fCORE) DPrint("WARNING:  cannot write to file '%s', errno: %d (%s)\n",
      DataFileName,
      errno,
      strerror(errno));

    fclose(dfp);

    return(FAILURE);
    }

  fclose(dfp);

  MFUCacheInvalidate(DataFileName);

  /* increment name counter */

  MFUGetCurrentIndex(FileName,&Counter,1);

  HeadIndex.Offset = RecSize;

  sprintf(HeadIndex.Key,"%08d",
    Counter);

  /* write index record */

  if ((ifp = fopen(IndexFileName,"rw+")) == NULL)
    {
    DBG(1,fCORE) DPrint("ALERT:    cannot open file '%s', errno: %d (%s)\n",
      IndexFileName,
      errno,
      strerror(errno));

    return(FAILURE);
    }

  if (fseek(ifp,0,SEEK_SET) == -1)
    {
    DBG(1,fCORE) DPrint("ALERT:    cannot seek to offset %d in file '%s', errno: %d (%s)\n",
      Offset,
      IndexFileName,
      errno,
      strerror(errno));

    fclose(dfp);

    return(FAILURE);
    }

  if (fwrite(&HeadIndex,sizeof(rindex_t),1,ifp) < 1)
    {
    DBG(1,fCORE) DPrint("WARNING:  cannot write to file '%s', errno: %d (%s)\n",
      IndexFileName,
      errno,
      strerror(errno));

    fclose(dfp);

    return(FAILURE);
    }

  if (fseek(ifp,bestindex * sizeof(rindex_t),SEEK_SET) == -1)
    {
    DBG(1,fCORE) DPrint("ALERT:    cannot seek to offset %d in file '%s', errno: %d (%s)\n",
      (int)(bestindex * sizeof(rindex_t)),
      IndexFileName,
      errno,
      strerror(errno));

    fclose(ifp);

    return(FAILURE);
    }

  if (fwrite(&NewIndex,sizeof(rindex_t),1,ifp) < 1)
    {
    DBG(1,fCORE) DPrint("WARNING:  cannot write to file '%s', errno: %d (%s)\n",
      IndexFileName,
      errno,
      strerror(errno));

    fclose(ifp);

    return(FAILURE);
    }

  fclose(ifp);

  MFUCacheInvalidate(IndexFileName);
 
  return(SUCCESS);
  }  /* END MFUPutRecord() */





int MFURemoveRecord(

  char *FileName,
  char *Key,
  int  *SC)

  {
  char  IndexFileName[MAX_PATH_LEN];

  int   count;
  int   index;

  FILE *ifp;

  rindex_t *Index;

  rindex_t  tmpIndex;

  const char *FName = "MFURemoveRecord";

  DBG(2,fCORE) DPrint("%s(%s,%s,SC)\n",
    FName,
    FileName,
    Key);

  sprintf(IndexFileName,"%s.ind",
    FileName);

  if ((Index = (rindex_t *)MFULoad(IndexFileName,sizeof(rindex_t),macmRead,&count,NULL)) == FAILURE)
    {
    DBG(2,fCORE) DPrint("ALERT:    cannot load '%s' index file\n",
      IndexFileName);

    return(FAILURE);
    }

  for (index = 1;index < count;index++)
    {
    if (!strcmp(Key,Index[index].Key))
      break;
    }

  if (index == count)
    {
    DBG(2,fCORE) DPrint("ALERT:    cannot locate record '%s' in file '%s'\n",
      Key,
      FileName);

    return(FAILURE);
    } 

  tmpIndex.Key[0] = '\0';

  /* write index record */

  if ((ifp = fopen(IndexFileName,"rw+")) == NULL)
    {
    DBG(1,fCORE) DPrint("ALERT:    cannot open file '%s', errno: %d (%s)\n",
      IndexFileName,
      errno,
      strerror(errno));

    return(FAILURE);
    }

  if (fseek(ifp,index * sizeof(rindex_t),SEEK_SET) == -1)
    {
    DBG(1,fCORE) DPrint("ALERT:    cannot seek to offset %d in file '%s', errno: %d (%s)\n",
      (int)(index * sizeof(rindex_t)),
      IndexFileName,
      errno,
      strerror(errno));

    fclose(ifp);

    return(FAILURE);
    }

  if (fwrite(&tmpIndex,sizeof(rindex_t),1,ifp) < 1)
    {
    DBG(1,fCORE) DPrint("WARNING:  cannot write to file '%s', errno: %d (%s)\n",
      IndexFileName,
      errno,
      strerror(errno));

    fclose(ifp);

    return(FAILURE);
    }

  fclose(ifp);

  MFUCacheInvalidate(IndexFileName);

  return(SUCCESS);
  }  /* END MFURemoveRecord() */





int MFUGetInfo(

  char  *FileName,
  long  *ModifyTime,
  long  *FileSize,
  int   *IsExe)

  {
  struct stat sbuf;

  if (stat(FileName,&sbuf) == -1)
    {
    DBG(2,fCORE) DPrint("INFO:     cannot stat file '%s', errno: %d (%s)\n",
      FileName,
      errno,
      strerror(errno));

    return(FAILURE);
    }

  if (ModifyTime != NULL)
    *ModifyTime = (long)sbuf.st_mtime; 
 
  if (FileSize != NULL)
    *FileSize = (long)sbuf.st_size;

  if (IsExe != NULL)
    {
    if (sbuf.st_mode & S_IXUSR)
      *IsExe = TRUE;
    else
      *IsExe = FALSE;
    }

  return(SUCCESS);
  }  /* END MFUGetInfo() */





int MFULock(

  char *Directory,  /* I */
  char *LockFile)   /* I */

  {
#if !defined(__LINUX) && !defined(__CYGWIN) && !defined(__HPUX) && !defined(__NT)
  int          fd;
  struct flock lock;
  char         Name[MAX_MLINE];
#endif /* !__LINUX) && !__HPUX && !__NT */

  const char *FName = "MFULock";

  DBG(3,fALL) DPrint("%s(%s,%s)\n",
    FName,
    Directory,
    LockFile);

#if !defined(__LINUX) && !defined(__CYGWIN) && !defined(__HPUX) && !defined(__NT)

  if (!strstr(LockFile,Directory))
    {
    if (Directory[strlen(Directory) - 1] == '/')
      {
      sprintf(Name,"%s%s",
        Directory,
        LockFile);
      }
    else
      {
      sprintf(Name,"%s/%s",
        Directory,
        LockFile);
      }
    }
  else
    {
    strcpy(Name,LockFile);
    }

  if ((fd = open(Name,O_RDWR | O_CREAT,(S_IRWXU | S_IRWXG | S_IRWXO))) == -1)
    {
    perror("cannot open lockfile");

    DBG(1,fALL) DPrint("ERROR:    cannot open lockfile '%s', errno: %d (%s)\n",
      LockFile,
      errno,
      strerror(errno));

    return(FAILURE);
    }

  lock.l_type   = F_WRLCK;
  lock.l_start  = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len    = 0;
  lock.l_pid    = MOSGetPID();

  if (fcntl(fd,F_GETLK,&lock) == -1)
    {
    perror("cannot obtain lock information");

    DBG(1,fALL) DPrint("ERROR:    cannot obtain lock information on file '%s', errno: %d (%s)\n",
      LockFile,
      errno,
      strerror(errno));

    close(fd);

    return(FAILURE);
    }

  if (lock.l_type != F_UNLCK)
    {
    DBG(1,fALL) DPrint("ERROR:    cannot lock file '%s'  (lock held by PID: %d)\n",
      LockFile,
      lock.l_pid);

    close(fd);

    return(FAILURE);
    }

  lock.l_type = F_WRLCK;

  if (fcntl(fd,F_SETLK,&lock) == -1)
    {
    perror("cannot set lock");

    DBG(1,fALL) DPrint("ERROR:    unable to establish lock on file '%s', errno: %d (%s)\n",
      LockFile,
      errno,
      strerror(errno));

    close(fd);

    return(FAILURE);
    }

  DBG(3,fALL) DPrint("INFO:     server lock successful\n");

#endif /* !__LINUX && !__HP && !__NT */ 

  return(SUCCESS);
  }  /* END MFULock() */




		
int MFUCacheInvalidate(

  char *FileName) /* I */

  {
  int index;

  const char *FName = "MFUCacheInvalidate";

  DBG(7,fCORE) DPrint("%s(%s)\n",
    FName,
    FileName);

  for (index = 0;index < MAX_FILECACHE;index++)
    {
    if (MFileCache[index].FileName[0] == '\0')
      continue;

    if (strcmp(MFileCache[index].FileName,FileName))
      continue;

    /* located cache entry */

    /* reset timestamp */

    MFileCache[index].ProgTimeStamp = 0;
    MFileCache[index].FileTimeStamp = 0;

    break;
    }  /* END for (index) */

  return(SUCCESS);
  }  /* END MFUCacheInvalidate() */




int MFUGetCurrentIndex(

  char *FileName,
  int  *Index,
  int   Mode)

  {
  int index;

  for (index = 0;index < MAX_FILECACHE;index++)
    {
    if (!strcmp(FileName,MIndexState[index].FileName))
      {
      if (Mode == 1)
        MIndexState[index].Index++;

      *Index = MIndexState[index].Index;

      return(SUCCESS);
      }
    }    /* END for (index) */

  return(FAILURE);
  }  /* END MFUGetCurrentIndex() */





int MFUGetAttributes(

  char *PathName,  /* I */
  int  *Perm,      /* O */
  long *MTime,     /* O */
  long *Size,      /* O */
  int  *UID,       /* O */
  int  *IsPrivate, /* O */
  int  *IsExe)     /* O */

  {
  struct stat S;

  if (PathName == NULL)
    {
    return(FAILURE);
    }

  if (stat(PathName,&S) == -1)
    {
    return(FAILURE);
    }

  if (MTime != NULL)
    *MTime = (long)S.st_mtime;

  if (Size != NULL)
    *Size = (unsigned long)S.st_size;

  if (Perm != NULL)
    *Perm = (int)S.st_mode;

  if (UID != NULL)
    *UID = (int)S.st_uid;

  if (IsPrivate != NULL)
    {
    if (S.st_mode & (S_IRWXG|S_IRWXO))
      *IsPrivate = FALSE;
    else
      *IsPrivate = TRUE;
    }

  if (IsExe != NULL)
    {
    if (S.st_mode & S_IXUSR)
      *IsExe = TRUE;
    else
      *IsExe = FALSE;
    }

  return(SUCCESS);
  }  /* END MFUGetAttributes() */




int MFURename(

  char *SrcName,  /* I */
  char *DstName)  /* I */

  {
  if (rename(SrcName,DstName) == -1)
    {
    DBG(0,fCORE) DPrint("ERROR:    cannot rename file '%s' to '%s' errno: %d (%s)\n",
      SrcName,
      DstName,
      errno,
      strerror(errno));

    return(FAILURE);
    }

  return(SUCCESS);
  }  /* END MFURename() */
 
/* END MFile.c */

