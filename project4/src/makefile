# OSX compiler
# CC = clang++

# Dwarf compiler
CC = g++ -std=c++11 -O3

CXX = $(CC)

# OSX include paths (for MacPorts)
CFLAGS = -I/opt/local/include -I../include

# OSX include paths (for homebrew, probably)
CFLAGS = -I/usr/local/include -I../include

# Dwarf include paths
 #CFLAGS = -I../include # opencv includes are in /usr/include
 #CXXFLAGS = $(CFLAGS)

# OSX Library paths (if you use MacPorts)
LDFLAGS = -L/opt/local/lib

#OSX Library paths (if you use homebrew, probably)
LDFLAGS = -L/usr/local/lib

# Dwarf Library paths
#LDFLAGS = -L/usr/lib/x86_64-linux-gnu # opencv libraries are here

# opencv libraries
LDLIBS = -lopencv_core -lopencv_highgui -lopencv_video -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -lopencv_calib3d


BINDIR = ../bin

vid: vidDisplay.o OR_pipeline.o database.o
	$(CC) $^ -o $(BINDIR)/$@ $(LDFLAGS) $(LDLIBS)	

img: imgDisplay.o OR_pipeline.o database.o
	$(CC) $^ -o $(BINDIR)/$@ $(LDFLAGS) $(LDLIBS)

classImg: classifyImages.o OR_pipeline.o database.o
	$(CC) $^ -o $(BINDIR)/$@ $(LDFLAGS) $(LDLIBS)

confusion: computeConfusionMatrix.o OR_pipeline.o database.o
	$(CC) $^ -o $(BINDIR)/$@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o *~ 
