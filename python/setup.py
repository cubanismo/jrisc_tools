#!/usr/bin/env python3

###############################################################################
#
# SPDX-License-Identifier: CC0-1.0
#
# Author: James Jones
#
###############################################################################

from distutils.core import setup, Extension

jrisc = Extension('jrisc',
				  define_macros = [
					  ('JDIS_MAJOR', '1'),
					  ('JDIS_MINOR', '2'),
					  ('JDIS_MICRO', '0')],
				  include_dirs = ['..'],
				  sources = ['jrisc_pymodule.c'],
				  extra_objects = ['../libjrisc.a'])

setup(name = 'jrisc',
	  version = '1.2.0',
	  description = 'Jaguar RISC tools',
	  author = 'James Jones',
	  author_email = 'atari@theinnocuous.com',
	  url = 'https://github.com/cubanismo/jrisc_tools',
	  ext_modules = [jrisc])
