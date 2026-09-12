CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -Iinclude -D_POSIX_C_SOURCE=200809L
SRC = src/utils.c src/menu.c src/inventory.c src/order.c src/loyalty.c src/payment.c src/storage.c src/analytics.c src/auth.c src/ui.c
OBJ = $(SRC:.c=.o)
MAIN_OBJ = src/main.o
TEST_OBJ = tests/test_runner.o
TARGET = coffeeshop
TEST_TARGET = test_runner

all: $(TARGET)

$(TARGET): $(OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o tests/*.o $(TARGET) $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all test clean run
