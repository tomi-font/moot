NAME := libmoot.so

SRCDIR := src
OBJDIR := .obj

SRC := $(shell find $(SRCDIR) -name '*.cc')
OBJ := $(SRC:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

DISABLED_WARNINGS := \
	implicit-int-float-conversion \
	padded \
	suggest-destructor-override \
	switch-enum \
	c99-designator \
	pre-c++14-compat \
	pre-c++20-compat-pedantic \
	c++98-compat \
	c++20-compat \
	c++20-extensions \
	gnu-anonymous-struct \
	nested-anon-types \
	shadow-field-in-constructor \
	global-constructors \
	exit-time-destructors \
	unsafe-buffer-usage \

CXX := clang++
CXXFLAGS := -fPIC -std=gnu++26 -g -O0 -MMD -MP -I include -isystem lib -Weverything $(addprefix -Wno-,$(DISABLED_WARNINGS))

ifdef $(OS)
	LDFLAGS := -LC:\MinGW\bin
endif
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system -llua

$(NAME): $(OBJ)
	$(CXX) -shared -o$@ $^ $(LDFLAGS) $(LDLIBS)

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

.PHONY: clean distclean re
