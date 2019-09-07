NAME := excquis

SRCDIR := src
OBJDIR := .obj

SRC := $(shell find $(SRCDIR) -type f)
OBJ := $(SRC:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

CXXFLAGS := -Iinclude -MMD -Wall -Wextra -Wpadded

LDLIBS := -lsfml-window -lsfml-system
ifndef RELEASE
	LDLIBS := $(addsuffix -d,$(LDLIBS))
endif

all: $(NAME)

$(NAME): $(OBJDIR) $(OBJ)
	$(CXX) $(LDLIBS) -o$@ $(wordlist 2,$(words $^),$^)

$(OBJDIR):
	mkdir $@

-include $(OBJ:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -c -o$@ $<

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
