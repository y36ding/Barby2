# Makefile for demo Software
#
# To run the makefile, type "make"
#
# To learn more about makefiles, see the info under uwinfo:
#	 uwinfo / facilities / computers / supported software / unix
# or read the man pages
#


### User Modifiable Macros ###
#
# CC represents the compiler, LINK represents the linker
# CFLAGS represents the compiler command line parameters
# 	-c means compile only, i.e. create an object file
#	-g means add symbol table information for debugging purposes
# LNFLAGS represents the linker command line parameters
#
CC = g++
CFLAGS = -c -g -Wno-write-strings
LINK = g++
LNFLAGS = -g -Wno-write-strings


### Program list ###
##  list all of the executables to be created with the makefile


# default target: create all of the programs listed
#
OBJS = crt iRTX keyboard

all: crt iRTX keyboard clean

#   programs
#   these commands link the object files and libraries into executables 

iRTX: debug.o cleanup.o globals.o rtx_init.o iRTX.o userAPI.o MsgEnvQueue.o kernal.o iProcs.o kernal.o timerQ.o procPQ.o processQ.o procABC.o clock_proc.o debug_stack.o
	@echo linking iRTX.o file
	$(LINK) $(LNFLAGS) debug.o cleanup.o globals.o rtx_init.o iRTX.o userAPI.o MsgEnvQueue.o iProcs.o kernal.o timerQ.o procPQ.o processQ.o procABC.o clock_proc.o debug_stack.o -o iRTX

crt:crt.o
	@echo linking crt.o file
	$(LINK) $(LNFLAGS) crt.o -o crt
	
keyboard:keyboard.o
	@echo linking keyboard.o file
	$(LINK) $(LNFLAGS) keyboard.o -o keyboard


#compile the source code into object files
debug_stack.o: debug_stack.c
	@echo building debug_stack.c
	$(CC) $(CFLAGS) debug_stack.c

rtx_init.o: rtx_init.c
	@echo building rtx_init.c
	$(CC) $(CFLAGS) rtx_init.c

globals.o: globals.c
	@echo building globals.c
	$(CC) $(CFLAGS) globals.c

cleanup.o: cleanup.c
	@echo building cleanup.c
	$(CC) $(CFLAGS) cleanup.c

debug.o: debug.c debug_stack.c
	@echo building debug.c
	$(CC) $(CFLAGS) debug.c

crt.o: crt.c 
	@echo building crt.c
	$(CC) $(CFLAGS) crt.c
	
MsgEnvQueue.o:	MsgEnvQueue.c 
	@echo building MsgEnvQueue
	$(CC) $(CFLAGS) MsgEnvQueue.c
	    
kernal.o: kernal.c 
	@echo building kernal
	$(CC) $(CFLAGS) kernal.c

keyboard.o: keyboard.c
	@echo building keyboard.c
	$(CC) $(CFLAGS) keyboard.c 

iRTX.o: iRTX.c
	@echo building iRTX.c
	$(CC) $(CFLAGS) iRTX.c
	
userAPI.o: userAPI.c 
	@echo building userAPI.c
	$(CC) $(CFLAGS) userAPI.c
	
processP.o: processP.c
	@echo building processP.c
	$(CC) $(CFLAGS) processP.c
		
iProcs.o: iProcs.c
	@echo building iProcs.c
	$(CC) $(CFLAGS) iProcs.c
	
procPQ.o: procPQ.c
	@echo building procPQ.c
	$(CC) $(CFLAGS) procPQ.c
	
processQ.o: processQ.c
	@echo building processQ.c
	$(CC) $(CFLAGS) processQ.c
	
timerQ.o: timerQ.c
	@echo building timerQ.c
	$(CC) $(CFLAGS) timerQ.c
	
procABC.o: procABC.c
	@echo building procABC.c
	$(CC) $(CFLAGS) procABC.c
	
clock_proc.o: clock_proc.c
	@echo building clock_proc.c
	$(CC) $(CFLAGS) clock_proc.c

clean:
	@echo Cleaning object files
	rm *.o
	rm keyboardBuffer
	rm crtBuffer
	#rm ${OBJS}
