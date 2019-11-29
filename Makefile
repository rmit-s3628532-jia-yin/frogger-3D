SRC = game.c scene.c utilities.c camera.c frog.c image_helper.c image_DXT.c etc1_utils.c SOIL2.c
TARGET = game

$(TARGET): $(SRC)
	gcc -Wno-deprecated-declarations -o $(TARGET) $(SRC) -framework GLUT -framework OpenGL -framework Carbon