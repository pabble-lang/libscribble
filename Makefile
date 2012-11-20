# 
# Makefile
#

ROOT := .
include $(ROOT)/Common.mk

all: libscribble tool

libscribble:
	$(MAKE) --dir=src

tool:
	$(MAKE) --dir=src tool

libscribble.a: libscribble
	$(RM) $(LIB_DIR)/$@
	$(AR) $(ARFLAGS) $(BUILD_DIR)/$@ $(BUILD_DIR)/*.o
	$(CP) -v $(BUILD_DIR)/$@ $(LIB_DIR)/$@

include $(ROOT)/Rules.mk
