/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#ifndef JRISC_BASE_H_
#define JRISC_BASE_H_

enum JRISC_Error {
#define JRISC_ERROR(error) JRISC_##error,
#include "jrisc_errortable.h"
#undef JRISC_ERROR
};

#define JRISC_GPU_RAM 0xf03000
#define JRISC_DSP_RAM 0xf1b000

#endif /* JRISC_BASE_H_ */
