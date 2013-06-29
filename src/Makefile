# 
# src/Makefile
#

ROOT := ..
include $(ROOT)/Common.mk

all: sharedlib objs

objs: $(BUILD_DIR)/parser.o $(BUILD_DIR)/lexer.o \
	$(BUILD_DIR)/check.o $(BUILD_DIR)/check_constants.o $(BUILD_DIR)/check_defined_roles.o $(BUILD_DIR)/check_index_bindings.o \
	$(BUILD_DIR)/print.o $(BUILD_DIR)/project.o

sharedlib: $(BUILD_DIR)/scribble_parser.so $(BUILD_DIR)/scribble_lexer.so $(BUILD_DIR)/scribble_check.so $(BUILD_DIR)/scribble_print.so $(BUILD_DIR)/scribble_project.so

tool: objs parser-tool.c scribble-tool.c
	$(CC) $(CFLAGS) parser-tool.c \
	  $(BUILD_DIR)/parser.o $(BUILD_DIR)/lexer.o \
	  -o $(BIN_DIR)/parser-tool $(LDFLAGS)
	$(CC) $(CFLAGS) scribble-tool.c \
	  $(BUILD_DIR)/*.o \
	  -o $(BIN_DIR)/scribble-tool $(LDFLAGS)

parser.h: parser.y
	yacc -d -o parser.c parser.y

parser.c: parser.y
	yacc -d -o parser.c parser.y

lexer.c: lexer.l
	lex -o lexer.c lexer.l

# Parser source file from yacc output
$(INCLUDE_DIR)/scribble/parser.h: parser.h
	mv parser.h $(INCLUDE_DIR)/scribble/parser.h

# Empty placeholder file
$(INCLUDE_DIR)/scribble/lexer.h:
	touch $(INCLUDE_DIR)/scribble/lexer.h

$(BUILD_DIR)/check_%.o: $(INCLUDE_DIR)/scribble/check.h
	$(CC) $(CFLAGS) -c check_$*.c \
	  -o $(BUILD_DIR)/check_$*.o

$(BUILD_DIR)/scribble_check.so: $(BUILD_DIR)/check.o $(BUILD_DIR)/check_constants.o $(BUILD_DIR)/check_defined_roles.o $(BUILD_DIR)/check_index_bindings.o
	$(CC) $(CFLAGS) -shared \
	  $(BUILD_DIR)/check.o \
	  $(BUILD_DIR)/check_constants.o \
	  $(BUILD_DIR)/check_defined_roles.o \
	  $(BUILD_DIR)/check_index_bindings.o \
	  -o $(BUILD_DIR)/scribble_check.so

include $(ROOT)/Rules.mk
