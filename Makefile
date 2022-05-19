NAME := excquis

SRCDIR := src
OBJDIR := .obj

SRC := $(shell find $(SRCDIR) -name *.cc)
OBJ := $(SRC:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

CXXFLAGS := -std=c++23 -Iinclude -MMD -Wall -Wextra -Wno-switch

ifdef $(OS)
	LDFLAGS := -LC:\MinGW\bin
endif
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o$@ $^ $(LDFLAGS) $(LDLIBS)

-include $(OBJ:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o$@ $<

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
