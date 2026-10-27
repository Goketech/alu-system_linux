#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

/* size_t */
#include <stddef.h>

/*
 * Normally, MAX_PARAMS is always 6 per the x86-64 ABI:
 * https://linux-kernel-labs.github.io/refs/heads/master/lectures/syscalls.html
 */
#define MAX_PARAMS	6

/**
 * enum type_e - enumerates the data types that appear in syscall parameters
 * and returns
 *
 * Listed includes are from Ubuntu 14.04. If given a choice in the man page,
 * POSIX.1-2001 include is listed. Some types are defined in multiple headers.
 * Sometimes actual definition will be in a wrapped header like those in bits/,
 * in which case the highest-level wrapper found is named.
 *
 *  type_t                       data type                 include to define
 *
 * @AIO_CONTEXT_T:               aio_context_t             <linux/aio_abi.h>
 * @AIO_CONTEXT_T_P:             aio_context_t *           <linux/aio_abi.h>
 * @CADDR_T:                     caddr_t                   <sys/types.h>
 * @CAP_USER_DATA_T:             cap_user_data_t           <linux/capability.h>
 * @CAP_USER_HEADER_T:           cap_user_header_t         <linux/capability.h>
 * @CHAR_P:                      char *
 * @CHAR_PP:                     char **
 * @CLOCKID_T:                   clockid_t                 <time.h>
 * @CLOCK_T:                     clock_t                   <time.h>
 * @CPU_SET_T_P:                 cpu_set_t *               <sched.h>
 * @DEV_T:                       dev_t                     <sys/types.h> or
 *                                                         <sys/stat.h>
 * @ENUM___PTRACE_REQUEST:       enum __ptrace_request     <sys/ptrace.h>
 * @FD_SET_P:                    fd_set *                  <sys/select.h>
 * @GID_T:                       gid_t                     <sys/types.h> or
 *                                                         <sys/stat.h>
 * @GID_T_P:                     gid_t *                   <sys/types.h> or
 *                                                         <sys/stat.h>
 * @IDTYPE_T:                    idtype_t                  <sys/wait.h>
 * @ID_T:                        id_t                      <sys/types.h> or
 *                                                         <sys/resource.h> or
 *                                                         <sys/wait.h>
 * @INT:                         int
 * @INT_P:                       int *
 * @KEY_SERIAL_T:                key_serial_t
 *      (keyutils.h not found, type used by add_key(2) and request_key(2))
 * @KEY_T:                       key_t                     <sys/types.h> or
 *                                                         <sys/ipc.h>
 * @LOFF_T_P:                    loff_t *                  <sys/types.h>
 * @LONG:                        long
 * @LONG_P:                      long *
 * @MODE_T:                      mode_t                    <sys/types.h> or
 *                                                         <sys/mman.h> or
 *                                                         <sys/stat.h> or
 *                                                         <sys/ipc.h> or
 *                                                         <fcntl.h>
 * @MQD_T:                       mqd_t                     <mqueue.h>
 * @NFDS_T:                      nfds_t                    <sys/poll.h>
 * @OFF64_T:                     off64_t                   <stdio.h> or
 *                                                         <sys/types.h> or
 *                                                         <unistd.h> or
 *                                                         <fcntl.h>
 * @OFF_T:                       off_t                     <stdio.h> or
 *                                                         <sys/types.h> or
 *                                                         <sys/mman.h> or
 *                                                         <sys/stat.h> or
 *                                                         <unistd.h> or
 *                                                         <fcntl.h>
 * @OFF_T_P:                     off_t *                   (see off_t)
 * @PID_T:                       pid_t                     <sched.h> or
 *                                                         <termios.h> or
 *                                                         <utmpx.h> or
	{266, "clock_getres", INT, 2, {CLOCKID_T, STRUCT_TIMESPEC_P, -1, -1,
		-1, -1}},
	{267, "clock_nanosleep", INT, 4, {CLOCKID_T, INT, STRUCT_TIMESPEC_P,
		STRUCT_TIMESPEC_P, -1, -1}},
	{268, "statfs64", INT, 2, {CHAR_P, STRUCT_STATFS_P, -1, -1, -1, -1}},
	{269, "fstatfs64", INT, 2, {INT, STRUCT_STATFS_P, -1, -1, -1, -1}},
	{270, "tgkill", INT, 3, {INT, INT, INT, -1, -1, -1}},
	{271, "utimes", INT, 2, {CHAR_P, STRUCT_TIMEVAL_P, -1, -1, -1, -1}},
	{272, "fadvise64_64", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{273, "vserver", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{274, "mbind", INT, 6, {VOID_P, UNSIGNED_LONG, INT, UNSIGNED_LONG_P,
		UNSIGNED_LONG, UNSIGNED_INT}},
	{275, "get_mempolicy", INT, 5, {INT_P, UNSIGNED_LONG_P, UNSIGNED_LONG,
		UNSIGNED_LONG, UNSIGNED_LONG, -1}},
	{276, "set_mempolicy", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{277, "mq_open", MQD_T, 2, {CHAR_P, INT, -1, -1, -1, -1}},
	{278, "mq_unlink", INT, 1, {CHAR_P, -1, -1, -1, -1, -1}},
	{279, "mq_timedsend", INT, 5, {MQD_T, CHAR_P, SIZE_T,
		UNSIGNED_INT, STRUCT_TIMESPEC_P, -1}},
	{280, "mq_timedreceive", SSIZE_T, 5, {MQD_T, CHAR_P, SIZE_T,
		UNSIGNED_INT_P, STRUCT_TIMESPEC_P, -1}},
	{281, "mq_notify", INT, 2, {MQD_T, STRUCT_SIGEVENT_P, -1, -1, -1, -1}},
	{282, "mq_getsetattr", INT, 3, {MQD_T, STRUCT_MQ_ATTR_P,
		STRUCT_MQ_ATTR_P, -1, -1, -1}},
	{283, "kexec_load", LONG, 4, {UNSIGNED_LONG, UNSIGNED_LONG,
		STRUCT_KEXEC_SEGMENT_P, UNSIGNED_LONG, -1, -1}},
	{284, "waitid", INT, 4, {IDTYPE_T, ID_T, SIGINFO_T_P, INT, -1, -1}},
	{286, "add_key", KEY_SERIAL_T, 5, {CHAR_P, CHAR_P, VOID_P, SIZE_T,
		KEY_SERIAL_T, -1}},
	{287, "request_key", KEY_SERIAL_T, 4, {CHAR_P, CHAR_P, CHAR_P,
		KEY_SERIAL_T, -1, -1}},
	{288, "keyctl", LONG, 2, {INT, VARARGS, -1, -1, -1, -1}},
	{289, "ioprio_set", INT, 3, {INT, INT, INT, -1, -1, -1}},
	{290, "ioprio_get", INT, 2, {INT, INT, -1, -1, -1, -1}},
	{291, "inotify_init", INT, 1, {VOID, -1, -1, -1, -1, -1}},
	{292, "inotify_add_watch", INT, 3, {INT, CHAR_P, UINT32_T, -1, -1, -1}},
	{293, "inotify_rm_watch", INT, 2, {INT, INT, -1, -1, -1, -1}},
	{294, "migrate_pages", LONG, 4, {INT, UNSIGNED_LONG, UNSIGNED_LONG_P,
		UNSIGNED_LONG_P, -1, -1}},
	{295, "openat", INT, 3, {INT, CHAR_P, INT, -1, -1, -1}},
	{296, "mkdirat", INT, 3, {INT, CHAR_P, MODE_T, -1, -1, -1}},
	{297, "mknodat", INT, 4, {INT, CHAR_P, MODE_T, DEV_T, -1, -1}},
	{298, "fchownat", INT, 5, {INT, CHAR_P, UID_T, GID_T, INT, -1}},
	{299, "futimesat", INT, 3, {INT, CHAR_P, STRUCT_TIMEVAL_P, -1, -1, -1}},
	{300, "fstatat64", INT, 4, {INT, CHAR_P, STRUCT_STAT_P, INT, -1, -1}},
	{301, "unlinkat", INT, 3, {INT, CHAR_P, INT, -1, -1, -1}},
	{302, "renameat", INT, 4, {INT, CHAR_P, INT, CHAR_P, -1, -1}},
	{303, "linkat", INT, 5, {INT, CHAR_P, INT, CHAR_P, INT, -1}},
	{304, "symlinkat", INT, 3, {CHAR_P, INT, CHAR_P, -1, -1, -1}},
	{305, "readlinkat", INT, 4, {INT, CHAR_P, CHAR_P, SIZE_T, -1, -1}},
	{306, "fchmodat", INT, 4, {INT, CHAR_P, MODE_T, INT, -1, -1}},
	{307, "faccessat", INT, 4, {INT, CHAR_P, INT, INT, -1, -1}},
	{308, "pselect6", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{309, "ppoll", INT, 4, {STRUCT_POLLFD_P, NFDS_T, STRUCT_TIMESPEC_P,
		SIGSET_T_P, -1, -1}},
	{310, "unshare", INT, 1, {INT, -1, -1, -1, -1, -1}},
	{311, "set_robust_list", LONG, 2, {STRUCT_ROBUST_LIST_HEAD_P, SIZE_T,
		-1, -1, -1, -1}},
	{312, "get_robust_list", LONG, 3, {INT, STRUCT_ROBUST_LIST_HEAD_PP,
		SIZE_T_P, -1, -1, -1}},
	{313, "splice", SSIZE_T, 6, {INT, LOFF_T_P, INT, LOFF_T_P, SIZE_T,
		UNSIGNED_INT}},
	{314, "sync_file_range", INT, 4, {INT, OFF64_T, OFF64_T, UNSIGNED_INT,
		-1, -1}},
	{315, "tee", SSIZE_T, 4, {INT, INT, SIZE_T, UNSIGNED_INT, -1, -1}},
	{316, "vmsplice", SSIZE_T, 4, {INT, STRUCT_IOVEC_P, UNSIGNED_LONG,
		UNSIGNED_INT, -1, -1}},
	{317, "move_pages", LONG, 6, {INT, UNSIGNED_LONG, VOID_PP, INT_P,
		INT_P, INT}},
	{318, "getcpu", INT, 3, {UNSIGNED_INT_P, UNSIGNED_INT_P,
		STRUCT_GETCPU_CACHE_P, -1, -1, -1}},
	{319, "epoll_pwait", INT, 5, {INT, STRUCT_EPOLL_EVENT_P, INT, INT,
		SIGSET_T_P, -1}},
	{320, "utimensat", INT, 4, {INT, CHAR_P, STRUCT_TIMESPEC_P, INT,
		-1, -1}},
	{321, "signalfd", INT, 3, {INT, SIGSET_T_P, INT, -1, -1, -1}},
	{322, "timerfd_create", INT, 2, {INT, INT, -1, -1, -1, -1}},
	{323, "eventfd", INT, 2, {UNSIGNED_INT, INT, -1, -1, -1, -1}},
	{324, "fallocate", INT, 4, {INT, INT, OFF_T, OFF_T, -1, -1}},
	{325, "timerfd_settime", INT, 4, {INT, INT, STRUCT_ITIMERSPEC_P,
		STRUCT_ITIMERSPEC_P, -1, -1}},
	{326, "timerfd_gettime", INT, 2, {INT, STRUCT_ITIMERSPEC_P, -1, -1, -1,
		-1}},
	{327, "signalfd4", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{328, "eventfd2", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{329, "epoll_create1", INT, 1, {INT, -1, -1, -1, -1, -1}},
	{330, "dup3", INT, 3, {INT, INT, INT, -1, -1, -1}},
	{331, "pipe2", INT, 2, {INT_P, INT, -1, -1, -1, -1}},
	{332, "inotify_init1", INT, 1, {INT, -1, -1, -1, -1, -1}},
	{333, "preadv", SSIZE_T, 4, {INT, STRUCT_IOVEC_P, INT, OFF_T, -1, -1}},
	{334, "pwritev", SSIZE_T, 4, {INT, STRUCT_IOVEC_P, INT, OFF_T, -1, -1}},
	{335, "rt_tgsigqueueinfo", INT, 4, {PID_T, PID_T, INT, SIGINFO_T_P, -1,
		-1}},
	{336, "perf_event_open", INT, 5, {STRUCT_PERF_EVENT_ATTR_P, PID_T, INT,
		INT, UNSIGNED_LONG, -1}},
	{337, "recvmmsg", INT, 5, {INT, STRUCT_MMSGHDR_P, UNSIGNED_INT,
		UNSIGNED_INT, STRUCT_TIMESPEC_P, -1}},
	{338, "fanotify_init", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{339, "fanotify_mark", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{340, "prlimit64", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{341, "name_to_handle_at", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{342, "open_by_handle_at", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{343, "clock_adjtime", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{344, "syncfs", INT, 1, {INT, -1, -1, -1, -1, -1}},
	{345, "sendmmsg", INT, 4, {INT, STRUCT_MMSGHDR_P, UNSIGNED_INT,
		UNSIGNED_INT, -1, -1}},
	{346, "setns", INT, 2, {INT, INT, -1, -1, -1, -1}},
	{347, "process_vm_readv", SSIZE_T, 6, {PID_T, STRUCT_IOVEC_P,
		UNSIGNED_LONG, STRUCT_IOVEC_P, UNSIGNED_LONG, UNSIGNED_LONG}},
	{348, "process_vm_writev", SSIZE_T, 6, {PID_T, STRUCT_IOVEC_P,
		UNSIGNED_LONG, STRUCT_IOVEC_P, UNSIGNED_LONG, UNSIGNED_LONG}},
	{349, "kcmp", INT, 5, {PID_T, PID_T, INT, UNSIGNED_LONG, UNSIGNED_LONG,
		-1}},
	{350, "finit_module", INT, 3, {INT, CHAR_P, INT, -1, -1, -1}},
	{351, "sched_setattr", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{352, "sched_getattr", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{353, "renameat2", -1, 0, {-1, -1, -1, -1, -1, -1}},
	{354, "seccomp", -1, 0, {-1, -1, -1, -1, -1, -1}}
};


#endif /* !_SYSCALLS_H_ */
