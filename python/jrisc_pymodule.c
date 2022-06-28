/*
 * SPDX-License-Identifier: CC0-1.0
 *
 * Author: James Jones
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "jrisc_base.h"
#include "jrisc_ctx.h"
#include "jrisc_ctx_mem.h"
#include "jrisc_inst.h"
#include "jrisc_inst_string.h"

static PyObject *
jriscPy_disassemble(PyObject *self, PyObject *args, PyObject *kwMap)
{
	const char *mem;
	PyObject *retObj;
	PyObject *outList;
	PyObject *outString;
	static char *argKeywords[] = { "" /* byte array */, "baseAddress", "dsp", "machineCode", NULL };
	char *tempString = NULL;
	Py_ssize_t len;
	struct JRISC_Context *ctx;
	struct JRISC_Instruction inst;
	uint64_t base = 0xffffffffffffffffull;
	size_t instLength;
	int appendRes;
	int useDSP = 0;
	int showMachine = 0;
	enum JRISC_CPU cpu = JRISC_gpu;

	if (!PyArg_ParseTupleAndKeywords(args, kwMap, "y#|$Kpp", argKeywords,
									 &mem, &len, &base, &useDSP, &showMachine)) {
		Py_RETURN_NONE;
	}

	if (useDSP) {
		cpu = JRISC_dsp;
	}

	if (base == 0xffffffffffffffffull) {
		base = (cpu == JRISC_gpu) ? JRISC_GPU_RAM : JRISC_DSP_RAM;
	}

	if (jriscContextFromMemory(mem, len, NULL, 0, base, &ctx) != JRISC_success) {
		/* Throw some exception */
		return PyErr_NoMemory();
	}

	outList = PyList_New(0);

	if (!outList) {
		Py_RETURN_NONE;
	}

	while (jriscInstructionRead(ctx, cpu, &inst) == JRISC_success) {
		instLength = 0;
		jriscInstructionToString(&inst,
								 JRISC_STRINGFLAG_ADDRESS |
								 JRISC_STRINGFLAG_MACHINE_CODE,
								 NULL, &instLength);

		/* No such thing a zero-length valid instruction */
		assert(instLength);

		tempString = malloc(instLength);
		if (!tempString) {
			retObj = PyErr_NoMemory();
			goto error;
		}

		jriscInstructionToString(&inst,
								 JRISC_STRINGFLAG_ADDRESS |
								 (showMachine ? JRISC_STRINGFLAG_MACHINE_CODE : 0),
								 tempString, &instLength);

		outString = Py_BuildValue("s#", tempString, instLength - 1);

		free(tempString);

		if (!outString) {
			Py_INCREF(Py_None);
			retObj = Py_None;
			goto error;
		}

		appendRes = PyList_Append(outList, outString);
		Py_DECREF(outString);

		if (appendRes) {
			Py_INCREF(Py_None);
			retObj = Py_None;
			goto error;
		}
	}

	Py_INCREF(outList);
	retObj = outList;

error:
	Py_DECREF(outList);
	jriscContextDestroy(ctx);

	return retObj;
}

static PyMethodDef jriscMethods[] = {
	{"disassemble", (PyCFunction)jriscPy_disassemble, METH_VARARGS | METH_KEYWORDS, "disassemble a buffer"},

	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef jriscModule = {
	PyModuleDef_HEAD_INIT,
	"jrisc",	/* name of module */
	NULL,
	-1,
	jriscMethods
};

PyMODINIT_FUNC
PyInit_jrisc(void)
{
	return PyModule_Create(&jriscModule);
}
