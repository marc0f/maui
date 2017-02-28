/*
Package:  moab
Module:   MGUtil.c
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

/* contains:                            *
 *  int MOSSyslog(int,char *,...)       *
 *  int MUStrTok(char *,char *,char **) *
 *                                      */



int *MGUSyslogActive = NULL;  /* boolean */




int MUSNInit(
                                                                                
  char  **BPtr,       /* O (modified) */
  int    *BSpace,     /* O */
  char   *SrcBuf,     /* I */
  int     SrcBufSize) /* I */
                                                                                
  {
  if ((BPtr == NULL) ||
      (BSpace == NULL) ||
      (SrcBuf == NULL))
    {
    return(FAILURE);
    }
                                                                                
  *BPtr = SrcBuf;
                                                                                
  *BSpace = SrcBufSize;
                                                                                
  (*BPtr)[0] = '\0';
                                                                                
  return(SUCCESS);
  }  /* END MUSNInit() */





int MOSGetEUID()

  {
#if defined(__AIX43) || defined(__AIX51)
  return(getuid());
#endif /* __AIX43 */

  return(geteuid());
  }  /* END MOSGetEUID() */



int MOSGetUID()

  {
  return(getuid());
  }  /* END MOSGetUID() */




int MOSSyslogInit(
 
  msched_t *S)  /* I */

  {
  if (S == NULL)
    {
    return(FAILURE);
    }

  MGUSyslogActive = &S->SyslogActive;

  if ((S->SyslogActive == FALSE) && (S->UseSyslog == TRUE))
    {
    openlog(
      MSCHED_SNAME,
      LOG_PID|LOG_NDELAY,
      (S->SyslogFacility > 0) ? 
        S->SyslogFacility : 
        LOG_DAEMON);

    S->SyslogActive = TRUE;

    if (S->Mode == msmNormal)
      {
      MOSSyslog(LOG_INFO,"%s initialized",
        MSCHED_SNAME);
      }
    }

  return(SUCCESS);
  }  /* END MSyslogInitialize() */




int MOSSyslog(

  int   Facility,  /* I */
  char *Format,    /* I */
  ...)             /* I */

  {
  va_list Args;

  if ((MGUSyslogActive == NULL) ||
      (*MGUSyslogActive == FALSE))
    {
    return(SUCCESS);
    }

  va_start(Args,Format);

  syslog(Facility,Format,Args);

  va_end(Args);

  return(SUCCESS);
  }  /* END MOSSyslog() */




int MUCheckAuthFile(

  msched_t *S,          /* I (optional) */
  char     *KeyBuf,     /* O (optional) */
  int      *UseKeyFile, /* O (optional) */
  int       IsServer)   /* I (boolean) */

  {
  char tmpFileName[MAX_MLINE];

  int AuthFileOK = FALSE;
  int IsPrivate;

  int UID;

  char *ptr;

  /* determine file name */

  if ((ptr = getenv("MAUTH_FILE")) != NULL)
    {
    MUStrCpy(tmpFileName,ptr,sizeof(tmpFileName));
    }
  else if (S != NULL)
    {
    if (S->KeyFile[0] != '\0')
      {
      MUStrCpy(tmpFileName,S->KeyFile,sizeof(tmpFileName));
      }
    else
      {
      sprintf(tmpFileName,"%s/%s",
        S->HomeDir,
        MSCHED_KEYFILE);
      }
    }
  else
    { 
    strcpy(tmpFileName,MSCHED_KEYFILE);
    }

  /* check existence */

  if (MFUGetAttributes(tmpFileName,NULL,NULL,NULL,&UID,&IsPrivate,NULL) == SUCCESS)
    {
    if (IsPrivate == TRUE)
      {
      if (IsServer == TRUE)
        {
        if ((S != NULL) && (UID == S->UID))
          {
          AuthFileOK = TRUE;
          }
        }
      else
        {
        AuthFileOK = TRUE;
        }
      }
    }
  
  if (AuthFileOK == FALSE)
    {
    if (UseKeyFile != NULL)
      *UseKeyFile = FALSE;

    if (KeyBuf != NULL)
      strncpy(KeyBuf,MBUILD_SKEY,MAX_MNAME);

    return(SUCCESS);
    }

  if (UseKeyFile != NULL)
    *UseKeyFile = TRUE;

  if ((IsServer == TRUE) && (KeyBuf != NULL))
    {
    char *ptr;

    int   i;

    /* load key file */

    if ((ptr = MFULoad(tmpFileName,1,macmRead,NULL,NULL)) == NULL)
      {
      /* cannot load data */

      return(FAILURE);
      }

    MUStrCpy(KeyBuf,ptr,MAX_MNAME);

    for (i = strlen(KeyBuf) - 1;i > 0;i--)
      {
      if (!isspace(KeyBuf[i]))
        break;

      KeyBuf[i] = '\0';
      }  /* END for (i) */
    }    /* END if ((IsServer == TRUE) && (KeyBuf != NULL)) */
 
  return(SUCCESS);
  }  /* END MUCheckAuthFile() */





int MUGetTime(

  mulong   *Time,                  /* I/O */
  enum MTimeModeEnum RefreshMode,  /* I */
  msched_t *S)                     /* I (optional) */

  {
  mulong tmpTime;

  time_t tmpT;

  if (Time == NULL)
    {
    return(FAILURE);
    }

  if (((S != NULL) && (S->TimePolicy != mtpNONE)) ||
       (RefreshMode == mtmRefresh))
    {
    tmpTime = MIN(*Time,MAX_MTIME);
    }

  if (S != NULL)
    {
    if (S->X != NULL)
      {
      mx_t *X;

      X = (mx_t *)S->X;

      if ((X->XGetTime != (int (*)())0) &&
         ((S->Mode != msmSim) ||
         ((S->TimePolicy != mtpReal) &&
          (S->TimePolicy != mtpNONE))))
        {
        (*X->XGetTime)(X->xd,(long *)&tmpTime,RefreshMode);

        *Time = tmpTime;

        return(SUCCESS);
        }
      }    /* END if (S->X != NULL) */

    if ((S->Mode == msmSim) && (S->TimePolicy != mtpReal))
      {
      switch (RefreshMode)
        {
        case mtmNONE:
        default:

          /* no action necessary */

          break;

        case mtmRefresh:

          /* refresh */

          tmpTime += S->RMPollInterval;

          break;

        case mtmInit:

          /* initialize (load real time) */

          time(&tmpT);

          tmpTime = (mulong)tmpT;

          break;
        }  /* END switch(RefreshMode) */

      if (Time != NULL)
        *Time = tmpTime;

      return(SUCCESS);
      }  /* END if ((S->Mode == msmSim) && ...) */
    }    /* END if (S != NULL) */

  /* load real time */

  time(&tmpT);

  tmpTime = (mulong)tmpT;

  if (Time != NULL)
    *Time = tmpTime;

  return(SUCCESS);
  }  /* END MUGetTime() */




#ifndef __SANSI__

int MUReadPipe(

  char *Command,  /* I */
  char *Buffer,   /* O */
  int   BufSize)  /* I */

  {
  FILE *fp;
  int   rc;

  const char *FName = "MUReadPipe";

  DBG(5,fSOCK) DPrint("%s(%s,Buffer,%d)\n",
    FName,
    (Command != NULL) ? Command : "NULL",
    BufSize);

  if ((Command == NULL) || (Buffer == NULL))
    {
    return(FAILURE);
    }

  if ((fp = popen(Command,"r")) == NULL)
    {
    DBG(0,fSOCK) DPrint("ERROR:    cannot open pipe on command '%s', errno: %d (%s)\n",
      Command,
      errno,
      strerror(errno));

    return(FAILURE);
    }

  if ((rc = fread(Buffer,1,BufSize,fp)) == -1)
    {
    DBG(0,fSOCK) DPrint("ERROR:    cannot read pipe on command '%s', errno: %d (%s)\n",
      Command,
      errno,
      strerror(errno));

    pclose(fp);

    return(FAILURE);
    }

  /* terminate buffer */

  Buffer[rc] = '\0';

  DBG(5,fSOCK) DPrint("INFO:     pipe(%s) -> '%s'\n",
    Command,
    Buffer);

  pclose(fp);

  return(SUCCESS);
  }  /* END MUReadPipe() */

#endif /* __SANSI__ */




int MUStrCpy(

  char *Dst,     /* I */
  char *Src,     /* O */
  int   Length)  /* I */

  {
  int index;

  if ((Dst == NULL) || (Src == NULL) || (Length == 0))
    {
    return(FAILURE);
    }

  if (Length == -1)
    Length = MAX_MNAME;

  for (index = 0;index < Length;index++)
    {
    if (Src[index] == '\0')
      break;

    Dst[index] = Src[index];
    }  /* END for (index) */

  if (index >= Length)
    Dst[Length - 1] = '\0';
  else
    Dst[index] = '\0';

  return(SUCCESS);
  }  /* END MUStrCpy() */




char *MUStrTok(

  char  *Line,  /* I (optional) */
  char  *DList, /* I */
  char **Ptr)   /* O */

  {
  char *Head = NULL;

  int dindex;

  mbool_t ignchar;

  if (Line != NULL)
    {
    *Ptr = Line;
    }
  else if ((Ptr != NULL) && (*Ptr == NULL))
    {
    return(FAILURE);
    }

  ignchar = FALSE;

  while (**Ptr != '\0')
    {
    for (dindex = 0;DList[dindex] != '\0';dindex++)
      {
      if (**Ptr == DList[dindex])
        {
        **Ptr = '\0';

        (*Ptr)++;

        if (Head != NULL)
          {
          return(Head);
          }
        else
          {
          ignchar = TRUE;

          break;
          }
        }
      }    /* END for (dindex) */

    if ((ignchar != TRUE) && (**Ptr != '\0'))
      {
      if (Head == NULL)
        Head = *Ptr;

      (*Ptr)++;
      }

    ignchar = FALSE;
    }  /* END while (**Ptr != '\0') */

  return(Head);
  }  /* END MUStrTok() */





int MUGetIndex(

  char        *Value,           /* I */
  const char **ValList,         /* I */
  int          AllowSubstring,  /* I (boolean) */
  int          DefaultValue)    /* I */

  {
  const char *FName = "MUGetIndex";

  int index;

  DBG(3,fSTRUCT) DPrint("%s(%s,%s,%d)\n",
    FName,
    (Value != NULL) ? Value : "NULL",
    (ValList != NULL) ? "ValList" : "NULL",
    DefaultValue);

  if (ValList == NULL)
    {
    return(DefaultValue);
    }

  if (Value == NULL)
    {
    return(DefaultValue);
    }

  for (index = 0;ValList[index] != NULL;index++)
    {
    if ((AllowSubstring == FALSE) &&
        (!strcmp(Value,ValList[index])))
      {
      return(index);
      }
    else if ((AllowSubstring == TRUE) &&
             (!strncmp(Value,ValList[index],strlen(ValList[index]))))
      {
      return(index);
      }
    else if (AllowSubstring == MBNOTSET)
      {
      int len = strlen(ValList[index]);

      if (!strncmp(Value,ValList[index],len) &&
         (strchr(" \t\n=<>,:;|",Value[len]) || (Value[len] == '\0')))
        {
        return(index);
        }
      }
    }    /* END for (index) */

  return(DefaultValue);
  }  /* END MUGetIndex() */



int MUStrToLower(

  char *String) /* I (modified) */

  {
  int sindex;

  if (String == NULL)
    {
    return(SUCCESS);
    }

  for (sindex = 0;String[sindex] != '\0';sindex++)
    {
    String[sindex] = tolower(String[sindex]);
    }  /* END for (sindex) */

  return(SUCCESS);
  }  /* END MUStrToLower() */




int MUStrToUpper(

  char *String,  /* I (modified) */
  char *OBuf,    /* O (optional) */
  int   BufSize) /* I */

  {
  int sindex;
  char *ptr;

  if (String == NULL)
    {
    return(SUCCESS);
    }

  ptr = (OBuf != NULL) ? OBuf : String;

  for (sindex = 0;String[sindex] != '\0';sindex++)
    {
    if ((BufSize > 0) && (sindex >= (BufSize - 1)))
      break;

    ptr[sindex] = toupper(String[sindex]);
    }  /* END for (sindex) */

  ptr[sindex] = '\0';

  return(SUCCESS);
  }  /* END MUStrToUpper() */




int MMovePtr(
                                                                                
  char **SrcP,  /* I (modified) */
  char **DstP)  /* O (modified) */
                                                                                
  {
  *DstP = *SrcP;
                                                                                
  *SrcP = NULL;
                                                                                
  return(SUCCESS);
  }  /* END MMovePtr() */



/* END MGUtil.c */

