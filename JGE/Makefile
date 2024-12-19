CXX      := arm-vita-eabi-g++
CXXFLAGS := -Wall -w -std=c++11 -D__VITA__
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
LIB_DIR  := $(BUILD)/lib
TARGET   := libjge.a
SRC_DIR  := src
HEADERS_DIR := include

SOURCES := $(shell find $(SRC_DIR) -name "*.cpp")
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

CXXFLAGS += -DGLM_ENABLE_EXPERIMENTAL -DGL_GLEXT_PROTOTYPES #-mfloat-abi=softfp 
#CFLAGS += -mfloat-abi=softfp
#LDFLAGS += -mfloat-abi=softfp


all: $(LIB_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(HEADERS_DIR) -o $@ -c $<

$(LIB_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(LIB_DIR)
	arm-vita-eabi-ar rcsv $(LIB_DIR)/$(TARGET) $(OBJECTS)

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(LIB_DIR)/*
