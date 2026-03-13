CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncursesw -lpthread

SRCS = main.c common.c args.c tui.c server.c client.c key_exchange.c crypto.c send.c send_file.c send_msg.c outgoing.c recv.c recv_file.c recv_msg.c
OBJDIR = objects
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)
TARGET = saffron

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# Dependencies
$(OBJDIR)/common.o: 		common.h
$(OBJDIR)/args.o: 			common.h args.h
$(OBJDIR)/tui.o: 			common.h tui.h
$(OBJDIR)/server.o: 		common.h server.h
$(OBJDIR)/client.o: 		common.h client.h
$(OBJDIR)/crypto.o: 		common.h crypto.h
$(OBJDIR)/outgoing.o: 		common.h outgoing.h crypto.h tui.h send_file.h send_msg.h
$(OBJDIR)/send.o: 			common.h send.h
$(OBJDIR)/send_file.o: 		common.h send.h send_file.h crypto.h tui.h
$(OBJDIR)/send_msg.o: 		common.h send.h send_msg.h crypto.h tui.h
$(OBJDIR)/recv.o: 			common.h recv.h
$(OBJDIR)/recv_file.o: 		common.h recv.h recv_file.h crypto.h tui.h
$(OBJDIR)/recv_msg.o: 		common.h recv.h recv_msg.h crypto.h tui.h
$(OBJDIR)/key_exchange.o: 	common.h recv.h send.h key_exchange.h crypto.h
$(OBJDIR)/main.o: 			common.h args.h tui.h server.h client.h \
                  			key_exchange.h crypto.h send.h send_file.h send_msg.h \
                  			outgoing.h recv.h recv_file.h recv_msg.h