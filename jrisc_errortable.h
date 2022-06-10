/*
 * Jaguar RISC library error codes
 *
 * To use this file, define a macro named as such:
 *
 *    JRISC_ERROR(error)
 *
 * Then include this file. The macro will be evaluated once for each Jaguar
 * RISC library error code.
 */
JRISC_ERROR(success)				/* = 0: Must be first! */

JRISC_ERROR(ERROR_outOfMemory)
JRISC_ERROR(ERROR_ioError)
JRISC_ERROR(ERROR_invalidValue)
JRISC_ERROR(ERROR_invalidReg)
JRISC_ERROR(ERROR_invalidRegType)
JRISC_ERROR(ERROR_invalidOpCode)
