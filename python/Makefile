###############################################################################
#
# SPDX-License-Identifier: CC0-1.0
#
# Author: James Jones
#
###############################################################################

#
# Helper Makefile to build the jrisc python extention and install it somewhere.
#
# Set PYTHON_EXT_DIR on the command line to specify where to install, e.g.:
#
#   $ make PYTHON_EXT_DIR=/home/myuser/mylib/python
#
# Then test it out with the test program, e.g.:
#
#   $ PYTHONPATH="/home/myuser/mylib/python:${PYTHONPATH}" ./test.py
#

.PHONY: all

# Debian/Ubuntu stupidness: They know best and auto-enable --user if not running
# in a virtual env or as root. However, --user breaks --target. Hence, when
# running on Debian-based systems, we need to pass their custom anti--user
# parameter --system.
SYSTEM_ARG=$(shell pip3 install --help | grep \\-\\-system | sed 's/^[ \t]*//' | cut -d ' ' -f 1)

INSTALL_ARGS =
ifneq ($(PYTHON_EXT_DIR),)
	INSTALL_ARGS = $(SYSTEM_ARG) --target $(PYTHON_EXT_DIR)
endif

# More Debian/Ubuntu stupidness: Build locally first, because pip on
# Debian/Ubuntu seem to copy all the files to some temp directory first, which
# breaks the ".." relative path include_dir and extra_objects stuff.
all:
	./setup.py build
	pip3 install --upgrade $(INSTALL_ARGS) .

.PHONY: clean
clean:
	rm -rf build dist jrisc.egg-info __pycache__
