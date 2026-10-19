CC ?= cc
CPPFLAGS += -Icore -Ithird_party/libpo32/core/include -Ithird_party/libpo32/core/src -DGIN_FAST_FIXED=1 -UNDEBUG
CFLAGS += -std=c99 -O2 -g -Wall -Wextra -Werror -Wno-misleading-indentation -fsanitize=address,undefined -fno-sanitize-recover=all
SOURCES = core/gin_integer.c core/gin_precision.c core/gin_prepare.c core/tonic_voice.c third_party/libpo32/core/src/po32_synth.c
.PHONY: test
test: out/functional
	./out/functional
out/functional: $(SOURCES) tests/functional.c $(wildcard core/*.h) $(wildcard third_party/libpo32/core/include/*.h) $(wildcard third_party/libpo32/core/src/*.h)
	mkdir -p out
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) tests/functional.c -lm -o $@
