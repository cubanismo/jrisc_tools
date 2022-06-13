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

#endif /* JRISC_BASE_H_ */
