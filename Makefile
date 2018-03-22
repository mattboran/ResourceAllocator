CXXFLAGS = -Wall -std=gnu++11 -O0 -g -I$(INCLUDE) -fmessage-length=0

OBJS =		$(SRC)ResourceAllocator.o $(SRC)Task.o $(SRC)Action.o ${SRC}OptimisticResourceManager.o  $(SRC)ResourceManager.o $(SRC)BankerResourceManager.o

SRC = 		./src/

INCLUDE = 	./include/

LIBS =

TARGET =	ResourceAllocator

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET)  $(OBJS) $(LIBS)

all:	$(TARGET) 

clean:
	rm -f $(OBJS) $(TARGET)
