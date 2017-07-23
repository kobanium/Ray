TARGET = ray
CXX = g++
OPTIMIZE = -O3
CPP11 = -std=c++11
WARNING = -Wall
DEBUG = #-g
CFLAGS = ${OPTIMIZE} ${WARNING} ${CPP11} ${DEBUG}
LIBS = -lm -pthread
RM = rm

SRCS=${shell ls src/*.cpp}
HEDS=${shell ls src/*.h}
OBJS=${SRCS:.cpp=.o}

.SUFFIXES:.cpp .o .h

.PHONY: all
all : ${TARGET}

${TARGET} : ${OBJS}
	${CXX} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

.cpp.o:
	${CXX} ${CFLAGS} -c $< -o $@

.PHONY: clean

clean:
	${RM} -f ${TARGET} src/*~ src/*.o *~

Command.o: src/Command.cpp src/Command.h src/DynamicKomi.h src/GoBoard.h \
 src/Pattern.h src/UctSearch.h src/ZobristHash.h src/Message.h
Command.o: src/Command.h
DynamicKomi.o: src/DynamicKomi.cpp src/DynamicKomi.h src/GoBoard.h \
 src/Pattern.h src/UctSearch.h src/ZobristHash.h src/Message.h
DynamicKomi.o: src/DynamicKomi.h src/GoBoard.h src/Pattern.h \
 src/UctSearch.h src/ZobristHash.h
GoBoard.o: src/GoBoard.cpp src/GoBoard.h src/Pattern.h src/Semeai.h \
 src/UctRating.h src/PatternHash.h src/ZobristHash.h
GoBoard.o: src/GoBoard.h src/Pattern.h
Gtp.o: src/Gtp.cpp src/DynamicKomi.h src/GoBoard.h src/Pattern.h \
 src/UctSearch.h src/ZobristHash.h src/Gtp.h src/Nakade.h src/UctRating.h \
 src/PatternHash.h src/Message.h src/Point.h src/Rating.h \
 src/Simulation.h
Gtp.o: src/Gtp.h
Ladder.o: src/Ladder.cpp src/Message.h src/GoBoard.h src/Pattern.h \
 src/UctSearch.h src/ZobristHash.h src/Ladder.h src/SearchBoard.h \
 src/Point.h
Ladder.o: src/Ladder.h src/GoBoard.h src/Pattern.h
Message.o: src/Message.cpp src/Message.h src/GoBoard.h src/Pattern.h \
 src/UctSearch.h src/ZobristHash.h src/Point.h
Message.o: src/Message.h src/GoBoard.h src/Pattern.h src/UctSearch.h \
 src/ZobristHash.h
Nakade.o: src/Nakade.cpp src/Message.h src/GoBoard.h src/Pattern.h \
 src/UctSearch.h src/ZobristHash.h src/Nakade.h src/Point.h
Nakade.o: src/Nakade.h src/GoBoard.h src/Pattern.h
Pattern.o: src/Pattern.cpp src/GoBoard.h src/Pattern.h
Pattern.o: src/Pattern.h
PatternHash.o: src/PatternHash.cpp src/PatternHash.h src/GoBoard.h \
 src/Pattern.h
PatternHash.o: src/PatternHash.h src/GoBoard.h src/Pattern.h
Point.o: src/Point.cpp src/GoBoard.h src/Pattern.h src/Point.h
Point.o: src/Point.h
Rating.o: src/Rating.cpp src/Message.h src/GoBoard.h src/Pattern.h \
 src/UctSearch.h src/ZobristHash.h src/Nakade.h src/Point.h src/Rating.h \
 src/UctRating.h src/PatternHash.h src/Semeai.h src/Utility.h
Rating.o: src/Rating.h src/GoBoard.h src/Pattern.h src/UctRating.h \
 src/PatternHash.h
RayMain.o: src/RayMain.cpp src/Command.h src/GoBoard.h src/Pattern.h \
 src/Gtp.h src/PatternHash.h src/Rating.h src/UctRating.h src/Semeai.h \
 src/UctSearch.h src/ZobristHash.h
SearchBoard.o: src/SearchBoard.cpp src/SearchBoard.h src/GoBoard.h \
 src/Pattern.h
SearchBoard.o: src/SearchBoard.h src/GoBoard.h src/Pattern.h
Seki.o: src/Seki.cpp src/GoBoard.h src/Pattern.h src/Point.h src/Seki.h \
 src/Semeai.h
Seki.o: src/Seki.h src/GoBoard.h src/Pattern.h
Semeai.o: src/Semeai.cpp src/GoBoard.h src/Pattern.h src/Message.h \
 src/UctSearch.h src/ZobristHash.h src/Point.h src/Semeai.h \
 src/UctRating.h src/PatternHash.h
Semeai.o: src/Semeai.h src/GoBoard.h src/Pattern.h
Simulation.o: src/Simulation.cpp src/GoBoard.h src/Pattern.h \
 src/Message.h src/UctSearch.h src/ZobristHash.h src/Point.h src/Rating.h \
 src/UctRating.h src/PatternHash.h src/Simulation.h
Simulation.o: src/Simulation.h src/GoBoard.h src/Pattern.h
UctRating.o: src/UctRating.cpp src/Ladder.h src/GoBoard.h src/Pattern.h \
 src/Message.h src/UctSearch.h src/ZobristHash.h src/Nakade.h \
 src/PatternHash.h src/Point.h src/Semeai.h src/Utility.h src/UctRating.h
UctRating.o: src/UctRating.h src/GoBoard.h src/Pattern.h \
 src/PatternHash.h
UctSearch.o: src/UctSearch.cpp src/DynamicKomi.h src/GoBoard.h \
 src/Pattern.h src/UctSearch.h src/ZobristHash.h src/Ladder.h \
 src/Message.h src/PatternHash.h src/Seki.h src/Simulation.h \
 src/UctRating.h src/Utility.h
UctSearch.o: src/UctSearch.h src/GoBoard.h src/Pattern.h \
 src/ZobristHash.h
Utility.o: src/Utility.cpp src/Utility.h
Utility.o: src/Utility.h
ZobristHash.o: src/ZobristHash.cpp src/Nakade.h src/GoBoard.h \
 src/Pattern.h src/ZobristHash.h
ZobristHash.o: src/ZobristHash.h src/GoBoard.h src/Pattern.h

