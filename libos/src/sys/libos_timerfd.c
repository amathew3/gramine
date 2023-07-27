/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2019 Intel Corporation */


#include "libos_fs.h"
#include "libos_handle.h"
#include "libos_internal.h"
#include "libos_table.h"
#include "libos_utils.h"
#include "linux_eventfd.h"
#include "linux_abi/fs.h"
#include "pal.h"
#include "toml_utils.h"

static int create_timerfd(PAL_HANDLE* tfd, uint64_t initial_count, int flags) {
    int ret;


    PAL_HANDLE hdl = NULL;
    int pal_flags = 0;

    pal_flags |= flags & EFD_NONBLOCK ? PAL_OPTION_NONBLOCK : 0;
    pal_flags |= flags & EFD_SEMAPHORE ? PAL_OPTION_EFD_SEMAPHORE : 0;

    ret = PalStreamOpen(URI_PREFIX_TIMERFD_CREATE, PAL_ACCESS_RDWR, /*share_flags=*/0,
                        PAL_CREATE_IGNORED, pal_flags, &hdl);
    if (ret < 0) {
        log_error("timerfd: creation failure");
        return pal_to_unix_errno(ret);
    }

    /* set the initial count 
    size_t write_size = sizeof(initial_count);
    ret = PalStreamWrite(hdl, /*offset=*///0, &write_size, &initial_count);
    /*if (ret < 0) {
        log_error("eventfd: failed to set initial count");
        return pal_to_unix_errno(ret);
    }
    if (write_size != sizeof(initial_count)) {
        log_error("eventfd: interrupted while setting initial count");
        return -EINTR;
    }*/

    *tfd = hdl;
    return 0;
}

long libos_syscall_timerfd(unsigned int count, int flags) {
    int ret = 0;
    struct libos_handle* hdl = get_new_handle();

    if (!hdl) {
        ret = -ENOMEM;
        goto out;
    }

    hdl->type = TYPE_TIMERFD;
    hdl->fs = &timerfd_builtin_fs;
    hdl->flags = O_RDWR;
    hdl->acc_mode = MAY_READ | MAY_WRITE;

    if ((ret = create_timerfd(&hdl->pal_handle, count, flags)) < 0)
        goto out;

    hdl->info.eventfd.is_semaphore = !!(flags & EFD_SEMAPHORE);

    flags = flags & EFD_CLOEXEC ? FD_CLOEXEC : 0;

    /* get_new_handle() above increments hdl's refcount. Followed by another increment inside
     * set_new_fd_handle. So we need to put_handle() afterwards. */
    int vfd = set_new_fd_handle(hdl, flags, NULL);

    ret = vfd;

out:
    if (hdl)
        put_handle(hdl);

    return ret;
}

long libos_syscall_timerfd_create(unsigned int count) {
    return libos_syscall_timerfd(count, 0);
}
