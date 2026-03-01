CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses -lpthread

SRCS = main.c common.c args.c tui.c server.c client.c key_exchange.c crypto.c outgoing.c msg_recv.c file_recv.c
OBJS = $(SRCS:.c=.o)
TARGET = saffron

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# Dependencies
common.o: common.c common.h
args.o: args.c args.h common.h
tui.o: tui.c tui.h common.h
server.o: server.c server.h common.h
client.o: client.c client.h common.h
key_exchange.o: key_exchange.c key_exchange.h common.h
crypto.o: crypto.c crypto.h common.h
outgoing.o: outgoing.c outgoing.h common.h crypto.h
msg_recv.o: msg_recv.c msg_recv.h common.h crypto.h
file_recv.o: file_recv.c file_recv.h common.h crypto.h
main.o: main.c common.h args.h tui.h server.h client.h key_exchange.h crypto.h outgoing.h msg_recv.h file_recv.h