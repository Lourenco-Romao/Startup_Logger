include ../.env

CC = gcc

CFLAGS = -Wall -Wextra
CFLAGS += -DAPI_BASE_URL=\"$(API_BASE_URL)\"
CFLAGS += -DAPI_KEY=\"${API_KEY}\"

CURL_FLAGS = $(shell pkg-config --cflags --libs libcurl)

TARGET = build/startup-logger
SOURCES = src/main.c src/system.c src/api.c src/files.c

$(TARGET): $(SOURCES)
	mkdir -p build
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(CURL_FLAGS)

clean:
	rm -f $(TARGET)