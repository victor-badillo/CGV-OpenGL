# Define el compilador
CC = gcc

# Define las banderas de compilación
CFLAGS = -Wall -g

# Define las bibliotecas necesarias
LIBS = -lGL -lGLU -lglut -lm

# Define el nombre del ejecutable y el archivo fuente
TARGET = noria
SOURCES = noria.c
OBJECTS = $(SOURCES:.c=.o)

# Regla por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

# Regla para compilar archivos .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar archivos generados
clean:
	rm -f $(TARGET) $(OBJECTS)
