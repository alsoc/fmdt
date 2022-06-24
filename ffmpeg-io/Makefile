INCLUDES=-Iinclude
CFLAGS_LIBS=
LD_LIBS=-Llib -lffmpeg-io


LIBSRC=cmd.c common.c probe.c reader.c player.c writer.c formatter.c
TESTSRC=main.c

LIBOBJS=$(foreach file,$(LIBSRC),obj/$(file).o)
TESTOBJS=$(foreach file,$(TESTSRC),obj/$(file).o)
LIBDEPS=$(foreach file,$(LIBSRC),deps/$(file).d)
TESTDEPS=$(foreach file,$(TESTSRC),deps/$(file).d)

lib/libffmpeg-io.a: $(LIBOBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

exe/test: $(TESTOBJS) lib/libffmpeg-io.a
	@mkdir -p $(dir $@)
	$(CC) -std=c99 -pthread -Wall -Wextra -Wpedantic -g $(OPTFLAGS) $(TESTOBJS) -o $@ $(LD_LIBS) $(LDFLAGS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@ deps/$*)
	$(CC) -std=c99 -Wall -Wextra -Wpedantic -g -fasm -D_GNU_SOURCE $(OPTFLAGS) $(CFLAGS) $(CFLAGS_LIBS) $(INCLUDES) -MT $@ -MMD -MP -MF deps/$*.c.Td -c $< -o $@
	@mv deps/$*.c.Td deps/$*.c.d && touch $@

clean:
	rm -rf obj/ deps/ exe/ lib/

.PHONY: clean

deps/%.d: ;
.PRECIOUS: include/%
.PRECIOUS: src/%
.PRECIOUS: deps/%

-include $(DEPS)

