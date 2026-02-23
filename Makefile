# Простой Makefile для Windows
all: utf8 build run

utf8:
	chcp 65001

build:
	@echo "Компиляция..."
	gcc -Wall -g main.c -o program.exe

run:
	@echo "Запуск программы..."
	@echo "====================="
	program.exe

clean:
	@echo "Очистка..."
	del program.exe

help:
	@echo "Доступные команды:"
	@echo "make - собрать и запустить программу"
	@echo "make build - только собрать программу"
	@echo "make run - только запустить программу"
	@echo "make clean - удалить скомпилированные файлы"
	@echo "make help - показать справку помощи"



