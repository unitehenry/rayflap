TARGET_EXEC := rayflap

BUILD_DIR := ./build
SRC_DIR := ./src

$(shell mkdir -p $(BUILD_DIR))

$(BUILD_DIR)/$(TARGET_EXEC):
	gcc $(SRC_DIR)/main.c -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
