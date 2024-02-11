NAME := demo

SRCDIR := src
OBJDIR := .obj

SRC := $(shell find $(SRCDIR) -name *.cc)
OBJ := $(SRC:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

CXX := clang++
CXXFLAGS := -std=gnu++26 -Iinclude -MMD -MP -Weverything -Wno-switch -Wno-c++98-compat -Wno-pre-c++14-compat -Wno-padded -Wno-implicit-int-float-conversion -Wno-suggest-destructor-override -Wno-c++20-extensions

ifdef $(OS)
	LDFLAGS := -LC:\MinGW\bin
endif
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system

$(NAME): $(OBJ)
	$(CXX) -o$@ $^ $(LDFLAGS) $(LDLIBS)

-include $(OBJ:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o$@ $<

clean:
	rm -rf $(OBJDIR)

distclean: clean
	rm -f $(NAME)

re: distclean
	make

.PHONY: all clean distclean re
