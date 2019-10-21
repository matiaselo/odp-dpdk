/* Copyright (c) 2021, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef ODP_THREAD_INTERNAL_H_
#define ODP_THREAD_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Read ODP thread CPU IDs
 *
 * @param[out] cpu_ids      CPU ID array
 * @param      max_num      Max number of CPU IDs to write
 * @return Actual number of CPU IDs written to the output array
 */
int _odp_thread_cpu_ids(unsigned int *cpu_ids, int max_num);

/**
 * Read current thread mask all epoch value
 *
 * @return Thread mask all epoch value
 */
uint64_t _odp_thread_thrmask_epoch(void);

#ifdef __cplusplus
}
#endif
#endif
