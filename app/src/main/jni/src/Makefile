#
# bada
# Copyright (c) 2008-2009 Samsung Electronics, Inc.
# All rights reserved. 
# 
# This software is the confidential and proprietary information
# of Samsung Electronics, Inc. ("Confidential Information").  You
# shall not disclose such Confidential Information and shall use
# it only in accordance with the terms of the license agreement
# you entered into with Samsung Electronics. 
# 
#  
# Makefile for Applications
# 

$(CURMOD)_OBJECTS :=  \
	$(OBJ_DIR)/GlesCube11.o \
	$(OBJ_DIR)/GlesCube11Entry.o \
	$(OBJ_DIR)/ImageData1.o \
	$(OBJ_DIR)/ImageData2.o

CFLAGS += \
	-I$(CURMOD)

ALL_OBJECTS += $($(CURMOD)_OBJECTS)

#===============================================================================
#                           Build source file
#===============================================================================

$(CURMOD): $($(CURMOD)_OBJECTS)

$(OBJ_DIR)/%.o: $(CURMOD)/%.cpp
	@echo "  Compiling $<"
	$(ARMCPP) $(CFLAGS) $(MSG_FILE) $(OBJ_CMD) $(OBJ_FILE) $< 2> error.log