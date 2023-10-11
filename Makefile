OUTFILE = beaglepod
OUTDIR = $(HOME)/cmpt433/public/myApps
SOURCE = source-files/
SOURCES := $(wildcard $(SOURCE)/*.c)

PUBDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

LFLAGS = -L$(HOME)/cmpt433/public/pulse-audio_lib_BBB -L$(HOME)/cmpt433/public/asound_lib_BBB 

# List of object files derived from source files
OBJECTS = $(patsubst $(SOURCE)/%.c, $(OUTDIR)/%.o, $(SOURCES))

# Default target
all: $(OUTDIR)/$(OUTFILE)

# Rule to build the final executable
$(OUTDIR)/$(OUTFILE): $(OBJECTS)
	$(CC_C) $(CFLAGS) $^ -o $@ $(LFLAGS) -lbluetooth -lasound -pthread

# Rule to build .o files from .c files
$(OUTDIR)/%.o: $(SOURCE)/%.c | $(OUTDIR)
	$(CC_C) $(CFLAGS) -c $< -o $@

# Rule to create the output directory
$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	rm $(OUTDIR)/$(OUTFILE)
	rm $(OBJECTS)
