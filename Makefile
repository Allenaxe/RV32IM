# ==========================
#   CPU Pipeline Simulator
# ==========================

CXX       := g++
CXXFLAGS  := -std=c++20 -Wall -Wextra -O1 -Iinclude -Ilib/tabulate/include -MMD -MP

# Project structure
TARGET    := cpu_simulator
SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build

# Find all .cpp files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# Convert src/*.cpp → build/*.o
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Dependency files build/*.d
DEPS := $(OBJS:.o=.d)

# ==========================
#     Build target
# ==========================
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compiling rule
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if missing
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ==========================
#       Utilities
# ==========================
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

rebuild: clean all

run: $(TARGET)
	./$(TARGET)

# Include dependencies
-include $(DEPS)

.PHONY: all clean rebuild run