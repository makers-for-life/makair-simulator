
# Specify compiler
CXX=g++ 

# Compilation options
CXXFLAGS=-w -pedantic  -pthread

# Link editing options
LDFLAGS=

# Names of the executables to be generated
EXEC=simulator

# Path of source files
SRC_DIR=../srcs/

# Path of header files
HEADER_DIR=../inludes/

# Path of object files
OBJ_DIR=../build/

# List of project source files ('wildcard' allows to explore the entire folder in a variable)
SRC= $(wildcard $(SRC_DIR)*.cpp)

# List of objects files generated thanks to the list of sources files ('SRC' variable)
OBJ= $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SRC))

all: $(EXEC)

simulator: $(OBJ)
		$(CXX) -o $@ $^ $(LDFLAGS) $(CXXFLAGS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
		$(CXX) -o $@ -c $< $(CXXFLAGS)

# .PHONY is a particular target thats allows to systematically rebuild its dependencies
.PHONY: clean mrproper

# Delete all intermediate files
clean:
		rm -rf $(OBJ_DIR)*.o

# Delete all that can be regenerated and allows a complete project rebuid 
mrproper: clean
		rm -rf $(EXEC)
