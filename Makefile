CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncursesw -lpthread

SRCS = main.c common.c args.c tui.c server.c client.c key_exchange.c crypto.c send.c send_file.c send_msg.c outgoing.c recv.c recv_file.c recv_msg.c
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
outgoing.o: outgoing.c outgoing.h common.h crypto.h tui.h
send.o: send.c send.h common.h
send_file.o: send_file.c send_file.h common.h crypto.h tui.h
send_msg.o: send_msg.c send_msg.h common.h crypto.h tui.h
recv.o: recv.c recv.h common.h crypto.h tui.h
recv_file.o: recv_file.c recv_file.h common.h crypto.h tui.h
recv_msg.o: recv_msg.c recv_msg.h common.h crypto.h tui.h
main.o: main.c common.h args.h tui.h server.h client.h key_exchange.h crypto.h outgoing.h recv.h recv_file.h recv_msg.h