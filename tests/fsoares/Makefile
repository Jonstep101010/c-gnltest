TIMEOUT		= 10
CFLAGS		= -g -Wall -Wextra -Werror
STRICT		=
UTILS 		= utils/malloc_mock.c utils/utils.c file_utils.c
TIMEOUT_ARG	= -D TIMEOUT=$(TIMEOUT)

# SDK headers shipping w/Xcode 14.1 (22/11/01)
# and later deprecate some functions used here
CFLAGS		+= -Wno-deprecated-declarations

UNAME = $(shell uname -s)
ifeq ($(UNAME), Linux)
    CFLAGS = -g -Wall -Wextra -Werror
endif

TEST_TARGETS_DIR = ../test_targets

mandatoryone:
	-@clang $(CFLAGS) -D BUFFER_SIZE=1 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) -ldl -o test1.out $(TEST_TARGETS_DIR)/libget_next_line_1.a && ./test1.out < lines_around_10.txt
	-@clang $(CFLAGS) -D STRICT_MEM -D BUFFER_SIZE=1 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) -ldl -o test1.out $(TEST_TARGETS_DIR)/libget_next_line_1.a && ./test1.out < lines_around_10.txt
mandatoryten:
	-@clang $(CFLAGS) -D BUFFER_SIZE=10 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) -ldl -o test2.out $(TEST_TARGETS_DIR)/libget_next_line_10.a && ./test2.out < lines_around_10.txt
	-@clang $(CFLAGS) -D STRICT_MEM -D BUFFER_SIZE=10 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) -ldl -o test2.out $(TEST_TARGETS_DIR)/libget_next_line_10.a && ./test2.out < lines_around_10.txt
mandatorymillion:
	-@clang $(CFLAGS) -D BUFFER_SIZE=1000000 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) $(TEST_FILES) -ldl -o test3.out $(TEST_TARGETS_DIR)/libget_next_line_1000000.a && ./test3.out < lines_around_10.txt
	-@clang $(CFLAGS) -D STRICT_MEM -D BUFFER_SIZE=1000000 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) -ldl -o test2.out $(TEST_TARGETS_DIR)/libget_next_line_1000000.a && ./test2.out < lines_around_10.txt

mandatory: mandatoryone mandatoryten mandatorymillion

bonus:
	@tput setaf 3 && /bin/echo BONUS!
	-@clang $(CFLAGS) -D BUFFER_SIZE=1 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) $(BONUS_FILES) -ldl -o test4.out && ./test4.out < lines_around_10.txt && rm -f test.out
	-@clang $(CFLAGS) -D BUFFER_SIZE=10 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) $(BONUS_FILES) -ldl -o test5.out && ./test5.out < lines_around_10.txt && rm -f test.out
	-@clang $(CFLAGS) -D BUFFER_SIZE=1000000 $(TIMEOUT_ARG) $(STRICT) tester.c $(UTILS) $(BONUS_FILES) -ldl -o test6.out && ./test6.out < lines_around_10.txt && rm -f test.out

	-@clang $(CFLAGS) -D BUFFER_SIZE=1 $(TIMEOUT_ARG) $(STRICT) bonus.c $(UTILS) $(BONUS_FILES) -ldl -o test7.out && ./test7.out && rm -f test.out
	-@clang $(CFLAGS) -D BUFFER_SIZE=10 $(TIMEOUT_ARG) $(STRICT) bonus.c $(UTILS) $(BONUS_FILES) -ldl -o test8.out && ./test8.out && rm -f test.out
	-@clang $(CFLAGS) -D BUFFER_SIZE=1000000 $(TIMEOUT_ARG) $(STRICT) bonus.c $(UTILS) $(BONUS_FILES) -ldl -o test9.out && ./test9.out && rm -f test.out
