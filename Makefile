# MIT License
#
# Copyright (c) 2022 Eli Reed
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Simple makefile: Compile all .c files into .o files, generating "dependency" .d files too (see
# https://stackoverflow.com/q/2394609)


SRCDIR 		?= src
SRCS 		:= $(wildcard $(SRCDIR)/*.cpp)

OUTDIR 		:= out
OBJS 		:= $(SRCS:$(SRCDIR)/%.cpp=$(OUTDIR)/%.o)

KEY_DEPS	:= $(KEY_OBJS:%.o=%.d)
DEPS 		:= $(OBJS:%.o=%.d)

BINDIR 		:= bin

BIN  		:= sht30.hex

KEY_DIR		?= reference

KEY_SRCDIR 	:= $(KEY_DIR)/$(SRCDIR)
KEY_SRCS 	:= $(wildcard $(KEY_SRCDIR)/*.cpp)

KEY_OUTDIR 	:= $(KEY_DIR)/$(OUTDIR)
KEY_OBJS 	:= $(KEY_SRCS:$(KEY_SRCDIR)/%.cpp=$(KEY_OUTDIR)/%.o)

KEY_DEPS 	:= $(KEY_OBJS:%.o=%.d)

KEY_BINDIR 	:= $(KEY_DIR)/$(BINDIR)

CC   		:= avr-gcc

MCU 		:= atmega328p

MMCU 		:= -mmcu=$(MCU)

CFLAGS 		:= $(CFLAGS) -Wall -g -mmcu=$(MCU)

# This is a regular comment, that will not be displayed

## ----------------------------------------------------------------------
## This is a help comment. The purpose of this Makefile is to demonstrate
## a simple help mechanism that uses comments defined alongside the rules
## they describe without the need of additional help files or echoing of
## descriptions. Help comments are displayed in the order defined within
## the Makefile.
## ----------------------------------------------------------------------

# https://stackoverflow.com/questions/8889035/how-to-document-a-makefile/64996042#64996042
.PHONY: help
help:	## Show this help
	@egrep -h '\s##\s' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m  %-30s\033[0m %s\n", $$1, $$2}'

.PHONY: build
build: build-key build-solution	## Build answer key and solution

.PHONY: build-key
build-key: $(KEY_OBJS) $(KEY_BINDIR)		## Build answer key
	$(CC) $(CFLAGS) -o $(KEY_BINDIR)/$(BIN) $(KEY_OBJS)

.PHONY: build-solution
build-solution: $(OBJS) $(BINDIR)		## Build solution
	$(CC) $(CFLAGS) -o $(BINDIR)/$(BIN) $(OBJS)

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp $(OUTDIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(KEY_OUTDIR)/%.o: $(KEY_SRCDIR)/%.cpp $(KEY_OUTDIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(BINDIR):
	mkdir -p $@

$(OUTDIR):
	mkdir -p $@

$(KEY_OUTDIR):
	mkdir -p $@ 

$(KEY_BINDIR):
	mkdir -p $@ 

-include $(DEPS)
-include $(KEY_DEPS)

.PHONY: clean
clean:	## Clean up build artifacts
	rm -rf $(OUTDIR)
	rm -rf $(BINDIR)
	rm -rf $(KEY_OUTDIR)
	rm -rf $(KEY_BINDIR)