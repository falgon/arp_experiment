.PHONY: all

all: arps

FLAGS := -Wall -Wextra -pedantic -std=c++14 -O3
INCLUDE_PATH := -I ./includes -I ./includes/SrookCppLibraries
GXX := g++

OUTS = ./src/dump_arp.o\
	   ./src/sendarp.o

arps: $(OUTS)
	$(RM) -r dst
	mkdir dst
	mv ./src/dump_arp.o ./dst/dump_arp
	mv ./src/sendarp.o ./dst/sendarp

$(OUTS): %.o: %.cpp
	$(GXX) $(FLAGS) $(INCLUDE_PATH) $< -o $@

clean:
	$(RM) -r dst
