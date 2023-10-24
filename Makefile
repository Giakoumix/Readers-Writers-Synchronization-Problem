
INCLUDE = include
PROGRAMS = programs 
TEXT = files

CFLAGS = -pthread -I $(INCLUDE)
CC = gcc

EXEC = programs/main
OBJS = programs/main.o programs/process_txt.o programs/shared_memory.o
FILES = main_text.txt 
ARGS = main_text.txt 100 10

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(EXEC)

run: $(EXEC) 
	./$(EXEC) main_text.txt 100 10 4

clean: $(OBJS) $(EXEC)
	rm -f $(OBJS) $(EXEC) files/* !files/main_text
