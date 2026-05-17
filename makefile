SRC = $(filter-out $(shell find . -wholename './ignore/*.c'),$(shell find . -name '*.c'))
OBJ := $(SRC:%=./build/%.o)
ARGS = -O3 -std=c23 -I. -Isrc -Wall -Wno-misleading-indentation -Wno-deprecated-declarations -Wno-nonnull -Wno-sequence-point
OUT = out

PROGRAM_ARGS = src.t

run: $(OUT)
	@echo --------------------
	@./$(OUT) $(PROGRAM_ARGS)

$(OUT): $(OBJ)
	gcc -g -fsanitize=address $(ARGS) $(OBJ) -o $(OUT)

./build/%.c.o: %.c
	@mkdir -p $(dir $@)
	gcc -g -fsanitize=address $(ARGS) -c $< -o $@

.PHONY: run, prod, clean, git, make

git:
	@git add .
	@git commit -m "$$(date +%d/%-m/%y)"
	@git push

make: # Used to be called 'test' but I renamed it to 'make' because of muscle memory
	gcc $(ARGS) -o ./ignore/test ./ignore/test.c
	@echo --------------------
	@./ignore/test $(PROGRAM_ARGS)

prod:
	gcc $(ARGS) -o $(OUT) $(SRC)
	@echo --------------------
	@./$(OUT) $(PROGRAM_ARGS)

clean:
	@rm -rf $(OUT)
	@rm -rf ./build/
