main: main.c read_writer.c mem_manager.c elf_reader.c linker.c tool.h
	gcc main.c read_writer.c mem_manager.c elf_reader.c linker.c -o main
