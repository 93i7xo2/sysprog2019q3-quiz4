TEST_DATA_SIZE := 100 1000 2000
TEST_DATA := input.dat
CFLAGS := -O0 -Wall -Werror -g

# Control the build verbosity
ifeq ("$(VERBOSE)","1")
    Q :=
    VECHO = @true
else
    Q := @
    VECHO = @printf
endif

.PHONY: all clean

OBJS := \
	merge-original.o \
	merge-kaeteyaruyo.o \
	merge-optimize.o \
	merge-circular.o
BINS := $(OBJS:%.o=%)

all: ${BINS}

%: %.O
	echo $<
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) $(LDFLAGS)  -o $@ $< -lm

%.o: %.c
	echo $<
	$(VECHO) "  CC\t$@\n"
	$(Q)$(CC) -o $@ $(CFLAGS) -c $<

test:
	@echo "Test all."
	@dd if=/dev/urandom of=${TEST_DATA} bs=40 count=1 2>/dev/null; \
	for bin in ${BINS}; do \
		echo -n "$$bin:"; \
		./$$bin ${TEST_DATA} 10; \
		echo -n "sec \n"; \
	done;

perf:
	@for size in ${TEST_DATA_SIZE}; do \
		echo -n "Test $$size random numbers."; \
		dd if=/dev/urandom of=${TEST_DATA} bs=$$(($$size*4)) count=1 2>/dev/null; \
		for i in $$(seq 10); do \
			./merge-original $(TEST_DATA) $$size | awk -v size="$$size" '{print size","$$0}' 1>>merge-original.csv; \
			./merge-kaeteyaruyo $(TEST_DATA) $$size |  awk -v size="$$size" '{print size","$$0}' 1>>merge-kaeteyaruyo.csv; \
			./merge-optimize $(TEST_DATA) $$size |  awk -v size="$$size" '{print size","$$0}' 1>>merge-optimize.csv; \
			./merge-circular $(TEST_DATA) $$size |  awk -v size="$$size" '{print size","$$0}' 1>>merge-circular.csv; \
			echo -n "."; \
		done; \
		echo -n "Done!\n"; \
	done;

	@# Caculate average
	@$(RM) merge.txt
	@for i in merge-original merge-kaeteyaruyo merge-optimize merge-circular; do \
		echo "\"$$i\"" >> merge.txt;\
		for datasize in ${TEST_DATA_SIZE}; do \
			awk -F, -v pattern="$$datasize" '$$0 ~ pattern {print $$2}' $$i.csv | \
			awk '{ total += $$1; count++ } END { print total/count }' | \
			awk -v datasize="$$datasize" '{print datasize","$$0}' >> merge.txt; \
		done; \
		echo -n "\n\n" >> merge.txt; \
	done;

plot:
	@# Generate plot
	@gnuplot plot.gp


clean:
	$(RM) $(BINS) $(OBJS) $(TEST_DATA)
	$(RM) merge.txt \
	merge-original.csv \
	merge-kaeteyaruyo.csv \
	merge-optimize.csv \
	merge-circular.csv
	$(RM) output.png
