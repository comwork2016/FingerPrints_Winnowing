objs=obj/SplitContents.o obj/Document.o obj/DocumentDao.o obj/SimHash.o obj/WinNowing.o obj/DocumentOperation.o obj/main.o 

CPPINCLUDE=-Iinclude
BOOSTINCLUDE=-pthread -lmongoclient -lboost_thread -lboost_filesystem -lboost_program_options -lboost_system

vpath %.o obj
vpath %.cpp src
vpath %.h include

fingerprints: $(objs)
	g++ -o $@ $^ $(BOOSTINCLUDE) -lfriso

obj/%.o:%.cpp
	g++ $(CPPINCLUDE)  -o $@ -c $^

clean:
	-rm -f obj/*.o fingerprints
