/*
Package:  moab
Module:   MTComm.c
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

#ifdef __MGRIDENABLE

#ifndef __MTHREADS
#define __MTHREADS
#endif /* __MTHREADS */

#ifdef __MTHREADS

#include "msu.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include "communication.h"

pthread_mutex_t comm_mutex = PTHREAD_MUTEX_INITIALIZER;

/* prototypes */

void  communication_free(communication_t *);
void *comm_client_loop(comm_thread_t *);
void *comm_server_loop(comm_thread_t *);

/* END prototypes */



comm_thread_t* comm_thread_new(

  int max_connections) /* I */

  {
  comm_thread_t* thread;
  if ((thread = (comm_thread_t *)calloc(1,sizeof(comm_thread_t))) == NULL)
    {
    return(NULL);
    }

  thread->open_handler = NULL;
  thread->read_handler = NULL;
  thread->close_handler = NULL;
  thread->error_handler = NULL;
  thread->connection = (communication_t **)calloc(max_connections, sizeof(void*));

  thread->connections = 0;
  thread->max_connections = max_connections;
  thread->port = -1;
  thread->wakeup_fdr = -1;
  thread->wakeup_fdw = -1;

  return(thread);
  }  /* END comm_thread_new() */




void comm_thread_free(

  comm_thread_t *thread) 

  {
  int i;

  if (thread == NULL)
    {
    return;
    }

  for (i = 0;i < thread->connections;i++)
    {
    communication_free(thread->connection[i]);
    }

  if (thread->connection)
    free(thread->connection);

  if (thread->wakeup_fdr > 0)
    close(thread->wakeup_fdr);

  if (thread->wakeup_fdw > 0)
    close(thread->wakeup_fdw);

  free(thread);

  return;
  }  /* END comm_thread_free() */




int comm_thread_add_connection(comm_thread_t* thread, communication_t* comm) {
	if (thread->connections >= thread->max_connections) {
		thread->error_handler("too many open connections");
		return -1;
	}

	thread->connection[thread->connections++] = comm;

	return 0;
}




void* comm_thread_start(

  int                  port,
  int                  max_connections,
  comm_open_handler_f  open_handler,
  comm_close_handler_f close_handler,
  comm_read_handler_f  read_handler,
  comm_error_handler_f error_handler,
  void                *FPtr)

  {
  pthread_t t;

  int fd[2];

  comm_thread_t* thread = comm_thread_new(max_connections);

  if (pipe(fd) < 0) 
    {
    error_handler("unable to create a pipe (%s)", strerror(errno));

    comm_thread_free(thread);

    return(NULL);
    }

  thread->wakeup_fdr = fd[0];
  thread->wakeup_fdw = fd[1];
  thread->port = port;
  thread->open_handler = open_handler;
  thread->read_handler = read_handler;
  thread->close_handler = close_handler;
  thread->error_handler = error_handler;

  if (pthread_create(&t,NULL,(void* (*)(void*))FPtr,thread)) 
    {
    error_handler("unable to create a new thread");

    comm_thread_free(thread);

    return(NULL);
    }

  return(thread);
  }  /* END comm_thread_start() */





comm_thread_t* comm_client_thread_start(

  int                  max_connections,
  comm_open_handler_f  open_handler,
  comm_close_handler_f close_handler,
  comm_read_handler_f  read_handler,
  comm_error_handler_f error_handler) 

  {
  /* void *comm_client_loop(comm_thread_t *); */

  /* ARG   void *(*loop)(comm_thread_t*)) */

  /* DOUG  (void *(*)(comm_thread_t *))comm_client_loop)); */

  return((comm_thread_t *)comm_thread_start(
    -1,
    max_connections,
    open_handler,
    close_handler,
    read_handler,
    error_handler,
    (void *)comm_client_loop));
  }





comm_thread_t *comm_server_thread_start(

  int                  port,
  int                  max_connections,
  comm_open_handler_f  open_handler,
  comm_close_handler_f close_handler,
  comm_read_handler_f  read_handler,
  comm_error_handler_f error_handler) 
 
  {
  return((comm_thread_t *)comm_thread_start(
    port,
    max_connections,
    open_handler,
    close_handler,
    read_handler,
    error_handler,
    (void *)comm_server_loop));
  }





void comm_thread_stop(

  comm_thread_t *thread) 

  {
  int action = 0;

  if (write(thread->wakeup_fdw,&action,sizeof(int)) != sizeof(int))
    thread->error_handler(strerror(errno));

  comm_thread_free(thread);

  return;
  }  /* END comm_thread_stop() */





communication_t *communication_new(

  comm_thread_t *thread) 
  
  {
  communication_t *comm;
  if ((comm = (communication_t *)calloc(1, sizeof(communication_t))) == NULL)
    {
    return(NULL);
    }

  comm->thread = thread;
  comm->host   = NULL;
  comm->port   = -1;
  comm->user   = NULL;
  comm->fd     = -1;
  comm->speed  = 0;
  comm->uptime = time(NULL);
  comm->bytes  = 0;

  return(comm);
  }  /* END communication_new() */





void communication_free(

  communication_t *comm)  /* I (freed) */

  {
  if (comm == NULL)
    {
    return;
    }

  if (comm->host != NULL)
    free(comm->host);

  if (comm->fd > 0)
    {
    close(comm->fd);

    comm->fd = -1;
    }

  free(comm);

  return;
  }  /* END communication_free() */






communication_t *comm_client_new(

  comm_thread_t *thread,
  char          *host,
  int            port,
  void          *user) 
  
  {
  communication_t *comm = communication_new(thread);  /* alloc */

  comm->host = host ? strdup(host) : NULL;
  comm->port = port;
  comm->user = user;

  if (comm_thread_add_connection(thread,comm) < 0) 
    {
    communication_free(comm);

    return(NULL);
    }

  return(comm);
  }





void comm_client_free(

  communication_t *comm) /* I (free) */

  {
  communication_free(comm);

  return;
  }  /* END comm_client_free() */





int comm_client_open(

  communication_t *comm)  /* I (modified) */

  {
  struct sockaddr_in  addr;
  struct hostent     *hent;

  int fd;
  int flags;
  int result;

  if (comm->fd > 0)
    {
    /* SUCCESS - socket already open */

    return(SUCCESS);  
    }

  if (comm->host == NULL)
    {
    /* FAILURE */

    return(FAILURE);
    }

  hent = gethostbyname(comm->host);

  if (hent == NULL) 
    {
    comm->thread->error_handler(
      "unknown host name %s (%s)",
      comm->host,
      hstrerror(h_errno));

    return(FAILURE);
    }

  memset(&addr,0,sizeof(addr));

  addr.sin_family = AF_INET;

  memcpy(&addr.sin_addr.s_addr,hent->h_addr_list[0],sizeof(addr.sin_addr.s_addr));

  addr.sin_port = htons(comm->port);

  fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd < 0) 
    {
    comm->thread->error_handler(strerror(errno));

    return(FAILURE);
    }

  if ((flags = fcntl(fd,F_GETFL,0)) < 0) 
    {
    comm->thread->error_handler(strerror(errno));

    return(FAILURE);
    }

  if (fcntl(fd,F_SETFL,flags | O_NONBLOCK) < -1) 
    {
    comm->thread->error_handler(strerror(errno));

    return(FAILURE);
    }

  result = connect(fd,(struct sockaddr*)&addr,sizeof(addr));

  if ((result < 0) && (errno != EINPROGRESS)) 
    {
    comm->thread->error_handler("unable to connect to %s:%d (%s)",
      comm->host,
      comm->port,
      strerror(errno));

    close(fd);

    return(FAILURE);
    }

  comm->fd = fd;

  /* send socket descriptor to parent thread */

  if (write(comm->thread->wakeup_fdw,&fd,sizeof(int)) != sizeof(int))
    comm->thread->error_handler(strerror(errno));

  return(SUCCESS);
  }  /* END comm_client_open() */




void comm_close(

  communication_t *comm) /* I (modified) */

  {
  int fd = -1;

  if (comm == NULL)
    {
    return;
    }

  pthread_mutex_lock(&comm_mutex);

  if (comm->fd > 0) 
    {
    fd = comm->fd;

    comm->fd = -1;
    }

  pthread_mutex_unlock(&comm_mutex);

  if (fd < 0)
    {
    return;
    }

  fd = -fd;

  if (write(comm->thread->wakeup_fdw,&fd,sizeof(int)) != sizeof(int))
    comm->thread->error_handler(strerror(errno));

  return;
  }  /* END comm_close() */





/* TODO what if comm_close is called while sending? */

int comm_send(

  communication_t *comm, 
  const void      *data, 
  size_t           size) 
  
  {
  size_t sent = 0;
  fd_set wset;
  struct timeval timeout = {3, 0};
  int result;

  if (comm->fd < 0)
    {
    return(-1);
    }

  FD_ZERO(&wset);
  FD_SET(comm->fd,&wset);

  while (sent < size) 
    {
    int n;

    result = select(comm->fd + 1, NULL, &wset, NULL, &timeout);

    if (result == 0) 
      { 
      /* timed out */

      comm->thread->error_handler("sending timed out");

      return(-1);
      }

    if (result < 0) 
      {
      comm->thread->error_handler(strerror(errno));

      return(-1);
      }

    FD_SET(comm->fd,&wset);

    if ((n = write(comm->fd,&((char *)data)[sent],size - sent)) < 0) 
      {
      comm->thread->error_handler("unable to send data (%s)",
        strerror(errno));

      return(-1);
      }

    sent += n;
    }  /* END while (sent < size) */

  /* SUCCESS */

  return(0);
  }  /* END comm_send() */




void* comm_client_loop(

  comm_thread_t *thread) 
 
  {
  char buffer[BUFSIZ];
  fd_set allrset;
  fd_set allwset;
  fd_set wset;
  fd_set rset;
  int i;
  int wakeup_fd = thread->wakeup_fdr;
  int maxfd = wakeup_fd;

  FD_ZERO(&allrset);
  FD_ZERO(&allwset);
  FD_SET(wakeup_fd, &allrset);

  for (;;) 
    {
    int close_fd = -1;

    rset = allrset;
    wset = allwset;

    if (select(maxfd + 1,&rset,&wset,NULL,NULL) < 0) 
      {
      thread->error_handler("select failed: %s", 
        strerror(errno));

      return(NULL);
      }

    if (FD_ISSET(wakeup_fd,&rset)) 
      {
      int action = 0;

      if (read(wakeup_fd,&action,sizeof(int)) != sizeof(int)) 
        {
        thread->error_handler("pipe read failed: %s",  
          strerror(errno));

        return(NULL);
        }

      if (action == 0) 
        { 
        /* stop */

        thread->error_handler("communication thread stopped");

        return(NULL);
        }

      if (action > 0) 
        { 
        /* open */

        FD_SET(action,&allrset);
        FD_SET(action,&allwset);

        if (action > maxfd)
          maxfd = action;
        }
      else 
        { 
        /* close */

        action = -action;

        FD_CLR(action,&allrset);
        FD_CLR(action,&allwset);

        close_fd = action;
        }
      }    /* END if (FD_ISSET(wakeup_fd,&rset)) */

    for (i = 0;i < thread->connections;i++) 
      {
      communication_t *comm = thread->connection[i];

      int fd;

      fd = comm->fd;

      if (fd < 0)
        continue;

      if (FD_ISSET(fd,&wset)) 
        {
        int error;
        int errsiz = sizeof(int);

        FD_CLR(fd,&allwset);

        if (getsockopt(fd,SOL_SOCKET,SO_ERROR,&error,(socklen_t *)&errsiz) < 0) 
          {
          thread->error_handler(strerror(errno));

          comm_close(comm);

          continue;
          }

        if (error != 0) 
          {
          thread->error_handler(strerror(error));
 
          comm_close(comm);

          continue;
          }

        thread->open_handler(comm);
        }

      if (FD_ISSET(fd,&rset)) 
        {
        int n = read(fd,buffer,BUFSIZ);

        if (n < 0) 
          {
          thread->error_handler(strerror(errno));
          }
        else if (n == 0) 
          {
          thread->close_handler(comm);

          FD_CLR(fd, &allrset);

          comm_close(comm);
          }
        else if (thread->read_handler(comm,buffer,n)) 
          {
          FD_CLR(fd,&allrset);

          comm_close(comm);
          }
        }
      }    /* END for (i) */   

    if ((close_fd > 0) && (close(close_fd) < 0))
      thread->error_handler(strerror(errno));
    }  /* END for (;;) */

  return(NULL);
  }  /* END comm_client_loop() */




/* Listen for incoming connections on @port. */

void *comm_server_loop(

  comm_thread_t *thread) 

  {
  int sd;

  struct sockaddr_in sa;

  char buffer[BUFSIZ];
  struct sockaddr_in ca;
  int cl = sizeof(ca);
  fd_set rset, allset;
  int wakeup_fd = thread->wakeup_fdr;
  int maxfd;
  int i;

  if (thread == NULL)
    {
    /* FAILURE */

    return;
    }

  /* NOTE:  previously initialized w/
       struct sockaddr_in sa = {AF_INET,htons(thread->port),{INADDR_ANY},""}; */

  sa.sin_family = AF_INET;
  sa.sin_port = htons(thread->port);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_zero[0] = '\0';

  sd = socket(AF_INET,SOCK_STREAM,0);

  if (sd < 0) 
    {
    thread->error_handler(strerror(errno));

    return(NULL);
    }

  if (bind(sd,(struct sockaddr*)&sa,sizeof(sa)) < 0) 
    {
    thread->error_handler("unable to bind to port %d (%s)",
      thread->port,
      strerror(errno));

    return(NULL);
    }

  if (listen(sd,16) < 0) 
    {
    thread->error_handler(strerror(errno));

    return(NULL);
    }

  FD_ZERO(&allset);
  FD_SET(sd,&allset);
  FD_SET(wakeup_fd,&allset);

  maxfd = (sd > wakeup_fd) ? sd : wakeup_fd;

  for (;;) 
    {
    rset = allset;

    if (select(maxfd + 1,&rset,NULL,NULL,NULL) < 0) 
      {
      thread->error_handler(strerror(errno));

      return(NULL);
      }

    if (FD_ISSET(wakeup_fd,&rset)) 
      {
      int action = 0;

      if (read(wakeup_fd,&action,sizeof(int)) != sizeof(int)) 
        {
        thread->error_handler(strerror(errno));

        return(NULL);
        }

      if (action == 0) /* stop */
        {
        return(NULL);
        }
      else if (action > 0) /* open */
        {
        FD_SET(action,&allset);
        }
      else /* close */
        {
        FD_CLR(action,&allset);
        }
      }    /* END if (FD_ISSET(wakeup_fd,&rset)) */

    if (FD_ISSET(sd,&rset)) 
      {
      int cd;

      communication_t *comm;

      cd = accept(sd,(struct sockaddr*)&ca,(socklen_t *)&cl);

      if (cd < 0) 
        {
        thread->error_handler(strerror(errno));

        continue;
        }

      if (cl != sizeof(ca)) 
        {
        close(cd);

        thread->error_handler("unknown sockaddr type");

        continue;
        }

      comm = communication_new(thread);

      comm->fd = cd;

      if (comm_thread_add_connection(thread,comm) < 0)
        {
        communication_free(comm);

        continue;
        }

      if (thread->open_handler(comm)) 
        {
        thread->connections--;

        communication_free(comm);

        continue;
        }

      FD_SET(cd,&allset);

      if (cd > maxfd)
        maxfd = cd;

      continue;
      }  /* END if (FD_ISSET(sd,&rset)) */

    for (i = 0;i < thread->connections;i++) 
      {
      communication_t* comm = thread->connection[i];

      if (comm->fd > 0 && FD_ISSET(comm->fd,&rset)) 
        {
        int n;

        n = read(comm->fd,buffer,BUFSIZ);

        if (n < 0) 
          {
          thread->error_handler(strerror(errno));
          }
        else if (n == 0) 
          {
          thread->close_handler(comm);

          FD_CLR(comm->fd,&allset);

          comm_close(comm);
          }
        else if (thread->read_handler(comm,buffer,n)) 
          {
          FD_CLR(comm->fd,&allset);

          comm_close(comm);
          }
        }
      }
    }    /* END for (i) */

  close(sd);

  return(0);
  }  /* END comm_server_loop() */

#endif /* __MTHREADS */

#endif /* __MGRIDENABLE */

/* END MTComm.c */


