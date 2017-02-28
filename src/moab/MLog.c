/*
Package:  moab
Module:   MLog.c
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
        
/* Contains:                                    *
 *                                              *
 * #define DBG(X,F)                             *
 * int DPrint(Message,...)                      *
 * int MLogInitialize(NewLogFile,NewMaxFileSize) *
 * int MLogOpen(Iteration)                      *
 * int MLogRoll()                               *
 * char *MLogGetTime()                          *
 * void MLogLevelAdjust(signo)                  *
 *                                              */


#include "moab.h"
#include "msched-proto.h"  


#define MAX_LOGNAME  256
#define MAX_MDEBUG    10

#ifndef SUCCESS
#define SUCCESS              1
#endif

#ifndef FAILURE
#define FAILURE              0
#endif

#ifndef DBG

typedef struct {
  FILE *logfp;
  int   Threshold;
  int   FacilityList;
  } mlog_t;

#define DBG(X,F) if ((mlog.Threshold >= X) && ((mlog.FacilityList) & F))

#define MDEBUG(X) if (mlog.Threshold >= X)

#endif /* DBG */

mlog_t mlog = { 0,0,0xffffffff };

char  LogFile[MAX_LOGNAME + 1];
int   MaxLogFileSize = -1;

char *MLogGetTime(void);
void  MLogLevelAdjust(int);




int MLogInitialize(

  char *NewLogFile,     /* I */
  int   NewMaxFileSize, /* I */
  int   Iteration)      /* I */

  {
  static int SigSet = 0;

  if (NewMaxFileSize != -1)
    MaxLogFileSize = NewMaxFileSize;

  if (NewLogFile == NULL)
    {
    LogFile[0] = '\0';

    MLogOpen(Iteration);
    }
  else if ((strcmp(LogFile,"N/A") != 0) &&
      (strcmp(NewLogFile,"N/A") != 0) &&
      (strcmp(LogFile,NewLogFile) != 0))
    {
    strncpy(LogFile,NewLogFile,MAX_LOGNAME);
    LogFile[MAX_LOGNAME] = '\0';

    MLogOpen(Iteration);
    }

  if (SigSet == 0)
    {
#   if defined(__AIX41) || defined(__AIX42) || defined(__AIX43) || defined(__IRIX) || defined(__LINUX) || defined(__CYGWIN) || defined(__HPUX) || defined(__SOLARIS)
      signal(SIGUSR1,(void(*)(int))MLogLevelAdjust);
      signal(SIGUSR2,(void(*)(int))MLogLevelAdjust);
#   else 
#     ifndef __NT
        signal(SIGUSR1,MLogLevelAdjust);
        signal(SIGUSR2,MLogLevelAdjust);
#     endif /* __NT */
#   endif   /* __AIX41... */

    SigSet = 1;
    }

  return(SUCCESS);
  }  /* END MLogInitialize() */ 




int MLogOpen(

  int Iteration)  /* I */

  {
  const char *FName = "MLogOpen";

  DBG(4,fCORE) DPrint("%s()\n",
    FName);

  if ((mlog.logfp != NULL) && 
      (mlog.logfp != stderr))
    {
    fclose(mlog.logfp);

    mlog.logfp = NULL;
    }

  /* if null logfile, send logs to stderr */

  if ((LogFile == NULL) || (LogFile[0] == '\0'))
    {
    mlog.logfp = stderr;

    return(SUCCESS);
    }
  else if ((getenv(MSCHED_ENVLOGSTDERRVAR) != NULL) && (Iteration == 0))
    {
    fclose(stderr);
    }

  umask(0027);

  if (umask(0027) != 0027)
    {
    DBG(2,fCORE) DPrint("ERROR:    cannot set umask on logfile '%s', errno: %d (%s)\n",
      LogFile,
      errno,
      strerror(errno));
    }

  if ((mlog.logfp = fopen(LogFile,"a+")) == NULL)
    {
    DBG(1,fALL) fprintf(stderr,"WARNING:  cannot open log file '%s', errno: %d (%s)\n",
      LogFile,
      errno,
      strerror(errno));

    /* dump logs to stderr */

    mlog.logfp = stderr;

    return(FAILURE);
    }

  /* use smallest of line and 4K buffering */

  if (setvbuf(mlog.logfp,NULL,_IOLBF,4097) != 0)
    {
    fprintf(stderr,"WARING:  cannot setup line mode buffering on logfile\n");
    }
   
  return(SUCCESS);
  }  /* END MLogOpen() */




int MLogShutdown()

  {
  if (mlog.logfp != NULL)
    {
    fclose(mlog.logfp);
 
    mlog.logfp = NULL;
    }

  return(SUCCESS);
  }  /* END MLogShutdown() */



#ifndef __MTEST

int DPrint(

  char *Format,   /* I */
  ...)

  {
  va_list Args;

  if (mlog.logfp != NULL)
    {
    if (mlog.logfp != stderr)
      {
      fprintf(mlog.logfp,"%s",
        MLogGetTime());
      }

    va_start(Args,Format);

    vfprintf(mlog.logfp,Format,
      Args);

    va_end(Args);
    }  /* END if (mlog.logfp != NULL) */

  return(SUCCESS);
  }  /* END DPrint() */

#endif /* __MTEST */




int MLogRoll(

  char *Suffix,
  int   Flag,
  int   Iteration,
  int   Depth)

  {
  struct stat buf;
  int         dindex;

  char OldFile[MAX_LOGNAME];
  char NewFile[MAX_LOGNAME];

  const char *FName = "MLogRoll";

  DBG(5,fCORE) DPrint("%s(%s,%d,%d)\n",
    FName,
    (Suffix != NULL) ? Suffix : "NULL",
    Flag,
    Depth);

  if (LogFile[0] == '\0')
    {
    /* writing log data to stderr */

    return(SUCCESS);
    }

  if (Flag == 0)
    {
    if (MaxLogFileSize == -1)
      {
      /* no file size limit specified */

      return(SUCCESS);
      }

    if (stat(LogFile,&buf) == -1)
      {
      DBG(0,fCORE) DPrint("WARNING:  cannot get stats on file '%s', errno: %d (%s)\n",
        LogFile,
        errno,
        strerror(errno));

      /* re-open log file if necessary */

      if (errno == ENOENT)
        {
        MDEBUG(1)
          {
          fprintf(stderr,"WARNING:  logfile '%s' was removed  (re-opening file)\n",
            LogFile);
          }

        MLogOpen(Iteration);
        }

      return(SUCCESS);
      }
    }    /* END if (Flag == 0) */

  if (((LogFile[0] != '\0') || (mlog.logfp != stderr)) &&
      ((buf.st_size > MaxLogFileSize) || (Flag == 1)))
    {
    if ((Suffix != NULL) && (Suffix[0] == '\0'))
      { 
      sprintf(NewFile,"%s%s",
        LogFile,
        Suffix);

      DBG(2,fCORE) DPrint("INFO:     rolling logfile '%s' to '%s'\n",
        LogFile,
        NewFile);

      if (rename(LogFile,NewFile) == -1)
        {
        DBG(0,fCORE) DPrint("ERROR:    cannot rename logfile '%s' to '%s' errno: %d (%s)\n",
          LogFile,
          NewFile,
          errno,
          strerror(errno));
        }
      }
    else
      {
      for (dindex = Depth;dindex > 0;dindex--)
        {
        sprintf(NewFile,"%s.%d",
          LogFile,
          dindex);
 
        if (dindex == 1)
          strcpy(OldFile,LogFile);
        else
          sprintf(OldFile,"%s.%d",
            LogFile,
            dindex - 1);
 
        DBG(2,fCORE) DPrint("INFO:     rolling logfile '%s' to '%s'\n",
          OldFile,
          NewFile);
 
        if (rename(OldFile,NewFile) == -1)
          {
          DBG(0,fCORE) DPrint("ERROR:    cannot rename logfile '%s' to '%s' errno: %d (%s)\n",
            OldFile,
            NewFile,
            errno,
            strerror(errno));
          }
        }    /* END for (dindex) */      
      }

    MLogOpen(Iteration);
    }
  else
    {
    DBG(3,fCORE) DPrint("INFO:     not rolling logs (%lu < %d)\n",
      buf.st_size,
      MaxLogFileSize);
    }

  return(SUCCESS);
  }  /* END MLogRoll() */




char *MLogGetTime()

  {
  time_t        epoch_time = 0;
  struct tm    *present_time;
  static char   line[MAX_MLINE];
  static time_t now = 0;

  MUGetTime((mulong *)&epoch_time,mtmNONE,NULL);

  if (epoch_time != now)
    {
    now = epoch_time;

    if ((present_time = localtime(&epoch_time)) != NULL)
      {
      sprintf(line,"%2.2d/%2.2d %2.2d:%2.2d:%2.2d ",
        present_time->tm_mon + 1,
        present_time->tm_mday,
        present_time->tm_hour,
        present_time->tm_min,
        present_time->tm_sec);
      }
    else
      {
      sprintf(line,"%2.2d/%2.2d %2.2d:%2.2d:%2.2d ", 
        0,0,0,0,0);
      }
    }    /* END if (epoch_time != now) */

  return(line);
  }  /* END MLogGetTime() */





#ifndef __NT

void MLogLevelAdjust(

  int signo)

  {
  const char *FName = "MLogLevelAdjust";

  DBG(1,fCONFIG) DPrint("%s(%d)\n",
    FName,
    signo);

  if (signo == SIGUSR1)
    {
    if (mlog.Threshold < MAX_MDEBUG)
      {
      mlog.Threshold++;

      DBG(0,fCONFIG) DPrint("INFO:     received signal %d.  increasing LOGLEVEL to %d\n",
        signo,
        mlog.Threshold);
      }
    else
      {
      DBG(0,fCONFIG) DPrint("INFO:     received signal %d.  LOGLEVEL is already at max value %d\n",
        signo,
        mlog.Threshold);
      }

#if defined(__AIX41) || defined(__AIX42) || defined(__AIX43) || defined(__IRIX) || defined(__LINUX) || defined(__CYGWIN) || defined(__HPUX) || defined(__SOLARIS)

    signal(SIGUSR1,(void(*)(int))MLogLevelAdjust);

#else

    signal(SIGUSR1,MLogLevelAdjust);

#endif /* ... */
    }
  else if (signo == SIGUSR2)
    {
    if (mlog.Threshold > 0)
      {
      mlog.Threshold--;

      DBG(0,fCONFIG) DPrint("INFO:     received signal %d.  decreasing LOGLEVEL to %d\n",
        signo,
        mlog.Threshold);
      }
    else
      {
      DBG(0,fCONFIG) DPrint("INFO:     received signal %d.  LOGLEVEL is already at min value %d\n",
        signo,
        mlog.Threshold);
      }

#if defined(__AIX41) || defined(__AIX42) || defined(__AIX43) || defined(__IRIX) || defined(__LINUX) || defined(__CYGWIN) || defined(__HPUX) || defined(__SOLARIS) || defined(__OSF)

    signal(SIGUSR2,(void(*)(int))MLogLevelAdjust);

#else

    signal(SIGUSR2,MLogLevelAdjust);

#endif
    }

  fflush(mlog.logfp);

  return;
  }  /* END MLogLevelAdjust() */

#endif /* __NT */

/* END MLog.c */

