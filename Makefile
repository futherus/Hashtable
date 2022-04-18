export CXX      ?= gcc
export CXXFLAGS ?= -O2 -mavx -mavx2 -g --static-pie -std=c++14 -fmax-errors=100 -Wall -Wextra   \
    			   -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat 	   					\
    			   -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual 	   					\
    			   -Wchar-subscripts -Wconditionally-supported -Wconversion        				\
    			   -Wctor-dtor-privacy -Wempty-body -Wfloat-equal 		   						\
    			   -Wformat-nonliteral -Wformat-security -Wformat-signedness       				\
    			   -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op 	           				\
    			   -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd 	   					\
    			   -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls 				\
    			   -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192      				\
    			   -Wstrict-null-sentinel -Wstrict-overflow=2 			   						\
    			   -Wsuggest-attribute=noreturn -Wsuggest-final-methods 	   					\
    			   -Wsuggest-final-types -Wsuggest-override -Wswitch-default 	   				\
    			   -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused   				\
    			   -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix 	   					\
    			   -Wno-missing-field-initializers -Wno-narrowing 	           					\
    			   -Wno-old-style-cast -Wno-varargs -fcheck-new 		   						\
    			   -fsized-deallocation -fstack-check -fstack-protector            				\
    			   -fstrict-overflow -flto-odr-type-merging 	   		   						\
    			   -fno-omit-frame-pointer                                         				\
    			   -fPIE                                                           				\
    			#    -fsanitize=address 	                                           				\
    			#    -fsanitize=alignment                                            				\
    			#    -fsanitize=bool                                                 				\
    			#    -fsanitize=bounds                                               				\
    			#    -fsanitize=enum                                                 				\
    			#    -fsanitize=float-cast-overflow 	                           					\
    			#    -fsanitize=float-divide-by-zero 			           							\
    			#    -fsanitize=integer-divide-by-zero                               				\
    			#    -fsanitize=leak 	                                           					\
    			#    -fsanitize=nonnull-attribute                                    				\
    			#    -fsanitize=null 	                                           					\
    			#    -fsanitize=object-size                                          				\
    			#    -fsanitize=return 		                                   						\
    			#    -fsanitize=returns-nonnull-attribute                            				\
    			#    -fsanitize=shift                                                				\
    			#    -fsanitize=signed-integer-overflow                              				\
    			#    -fsanitize=undefined                                            				\
    			#    -fsanitize=unreachable                                          				\
    			#    -fsanitize=vla-bound                                            				\
    			#    -fsanitize=vptr                                                 				\
    			#    -lm -pie 					 

# not overwrite OBJDIR if recursive
export OBJDIR ?= $(CURDIR)/obj/

# for recursive compiling
all: list hashtable

#------------------------------------------------------------------------------
hashtable: | $(OBJDIR)
	@ cd src && $(MAKE)

list: | $(OBJDIR)
	@ cd include/List && $(MAKE)

$(OBJDIR):
	mkdir $(OBJDIR)
#------------------------------------------------------------------------------

BINDIR := $(CURDIR)/bin/
TARGET := $(BINDIR)test_ht
VPATH  := tests/ utils/ utils/logs

TEST        := hash_test1.cpp hash_test2.cpp hash_test3.cpp hash_test4.cpp hash_test5.cpp hash_test6.cpp hash_test7.cpp \
			   main.cpp test1.cpp crc32_test.cpp # opt_crc32_test.cpp
TESTOBJ     := $(addprefix $(OBJDIR), $(TEST:.cpp=.o))
RUN_TEST    := utils/run_tests.py
TEST_OUTDIR := test_output/

SRC := logs.cpp stats.cpp text.cpp
OBJ := $(addprefix $(OBJDIR), $(SRC:.cpp=.o) List.o Hashtable.o)

test: build_test | $(TEST_OUTDIR)
	python $(RUN_TEST) $(TEST_OUTDIR) $(addprefix $(BINDIR), $(basename $(TEST)))

build_test: list hashtable $(OBJ) $(TESTOBJ) | $(BINDIR)
	$(foreach var, $(TESTOBJ), 		  									\
		$(CXX) $(CXXFLAGS) $(OBJ) $(var) -o $(BINDIR)$(notdir $(basename $(var));) 	\
	)

$(OBJDIR)%.o : %.cpp | $(OBJDIR)
	@ echo Compiling $@
	@ $(CXX) -c $^ -o $@ $(CXXFLAGS)

$(BINDIR):
	mkdir $(BINDIR)

$(TEST_OUTDIR):
	mkdir $(TEST_OUTDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR) $(TEST_OUTDIR)