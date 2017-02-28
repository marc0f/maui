/*
Package:  moab
Module:   MTime.c
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
        
/* Contains:                                 *
 *   int MUStringToE(TimeLine,EpochTime)     *
 *                                           */


#include "moab.h"
#include "msched-proto.h"  

extern mlog_t mlog;

char *MUStrTok(char *Line,char *Delims,char **Ptr);





int MUStringToE(

  char   *TimeLine,
  long   *EpochTime)

  {
  char       Second[MAX_MNAME];
  char       Minute[MAX_MNAME];
  char       Hour[MAX_MNAME];
  char       Day[MAX_MNAME];
  char       Month[MAX_MNAME];
  char       Year[MAX_MNAME];
  char       TZ[MAX_MNAME];

  char       StringTime[MAX_MNAME];
  char       StringDate[MAX_MNAME];
  char       Line[MAX_MLINE];

  char      *ptr;
  char      *tail;

  struct tm  Time;
  struct tm *DefaultTime;

  time_t     ETime;             /* calculated epoch time */
  time_t     Now;

  int        YearVal;

  char      *TokPtr;

  const char *FName = "MUStringToE";

  DBG(2,fCONFIG) DPrint("%s(%s,EpochTime)\n",
    FName,
    TimeLine);

  time(&Now);

  /* check 'NOW' keyword */

  if (!strcmp(TimeLine,"NOW"))
    {
    *EpochTime = (long)Now;
   
    return(SUCCESS);
    }

  /* check 'OFF' keyword */

  if (!strcmp(TimeLine,"OFF"))
    {
    *EpochTime = MAX_MTIME;
  
    return(SUCCESS);
    }

  if ((ptr = strchr(TimeLine,'+')) != NULL)
    {
    /* using relative time */

    /* Format [ +d<DAYS> ][ +h<HOURS> ][ +m<MINUTES> ][ +s<SECONDS> ] */

    ETime = Now + MUTimeFromString(ptr + 1);
    }
  else
    { 
    /* using absolute time */

    /* Format:  HH[:MM[:SS]][_MM[/DD[/YY]]] */

    setlocale(LC_TIME,"en_US.iso88591");

    DefaultTime = localtime(&Now);

    /* copy default values into time structure */

    strcpy(Second,"00");
    strcpy(Minute,"00");
    strftime(Hour  ,MAX_MNAME,"%H",DefaultTime);

    strftime(Day   ,MAX_MNAME,"%d",DefaultTime);
    strftime(Month ,MAX_MNAME,"%m",DefaultTime);
  
    strftime(Year  ,MAX_MNAME,"%Y",DefaultTime);
  
    strftime(TZ    ,MAX_MNAME,"%Z",DefaultTime);

    if ((tail = strchr(TimeLine,'_')) != NULL)
      {
      /* time and date specified */

      strncpy(StringTime,TimeLine,(tail - TimeLine));
      StringTime[(tail - TimeLine)] = '\0';

      strcpy(StringDate,(tail + 1));

      DBG(7,fCONFIG) DPrint("INFO:     time: '%s'  date: '%s'\n",
        StringTime,
        StringDate);

      /* parse date */

      if ((ptr = MUStrTok(StringDate,"/",&TokPtr)) != NULL)
        {
        strcpy(Month,ptr);

        if ((ptr = MUStrTok(NULL,"/",&TokPtr)) != NULL)
          {
          strcpy(Day,ptr);

          if ((ptr = MUStrTok(NULL,"/",&TokPtr)) != NULL)
            {
            YearVal = atoi(ptr);

            if (YearVal < 97)
              {
              sprintf(Year,"%d",
                YearVal + 2000);
              }
            else if (YearVal < 1900) 
              {
              sprintf(Year,"%d",
                YearVal + 1900);
              }
            else 
              {
              sprintf(Year,"%d",
                YearVal);
              }
            }
          }
        }
      }
    else
      {
      strcpy(StringTime,TimeLine);
      }

    /* parse time */

    if ((ptr = MUStrTok(StringTime,":_",&TokPtr)) != NULL)
      {
      strcpy(Hour,ptr);

      if ((ptr = MUStrTok(NULL,":_",&TokPtr)) != NULL)
        {
        strcpy(Minute,ptr);

        if ((ptr = MUStrTok(NULL,":_",&TokPtr)) != NULL)
          strcpy(Second,ptr);
        }
      }

    /* create time string */

    sprintf(Line,"%s:%s:%s %s/%s/%s %s",
      Hour,
      Minute,
      Second,
      Month,
      Day,
      Year,
      TZ);

    /* perform bounds checking */

    if ((atoi(Second) > 59) || 
        (atoi(Minute) > 59) || 
        (atoi(Hour)   > 23) || 
        (atoi(Month)  > 12) || 
        (atoi(Day)    > 31) || 
        (atoi(Year)   > 2097))
      {
      DBG(1,fCONFIG) DPrint("ERROR:    invalid time specified '%s' (bounds exceeded)\n",
        Line);

      return(FAILURE);
      }

    memset(&Time,0,sizeof(Time));

    Time.tm_hour = atoi(Hour);
    Time.tm_min  = atoi(Minute);
    Time.tm_sec  = atoi(Second);
    Time.tm_mon  = atoi(Month) - 1;
    Time.tm_mday = atoi(Day);
    Time.tm_year = atoi(Year) - 1900;

    /* adjust for TZ */

    Time.tm_isdst = -1;

    /* place current time into tm structure */

    DBG(5,fCONFIG) DPrint("INFO:     generated time line: '%s'\n",
      Line);

    /* strptime(Line,"%T %m/%d/%Y %Z",&Time); */

    if ((ETime = mktime(&Time)) == -1)
      {
      DBG(5,fCONFIG) DPrint("ERROR:    cannot determine epoch time for '%s', errno: %d (%s)\n",
        Line,
        errno,
        strerror(errno));

      return(FAILURE);
      }
    }  /* END else (strchr(TimeLine,'+')) */

  DBG(3,fCONFIG) DPrint("INFO:     current   epoch:  %lu  time:  %s\n",
    (unsigned long)Now,
    ctime(&Now));

  DBG(3,fCONFIG) DPrint("INFO:     calculated epoch: %lu  time:  %s\n",
    (unsigned long)ETime,
    ctime(&ETime));

  *EpochTime = (long)ETime;

  return(SUCCESS);
  }  /* END MUStringToE() */




long MUTimeFromString(

  char *TString)

  {
  long  val;

  char *ptr1;
  char *ptr2;
  char *ptr3;
  char *ptr4;

  char *TokPtr;

  char  Line[MAX_MLINE];

  const char *FName = "MUTimeFromString";

  DBG(2,fCONFIG) DPrint("%s(%s)\n",
    FName,
    (TString != NULL) ? TString : "NULL");

  if (TString == NULL)
    return(0);

  if (!strcmp(TString,"INFINITY"))
    return(MAX_MTIME);

  if (strchr(TString,':') == NULL)
    {
    /* line specified as 'raw' seconds */

    val = strtol(TString,NULL,0);

    DBG(4,fCONFIG) DPrint("INFO:     string '%s' specified as seconds\n",
      TString);

    return(val);
    }
  else if (strchr(TString,'_') != NULL)
    {
    /* line specified as 'absolute' time */

    MUStringToE(TString,&val);

    DBG(4,fCONFIG) DPrint("INFO:     string '%s' specified as absolute time\n",
      TString);
 
    return(val);
    }

  /* line specified in 'military' time */

  MUStrCpy(Line,TString,sizeof(Line));

  ptr1 = NULL;
  ptr2 = NULL;
  ptr3 = NULL;
  ptr4 = NULL;

  if ((ptr1 = MUStrTok(Line,":",&TokPtr)) != NULL)
    {
    if ((ptr2 = MUStrTok(NULL,":",&TokPtr)) != NULL)
      {
      if ((ptr3 = MUStrTok(NULL,":",&TokPtr)) != NULL)
        {
        ptr4 = MUStrTok(NULL,":",&TokPtr);
        }
      }
    }

  if (ptr1 == NULL)
    {
    DBG(4,fCONFIG) DPrint("INFO:     cannot read string '%s'\n",
      TString);

    return(0);
    }

  if (ptr4 == NULL)
    {
    /* adjust from HH:MM:SS to DD:HH:MM:SS notation */

    ptr4 = ptr3;
    ptr3 = ptr2;
    ptr2 = ptr1;
    ptr1 = NULL;
    }

  val = (((ptr1 != NULL) ? atoi(ptr1) : 0) * 86400) +
        (((ptr2 != NULL) ? atoi(ptr2) : 0) *  3600) +
        (((ptr3 != NULL) ? atoi(ptr3) : 0) *    60) +
        (((ptr4 != NULL) ? atoi(ptr4) : 0) *     1);

  DBG(4,fCONFIG) DPrint("INFO:     string '%s' -> %ld\n",
    TString,
    val);

  return(val);
  }  /* END MUTimeFromString() */


/* END MTime.c */
