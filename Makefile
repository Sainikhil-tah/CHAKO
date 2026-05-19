CXX := g++

CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS :=

TARGET := chess

SRCDIR := src
INCDIR := include
OBJDIR := build/obj

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)
	@echo "Build successful: ./$(TARGET)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build $(TARGET)

-include $(OBJS:.o=.d)
