all: falcon.o hatchery.o cmd.o lexer.o drawing.o database.o elementlist.o router.o
	g++ falcon.o hatchery.o cmd.o lexer.o drawing.o database.o elementlist.o router.o -o falcon -lfreeglut -lglu32 -lopengl32 -lwinmm -lgdi32
#-lglui

cmd.o: cmd.cpp cmd.h
	g++ -c cmd.cpp

lexer.o: lexer.cpp lexer.h
	g++ -c lexer.cpp

falcon.o: falcon.cpp falcon.h
	g++ -c falcon.cpp

hatchery.o: hatchery.cpp hatchery.h
	g++ -c hatchery.cpp

drawing.o: drawing.cpp drawing.h
	g++ -c drawing.cpp

database.o: database.cpp database.h
	g++ -c database.cpp

elementlist.o: elementlist.cpp elementlist.h
	g++ -c elementlist.cpp

router.o: router.cpp router.h
	g++ -c router.cpp

clean:
	del *.o