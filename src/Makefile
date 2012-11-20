# 
# src/Makefile
#

ROOT := ..
include $(ROOT)/Common.mk

all: sharedlib objs

objs: $(BUILD_DIR)/parser.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/check.o $(BUILD_DIR)/print.o $(BUILD_DIR)/project.o

sharedlib: $(BUILD_DIR)/scribble_parser.so $(BUILD_DIR)/scribble_lexer.so $(BUILD_DIR)/scribble_check.so $(BUILD_DIR)/scribble_print.so $(BUILD_DIR)/scribble_project.so

tool: objs scribble-tool.c
	$(CC) $(CFLAGS) scribble-tool.c \
	  $(BUILD_DIR)/parser.o $(BUILD_DIR)/lexer.o \
	  $(BUILD_DIR)/check.o $(BUILD_DIR)/print.o $(BUILD_DIR)/project.o \
	  -o $(BIN_DIR)/scribble-tool $(LDFLAGS)

parser.h: parser.y
	yacc -d -o parser.c parser.y

parser.c: parser.y
	yacc -d -o parser.c parser.y

lexer.c: lexer.l
	lex -o lexer.c lexer.l

# Parser source file from yacc output
$(INCLUDE_DIR)/parser.h: parser.h
	mv parser.h $(INCLUDE_DIR)

# Empty placeholder file
$(INCLUDE_DIR)/lexer.h:
	touch $(INCLUDE_DIR)/lexer.h

include $(ROOT)/Rules.mk
