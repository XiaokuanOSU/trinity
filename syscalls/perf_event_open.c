/*
 * SYSCALL_DEFINE5(perf_event_open,
	 struct perf_event_attr __user *, attr_uptr,
	 pid_t, pid, int, cpu, int, group_fd, unsigned long, flags)
 */

#include <stdlib.h>
#include <string.h>
#include <linux/perf_event.h>
#include "sanitise.h"
#include "compat.h"
#include "maps.h"
#include "shm.h"

static void sanitise_perf_event_open(int childno)
{
	struct perf_event_attr *attr;

	shm->a1[childno] = (unsigned long) page_rand;
	attr = (struct perf_event_attr *) shm->a1[childno];

	/* this makes sure we clear out the reserved fields. */
	memset(page_rand, 0, sizeof(struct perf_event_attr));

	switch(rand() % 6) {
		case 0:	attr->type = PERF_TYPE_HARDWARE;
			switch(rand() % 10) {
				case 0: attr->config=PERF_COUNT_HW_CPU_CYCLES;
					break;
				case 1: attr->config=PERF_COUNT_HW_INSTRUCTIONS;
					break;
				case 2: attr->config=PERF_COUNT_HW_CACHE_REFERENCES;
					break;
				case 3: attr->config=PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
					break;
				case 4: attr->config=PERF_COUNT_HW_BRANCH_MISSES;
					break;
				case 5: attr->config=PERF_COUNT_HW_BUS_CYCLES;
					break;
				case 6: attr->config=PERF_COUNT_HW_STALLED_CYCLES_FRONTEND;
					break;
				case 7: attr->config=PERF_COUNT_HW_STALLED_CYCLES_BACKEND;
					break;
				case 8: attr->config=PERF_COUNT_HW_REF_CPU_CYCLES;
					break;
				case 9: attr->config = rand();
					break;
				default: break;
			}
			break;
		case 1: attr->type = PERF_TYPE_SOFTWARE;
			switch(rand() % 10) {
				case 0: attr->config=PERF_COUNT_SW_CPU_CLOCK;
					break;
				case 1: attr->config=PERF_COUNT_SW_TASK_CLOCK;
					break;
				case 2: attr->config=PERF_COUNT_SW_PAGE_FAULTS;
					break;
				case 3: attr->config=PERF_COUNT_SW_CONTEXT_SWITCHES;
					break;
				case 4: attr->config=PERF_COUNT_SW_CPU_MIGRATIONS;
					break;
				case 5: attr->config=PERF_COUNT_SW_PAGE_FAULTS_MIN;
					break;
				case 6: attr->config=PERF_COUNT_SW_PAGE_FAULTS_MAJ;
					break;
				case 7: attr->config=PERF_COUNT_SW_ALIGNMENT_FAULTS;
					break;
				case 8: attr->config=PERF_COUNT_SW_EMULATION_FAULTS;
					break;
				case 9: attr->config=rand();
				default: break;
			}
			break;
		case 2: attr->type = PERF_TYPE_TRACEPOINT;
			break;
		case 3: attr->type = PERF_TYPE_HW_CACHE;
			break;
		case 4: attr->type = PERF_TYPE_RAW;
			/* can be arbitrary 64-bit value */
			/* there are some constraints we can add */
			/* to make it more likely to be a valid event */
			attr->config = rand();

			break;
		case 5: attr->type = PERF_TYPE_BREAKPOINT;
			break;
		default: break;
	}

	switch(rand() % 2) {
		case 0: attr->size = sizeof(struct perf_event_attr);
			break;
		case 1: attr->size = get_len();
		default: break;
	}
}

struct syscall syscall_perf_event_open = {
	.name = "perf_event_open",
	.num_args = 5,
	.arg1name = "attr_uptr",
	.arg1type = ARG_ADDRESS,
	.arg2name = "pid",
	.arg2type = ARG_PID,
	.arg3name = "cpu",
	.arg3type = ARG_CPU,
	.arg4name = "group_fd",
	.arg4type = ARG_FD,
	.arg5name = "flags",
	.arg5type = ARG_LIST,
	.arg5list = {
		.num = 3,
		.values = { PERF_FLAG_FD_NO_GROUP, PERF_FLAG_FD_OUTPUT, PERF_FLAG_PID_CGROUP },
	},
	.sanitise = sanitise_perf_event_open,
	.flags = NEED_ALARM,
};
