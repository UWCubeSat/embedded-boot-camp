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

KEY_SRCDIR 	:= ./key
KEY_SRCS 	:= $(wildcard $(KEY_SRCDIR)/*.cpp)

KEY_OUTDIR 	:= ./key/out
KEY_OBJS 	:= $(KEY_SRCS:$(KEY_SRCDIR)/%.c=$(KEY_OUTDIR)/%.o)

KEY_DEPS 	:= $(KEY_OBJS:%.o=%.d)

KEY_BINDIR 	:= ./key/bin

KEY_BIN     := sht30_key

SRCDIR 		:= ./src
SRCS 		:= $(wildcard $(SRCDIR)/*.c)

OUTDIR 		:= ./out
OBJS 		:= $(SRCS:$(SRCDIR)/%.c=$(OUTDIR)/%.o)

DEPS 		:= $(OBJS:%.o=%.d)

BINDIR 		:= ./bin

BIN  		:= sht30

CC   		:= avr-gcc

MCU 		:= atmega328p

CFLAGS 		:= $(CFLAGS) -Wall -g -mmcu=$(MCU)



.PHONY: build
build: build-key build-answer

.PHONY: build-key
build-key: $(KEY_OBJS)
	$(CC) $(CFLAGS) -o $(KEY_BINDIR)/$(KEY_BIN) $(KEY_OBJS)

.PHONY: build-answer
build-answer:
	$(CC) $(CFLAGS) -o $(BINDIR)/$(BIN) $(OBJS)

$(OUTDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(KEY_OUTDIR)/%.o: $(KEY_SRCDIR)/%.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

.PHONY: help
help:
	@echo "Making all targets for $(MCU)"

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(BINDIR)/$(BIN)
	rm -f $(KEY_OBJS)
	rm -f $(KEY_DEPS)
	rm -f $(KEY_BINDIR)/$(KEY_BIN)

.PHONY: all clean