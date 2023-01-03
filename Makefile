CFLAGS = -Wall -Wextra -ansi -pedantic
LDFLAGS = $(CFLAGS) -lm

SRC_DIR = src
BUILD_DIR = build
BIN = spamid.exe


all: clean $(BUILD_DIR) $(BIN)

$(BIN): $(BUILD_DIR)/spamid.o $(BUILD_DIR)/classifier.o $(BUILD_DIR)/hashtable.o $(BUILD_DIR)/vector.o $(BUILD_DIR)/arrays.o $(BUILD_DIR)/primes.o $(BUILD_DIR)/utils.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/spamid.o: $(SRC_DIR)/spamid.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/classifier.o: $(SRC_DIR)/classifier.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/hashtable.o: $(SRC_DIR)/structures/hashtable.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/vector.o: $(SRC_DIR)/structures/vector.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/arrays.o: $(SRC_DIR)/utilities/arrays.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/primes.o: $(SRC_DIR)/utilities/primes.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/utils.o: $(SRC_DIR)/utilities/utils.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(BIN)
