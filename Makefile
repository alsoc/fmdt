CC=gcc
CFLAGS=-Wall -std=c99 -O3 -march=native -D_POSIX_C_SOURCE=200809L
# CFLAGS=-Wall -std=c99 -D_POSIX_C_SOURCE=200809L
CXXFLAGS=-Wall -std=c++17 -O3
EXEC_NAME=ballon
INCLUDES=-I include/ -I ffmpeg-io/include
LIBS=-Llib/ffmpeg-io/lib -lm -lffmpeg-io
DEFINES=

BUILD_DIR=build/
SRC_DIR=src/

C_FILES = $(filter-out $(SRC_DIR)VideoTrack.c, $(wildcard $(SRC_DIR)*.c))
OBJ_FILES = $(addprefix $(BUILD_DIR),$(notdir $(C_FILES:.c=.o))) $(addprefix $(BUILD_DIR),$(notdir $(CXX_FILES:.cpp=.o)))
OBJ_FILES_TRACK = $(filter-out $(BUILD_DIR)Main.o , $(addprefix $(BUILD_DIR),$(notdir $(C_FILES:.c=.o))) $(addprefix $(BUILD_DIR),$(notdir $(CXX_FILES:.cpp=.o))))

all: obj_dir $(EXEC_NAME) tracking

clean:
	rm -f $(EXEC_NAME) $(OBJ_FILES) tracking img/img*_out*.png $(BUILD_DIR)VideoTrack.o
#	@$(MAKE) -C lib/ffmpeg-io clean

$(EXEC_NAME): $(OBJ_FILES) lib/ffmpeg-io/lib/libffmpeg-io.a
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

$(BUILD_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES)  -o $@ -c $<

tracking : $(OBJ_FILES_TRACK) $(BUILD_DIR)VideoTrack.o lib/ffmpeg-io/lib/libffmpeg-io.a
	$(CC) -o tracking $(OBJ_FILES_TRACK) $(BUILD_DIR)VideoTrack.o $(LIBS)

VideoTrack.o : $(SRC_DIR)VideoTrack.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES)  -o $@ -c $<

test : 
	./ballon -input ../videos/C0089.MP4 -start_frame 510 -end_frame 512

full : 
	./ballon -input /dsk/l1/misc/cc3801875/videos/C0089.MP4 -start_frame 0 -end_frame 6000 -validation ./validation/C0089.txt

tau : 
	./ballon -input /dsk/l1/misc/cc3801875/Meteors_in_mkv/4 -start_frame 1 -end_frame 41 -light_max 85 -light_min 75

lib/ffmpeg-io/lib/libffmpeg-io.a:
	@$(MAKE) -C ffmpeg-io

obj_dir:
	mkdir -p $(BUILD_DIR)

.PHONY: lib/ffmpeg-io/lib/libffmpeg-io.a

zip:
	zip -r -9 meteor . -i src/*.c include/*.h Makefile
