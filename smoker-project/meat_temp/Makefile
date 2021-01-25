FOO=$(CC)gcc
SOURCE=calc_coeff.c
SOURCE_TEMP=meat_temp.c
SOURCE_RES=get_resistance.c
CFLAGS=-Wall
TGT=calc_coeff
TGT_MEAS=temp
TGT_RES=get_resistance
BIN_FOLDER=bin
PCBIN_FOLDER=PCbin

all: $(BIN_FOLDER)/$(TGT) $(BIN_FOLDER)/$(TGT_MEAS) $(BIN_FOLDER)/$(TGT_RES)

$(BIN_FOLDER)/$(TGT): $(SOURCE)
	$(FOO) $(CFLAGS) $(SOURCE) -o $(BIN_FOLDER)/$(TGT) -lm
	gcc $(CFLAGS) $(SOURCE) -o $(PCBIN_FOLDER)/$(TGT) -lm

$(BIN_FOLDER)/$(TGT_MEAS): $(SOURCE_TEMP)
	$(FOO) $(CFLAGS) $(SOURCE_TEMP) -o $(BIN_FOLDER)/$(TGT_MEAS) -lm
	gcc $(CFLAGS) $(SOURCE_TEMP) -o $(PCBIN_FOLDER)/$(TGT_MEAS) -lm

$(BIN_FOLDER)/$(TGT_RES): $(SOURCE_RES)
	$(FOO) $(CFLAGS) $(SOURCE_RES) -o $(BIN_FOLDER)/$(TGT_RES) -lm
	gcc $(CFLAGS) $(SOURCE_RES) -o $(PCBIN_FOLDER)/$(TGT_RES) -lm

clean:
	rm -rf $(PCBIN_FOLDER)/$(TGT)
	rm -rf $(PCBIN_FOLDER)/$(TGT_MEAS)
	rm -rf $(PCBIN_FOLDER)/$(TGT_RES)
	rm -rf $(BIN_FOLDER)/$(TGT)
	rm -rf $(BIN_FOLDER)/$(TGT_MEAS)
	rm -rf $(BIN_FOLDER)/$(TGT_RES)
