# Irfansha Shaik 06.07.2019 (Swansea)
# Copyright 2019 Oliver Kullmann

# Started from the copy:
# Url: https://github.com/OKullmann/oklibrary/commits/master/Satisfiability/Transformers/Generators/Queens/SimpleBacktracking/Makefile
# Git Id: b713a0074311ad3f4f311dc9ed04381c2e877297


SHELL = /bin/bash
.SUFFIXES :

.PHONY : all programs optimised debug test regtest apptest clean cleanall


git_id = $(shell git rev-parse HEAD)
git_macro = -DGITID="\"$(git_id)\""
machine_name = $(shell hostname)
machine_macro = -DMACHINE="\"$(machine_name)\""
bogomips_value = $(shell grep -m 1 "bogomips" /proc/cpuinfo | cut -f2 | cut -d' ' -f2)
bogomips_macro = -DBOGOMIPS=$(bogomips_value)

special_macros = $(machine_macro) $(bogomips_macro) $(git_macro)


CXX = g++

LStandard_options = --std=c++17 -pedantic
Warning_options = -fmax-errors=5 -Wall -Wextra
Optimisation_core = -Ofast -DNDEBUG -march=native
# "plus" means extensions, "minus" means restriction:
Optimisation_plus = -fwhole-program -funsafe-loop-optimizations
Optimisation_minus =
Optimisation_options = $(Optimisation_core) $(Optimisation_plus) $(Optimisation_minus)
numerics_plus =
numerics_minus =
numerics_options = $(numerics_plus) $(numerics_minus)
Debug_options = -g -D_GLIBCXX_DEBUG
# Options to reproduce compilation:
repr_macro = -DOPTIMISATION="\"$(LStandard_options) $(Optimisation_options) $(target_specifics)\""
reprdebug_macro = -DOPTIMISATION="\"$(LStandard_options) $(Warning_options) $(Debug_options) $(target_specifics)\""


include_options =

opt_progs = qcount
debug_progs = $(addsuffix _debug, $(opt_progs))


all : programs test

programs : optimised debug

optimised : $(opt_progs)

$(opt_progs) :
	$(CXX) $(LStandard_options) $(Warning_options) $(include_options) $(Optimisation_options) $(numerics_options) $(special_macros) $(repr_macro) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< -o $@

debug : $(debug_progs)

$(debug_progs) :
	$(CXX) $(LStandard_options) $(Warning_options) $(include_options) $(Debug_options) $(special_macros) $(reprdebug_macro) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $< -o $@


SETN=-DNN=16

qcount : CXXFLAGS+=$(SETN)
qcount : target_specifics=$(SETN)
qcount : NQueens.cpp

qcount_debug : CXXFLAGS += $(SETN)
qcount_debug : target_specifics=$(SETN)
qcount_debug : NQueens.cpp

test : regtest apptest

regtest :

apptest :


clean :
	- rm $(debug_progs)

cleanall : clean
	- rm $(opt_progs)

