CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -luv -lm

BUILD_DIR = build

# TARGET = $(BUILD_DIR)/output

# Create build directory
$(shell mkdir -p $(BUILD_DIR))

all:
	$(CC) $(CFLAGS) libuv.c -o $(BUILD_DIR)/libuv $(LDFLAGS)
01:
	$(CC) $(CFLAGS) 01_system_info.c -o $(BUILD_DIR)/01_system_info $(LDFLAGS)
02: 
	$(CC) $(CFLAGS) 02_idle.c -o $(BUILD_DIR)/02_idle $(LDFLAGS)
03: 
	$(CC) $(CFLAGS) 03_fs_readsync.c -o $(BUILD_DIR)/03_fs_readsync $(LDFLAGS)
04: 
	$(CC) $(CFLAGS) 04_fs_readasync.c -o $(BUILD_DIR)/04_fs_readasync $(LDFLAGS)
05: 
	$(CC) $(CFLAGS) 05_fs_readasync_context.c -o $(BUILD_DIR)/05_fs_readasync_context $(LDFLAGS)
06: 
	$(CC) $(CFLAGS) 06_fs_allasync.c -o $(BUILD_DIR)/06_fs_allasync $(LDFLAGS)
07: 
	$(CC) $(CFLAGS) 07_tcp_echo_server.c -o $(BUILD_DIR)/07_tcp_echo_server $(LDFLAGS)