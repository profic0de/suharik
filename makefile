SRC = $(filter-out $(shell find . -wholename './ignore/*.c'),$(shell find . -name '*.c'))
OBJ := $(SRC:%=./build/%.o)
ARGS = -I. -Isrc -Wall -Wno-deprecated-declarations -Wno-nonnull -Wno-sequence-point -lcurl -lm 
OUT = out

PROGRAM_ARGS = src.sk

run: $(OUT)
	@echo --------------------
	@./$(OUT) $(PROGRAM_ARGS)

$(OUT): $(OBJ)
	gcc -g -fsanitize=address $(ARGS) $(OBJ) -o $(OUT)

./build/%.c.o: %.c
	@mkdir -p $(dir $@)
	gcc -g -fsanitize=address $(ARGS) -c $< -o $@

.PHONY: run, prod, clean, git

git:
	@git add .
	@git commit -m "$$(date +%d/%-m/%y)"
	@git push

prod:
	gcc $(ARGS) -o $(OUT) $(SRC)
	@echo --------------------
	@./$(OUT) $(PROGRAM_ARGS)

clean:
	@rm -rf $(OUT)
	@rm -rf ./build/
