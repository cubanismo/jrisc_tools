/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#ifndef JRISC_REGTYPE_H_
#define JRISC_REGTYPE_H_

enum JRISC_RegType {
	JRISC_reg,
	JRISC_indirect,
	JRISC_condition,
	JRISC_simmediate,		/* Signed immediate [-16,+15] */
	JRISC_uimmediate,		/* Unsigned immedaite [1,32] */
	JRISC_zuimmediate,		/* Unsigned immedaite [0,31] */
	JRISC_shlimmediate,		/* Unsigned immediate 32-[0,31] */
	JRISC_pcoffset,
	JRISC_flag,
	JRISC_unused,
};

#endif /* JRISC_REGTYPE_H_ */
