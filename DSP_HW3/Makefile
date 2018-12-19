# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
# SRIPATH ?= /data/DSP_HW3/103_2/srilm-1.5.10
SRIPATH ?= ../srilm_macosx
MACHINE_TYPE ?= macosx
#LM ?= bigram.lm

CXX = /usr/bin/g++
CXXFLAGS = -O3 -I $(SRIPATH)/include -w
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp utils.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
.PHONY: all clean map run

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

run:
	for i in $(shell seq 1 10); \
	do \
		echo "Running $$i.txt"; \
		./mydisambig testdata/$$i.txt > result2/$$i.txt; \
	done;

#map: \
	@#TODO How to map? \
	@echo "Mapping!" \
	@#./mapping $(FROM) $(TO) \
	@#matlab mapping.m $(FROM) $(TO) \
	@#python3 mapping.py $(FROM) $(TO) \
	@#sh mapping.sh $(FROM) $(TO) \
	@#perl mapping.pl Big5-ZhuYin.map ZhuYin-Big5.map
clean:
	$(RM) $(OBJ) $(TARGET)
