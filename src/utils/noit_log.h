/*
 * Copyright (c) 2005-2009, OmniTI Computer Consulting, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name OmniTI Computer Consulting, Inc. nor the names
 *      of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _UTILS_NOIT_LOG_H
#define _UTILS_NOIT_LOG_H

#include "noit_defines.h"
#include <pthread.h>
#include <stdarg.h>
#include <sys/uio.h>
#include <sys/time.h>
#include "utils/noit_hash.h"

struct _noit_log_stream_outlet_list {
  struct _noit_log_stream *outlet;
  struct _noit_log_stream_outlet_list *next;
};

typedef struct {
  int (*openop)(struct _noit_log_stream *);
  int (*reopenop)(struct _noit_log_stream *);
  int (*writeop)(struct _noit_log_stream *, const void *, size_t);
  int (*writevop)(struct _noit_log_stream *, const struct iovec *iov, int iovcnt);
  int (*closeop)(struct _noit_log_stream *);
  size_t (*sizeop)(struct _noit_log_stream *);
  int (*renameop)(struct _noit_log_stream *, const char *);
} logops_t;

#ifdef noit_log_impl
typedef struct _noit_log_stream * noit_log_stream_t;
#else
typedef struct _noit_log_stream {
  unsigned enabled:1;
  unsigned debug:1;
  unsigned timestamps:1;
  /* totally private type, don't even think about it */
} * noit_log_stream_t;
#endif

extern noit_log_stream_t noit_stderr;
extern noit_log_stream_t noit_debug;
extern noit_log_stream_t noit_error;

API_EXPORT(int) noit_log_global_enabled();
API_EXPORT(void) noit_log_init();
API_EXPORT(int) noit_log_reopen_all();
API_EXPORT(void) noit_register_logops(const char *name, logops_t *ops);
API_EXPORT(void *) noit_log_stream_get_ctx(noit_log_stream_t);
API_EXPORT(void) noit_log_stream_set_ctx(noit_log_stream_t, void *);
API_EXPORT(const char *) noit_log_stream_get_type(noit_log_stream_t);
API_EXPORT(const char *) noit_log_stream_get_name(noit_log_stream_t);
API_EXPORT(const char *) noit_log_stream_get_path(noit_log_stream_t);

API_EXPORT(noit_log_stream_t)
  noit_log_stream_new(const char *, const char *, const char *,
                      void *, noit_hash_table *);
API_EXPORT(noit_log_stream_t)
  noit_log_stream_new_on_fd(const char *, int, noit_hash_table *);
API_EXPORT(noit_log_stream_t)
  noit_log_stream_new_on_file(const char *, noit_hash_table *);
API_EXPORT(noit_log_stream_t) noit_log_stream_find(const char *);
API_EXPORT(void) noit_log_stream_remove(const char *name);
API_EXPORT(void) noit_log_stream_add_stream(noit_log_stream_t ls,
                                            noit_log_stream_t outlet);
API_EXPORT(noit_log_stream_t)
                 noit_log_stream_remove_stream(noit_log_stream_t ls,
                                               const char *name);
API_EXPORT(void) noit_log_stream_reopen(noit_log_stream_t ls);

#define NOIT_LOG_RENAME_AUTOTIME ((const char *)-1)

API_EXPORT(int) noit_log_stream_rename(noit_log_stream_t ls, const char *);
API_EXPORT(void) noit_log_stream_close(noit_log_stream_t ls);
API_EXPORT(size_t) noit_log_stream_size(noit_log_stream_t ls);
API_EXPORT(size_t) noit_log_stream_written(noit_log_stream_t ls);
API_EXPORT(const char *) noit_log_stream_get_property(noit_log_stream_t ls,
                                                      const char *);
API_EXPORT(void) noit_log_stream_set_property(noit_log_stream_t ls,
                                              const char *, const char *);
API_EXPORT(void) noit_log_stream_free(noit_log_stream_t ls);
API_EXPORT(int) noit_vlog(noit_log_stream_t ls, struct timeval *,
                          const char *file, int line,
                          const char *format, va_list arg);
API_EXPORT(int) noit_log(noit_log_stream_t ls, struct timeval *,
                         const char *file, int line,
                         const char *format, ...)
#ifdef __GNUC__
  __attribute__ ((format (printf, 5, 6)))
#endif
  ;

#define noitLT(ls, t, args...) do { \
  if((ls) && (noit_log_global_enabled() || (ls)->enabled)) \
    noit_log(ls, t, __FILE__, __LINE__, args); \
} while(0)
#define noitL(ls, args...) do { \
  if((ls) && (noit_log_global_enabled() || (ls)->enabled)) { \
    struct timeval __noitL_now; \
    gettimeofday(&__noitL_now, NULL); \
    noit_log(ls, &__noitL_now, __FILE__, __LINE__, args); \
  } \
} while(0)

#define SETUP_LOG(a, b) do { if(!a##_log) a##_log = noit_log_stream_find(#a); \
                             if(!a##_log) { b; } } while(0)

#endif
