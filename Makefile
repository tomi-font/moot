NAME := excquis

SRCDIR := src
OBJDIR := .obj

SRC := $(shell find $(SRCDIR) -name *.cc)
OBJ := $(SRC:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

CXXFLAGS := -std=c++1z -Iinclude -MMD -Wall -Wextra

ifeq ($(OS),Windows_NT)
	LDFLAGS := -LC:\MinGW\bin
endif
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o$@ $^

-include $(OBJ:.o=.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CXX) $(CXXFLAGS) -c -o$@ $<

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
