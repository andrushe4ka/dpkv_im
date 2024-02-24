CC=C:/sdcc/bin/sdcc.exe
CFLAGS=-mstm8 
INCLUDEPATH=../stm8s-sdcc/inc/
SRCPATH=../stm8s-sdcc/src/
DEFINES=STM8S103 -D SDCC
SOURCE=main
OUTPUT_DIR=build/
.PHONY: all
all: $(OUTPUT_DIR)main.ihx
$(OUTPUT_DIR)main.ihx: main.c $(OUTPUT_DIR)stm8s_clk.rel $(OUTPUT_DIR)stm8s_uart1.rel $(OUTPUT_DIR)stm8s_gpio.rel $(OUTPUT_DIR)stm8s_tim2.rel $(OUTPUT_DIR)stm8s_tim4.rel $(OUTPUT_DIR)stm8s_adc1.rel $(OUTPUT_DIR)debug_lib.rel
	$(CC) $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) $(SOURCE).c $(OUTPUT_DIR)stm8s_clk.rel $(OUTPUT_DIR)stm8s_uart1.rel $(OUTPUT_DIR)stm8s_gpio.rel $(OUTPUT_DIR)stm8s_tim2.rel $(OUTPUT_DIR)stm8s_tim4.rel $(OUTPUT_DIR)stm8s_adc1.rel $(OUTPUT_DIR)debug_lib.rel
$(OUTPUT_DIR)stm8s_clk.rel: $(SRCPATH)stm8s_clk.c $(INCLUDEPATH)stm8s_conf.h
	$(CC) -c $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) $(SRCPATH)stm8s_clk.c
$(OUTPUT_DIR)stm8s_uart1.rel: $(SRCPATH)stm8s_uart1.c $(INCLUDEPATH)stm8s_conf.h
	$(CC) -c $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) $(SRCPATH)stm8s_uart1.c
$(OUTPUT_DIR)stm8s_gpio.rel: $(SRCPATH)stm8s_gpio.c $(INCLUDEPATH)stm8s_conf.h
	$(CC) -c $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) $(SRCPATH)stm8s_gpio.c
$(OUTPUT_DIR)stm8s_tim2.rel: $(SRCPATH)stm8s_tim2.c $(INCLUDEPATH)stm8s_conf.h
	$(CC) -c $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) $(SRCPATH)stm8s_tim2.c
$(OUTPUT_DIR)stm8s_tim4.rel: $(SRCPATH)stm8s_tim4.c $(INCLUDEPATH)stm8s_conf.h
	$(CC) -c $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) $(SRCPATH)stm8s_tim4.c
$(OUTPUT_DIR)stm8s_adc1.rel: $(SRCPATH)stm8s_adc1.c $(INCLUDEPATH)stm8s_conf.h
	$(CC) -c $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) $(SRCPATH)stm8s_adc1.c
$(OUTPUT_DIR)debug_lib.rel: debug_lib.c
	$(CC) -c $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) debug_lib.c
	
#	$(CC) -E $(CFLAGS) -I $(INCLUDEPATH) -D $(DEFINES) -o $(OUTPUT_DIR) $(SOURCE).c > $(OUTPUT_DIR)preprocessed.c