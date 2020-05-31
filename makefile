
CC = gcc
CFLAGS = -Wextra -Wall -std=gnu99 -Iinclude -Wno-unused-parameter -Wno-unused-variable -Wno-duplicate-decl-specifier

MQTT_C_SOURCES = mqtt/mqtt.c mqtt/mqtt_pal.c
MQTT_C_APP = bin/kontroler bin/senzorski_pi bin/aktuatorski_pi 
BINDIR = bin

all: $(BINDIR)  $(MQTT_C_APP)
bin/kontroler: src/kontroler.c $(MQTT_C_SOURCES)
	$(CC) $(CFLAGS) $^ -pthread -o $@


bin/senzorski_%: src/senzorski_%.c $(MQTT_C_SOURCES)
	$(CC) $(CFLAGS) $^ -pthread -o $@

bin/aktuatorski_pi: src/aktuatorski_pi.c $(MQTT_C_SOURCES)
	$(CC) $(CFLAGS) $^ -pthread -o $@


$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(BINDIR)
