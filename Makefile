PRODUCT_NAME       = cs1372
SOURCES            = main.c myLib.c text.c levels.c game.c sound.c images/ScreenStart.c images/Controls.c images/PapyrusStory1.c images/PapyrusStory2.c images/PapyrusStory3.c images/PauseScreen.c images/Level1.c images/SpritesBT.c images/Papyrus.c images/GameOver.c images/TheEnd.c sound/Snd_Start.c sound/Snd_Level1.c sound/Snd_Knife.c sound/powerupsfx.c sound/Snd_GameOver.c sound/Snd_Scorpio.c sound/Snd_TheEnd.c sound/Snd_Sword.c

DKPATH             = C:/devkitARM/bin
CCPATH             = C:/cygwin/bin
VBASIM             = C:/vba/VisualBoyAdvance.exe
FIND               = find
COPY               = cp -r

ELF_NAME           = $(PRODUCT_NAME).elf
ROM_NAME           = $(PRODUCT_NAME).gba
BIN_NAME           = $(PRODUCT_NAME)

ifeq ($(CONF),Debug)
DEBUG              = yes
endif

ifndef DEBUG

MODEL              = -mthumb-interwork -marm -mlong-calls
SPECS              = -specs=gba.specs 

AS                 = $(DKPATH)/arm-eabi-as
ASFLAGS            = -mthumb-interwork

CC                 = $(DKPATH)/arm-eabi-gcc
CFLAGS             = $(MODEL) -O2 -Wall -pedantic -Wextra -D_ROM=$(ROM_NAME) -D_VBA=$(VBASIM)
CC_WRAP            = $(CCPATH)/gcc
CFLAGS_WRAP        = -O3 -Wall -pedantic -D_ROM='"$(ROM_NAME)"' -D_VBA='"$(VBASIM)"'

LD                 = $(DKPATH)/arm-eabi-gcc
LDFLAGS            = $(SPECS) $(MODEL) -lm

OBJCOPY            = $(DKPATH)/arm-eabi-objcopy
OBJCOPYFLAGS       = -O binary

GBAFIX             = $(DKPATH)/gbafix

RM                 = rm -f

OBJECTS = $(filter-out gba_wrapper%,$(SOURCES:.c=.o))

all : build $(BIN_NAME)

run : build
	$(VBASIM) $(ROM_NAME)

build : UNZIP $(ROM_NAME)

$(BIN_NAME) : gba_wrapper.c
	$(CC_WRAP) $(CFLAGS_WRAP) -o $@ $^

$(ROM_NAME) : $(ELF_NAME)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(ELF_NAME) $(ROM_NAME)
	$(GBAFIX) $(ROM_NAME)

$(ELF_NAME) : $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJECTS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

else

CC                 = $(CCPATH)/gcc
CFLAGS             = -D_DEBUG -Wall -pedantic -Wextra -ggdb

all : build

run : build
	./$(BIN_NAME)

clean :

build : UNZIP $(BIN_NAME)

$(BIN_NAME) : $(SOURCES)
	$(CC) -o $(BIN_NAME) $(CFLAGS) $(SOURCES)

endif

UNZIP :
	-@$(FIND) . -iname "*.zip" -exec unzip -n {} \; -exec echo "This project must be rebuilt" \; -exec rm {} \;

clean:
	$(RM) $(ROM_NAME)
	$(RM) $(ELF_NAME)
	$(RM) $(BIN_NAME)
	$(RM) *.o

