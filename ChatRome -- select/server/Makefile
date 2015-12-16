MYNAME = makefile
CC = gcc

objects = server.o register.o login.o list.o config.o chat.o

server: $(objects)
	cc -g -o server $(objects) -lsqlite3 -lpthread

server.o: server.c config.h
	cc -c server.c 

register.o: register.c config.h
	cc -c register.c

login.o: login.c config.h
	cc -c login.c

list.o: list.c config.h
	cc -c list.c

config.o: config.c config.h
	cc -c config.c

chat.o: chat.c config.h
	cc -c chat.c
#比较稳健的clean做法，表示clean是一个伪目标
.PHONY: clean

#前面-的意思是：也许某些文件出现问题，忽略，继续执行
clean:
	-rm server $(objects) 

