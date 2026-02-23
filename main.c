#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN_DATE 11 // максимальная длина поля "дата"
#define MAX_LEN_TIME 9 // максимальная длина поля "время"
#define MAX_LEN_TYPE 6 // максимальная длина поля "тип" (info, warn, error)
#define EVENT_MSG_MAX_LEN 256 // максимальная длин "сообщение"
#define EVENT_MAX_SIZE 16 //начальная размерность массива
#define MENU_CHOICE_MAX 9 //количество действий в программе

typedef struct { // Структура даты
    unsigned int year;
    unsigned short mon;
    unsigned short day;
} date_t;

typedef struct { // Структура времени
    unsigned short hour;
    unsigned short min;
    unsigned short sec;
} ttime_t;

typedef enum { // Enum для типа события
    ET_INFO = 0,
    ET_WARN,
    ET_ERROR,
} event_type_t;

typedef struct { // Структура события
    unsigned int id;
    date_t date;
    ttime_t time;
    event_type_t type;
    char message[EVENT_MSG_MAX_LEN];
} event_t;


void print_menu(void); //Прототип функции вывода списка меню
event_t* init_events(unsigned int max_size); //Прототип функции инициализации массива
void flush_input(); //Прототип функции очистки буфера ввода после функции scanf()

//Работа с добавлением события
int scan_data_for_event(event_t *event, int count_id);
int validate_date(int year, unsigned short mon, unsigned short day);
int validate_time(unsigned short hour, unsigned short min, unsigned short sec);
int validate_type(char *type);
int validate_message(char *message);
event_t* realloc_events(event_t* events, int event_count, unsigned int *max_event_count);
int add_event(event_t* events, event_t* new_event, int event_count, int count_id);


//Работа с выводом всех слбытий
int show_all_events(int event_count, event_t* events); //показ всех событий
void print_event(event_t *event); //вынес printf полей в отдельную функцию
void event_type_to_str(event_type_t type, char* in); //перевод типа событий для отображения

//Работа с сортировкой событий
int sort_events(int event_count, event_t* events); //сортировка событий по дате и времени
void moving_elements_array(int j, event_t* events); //перемещение эелементов в массиве

//Работа с выводом записей по типу
void show_by_type(int event_count, event_t* events); //показ событий по типу

//Работа с поиском событий по id
void find_by_id(int event_count, event_t* events); //поиск событий по id

//Работа с удалением событий по id
int delete_by_id(int event_count, int input_id_delete, event_t* events); //удаление события по id

//Работа с сохраненеием событий в файл
void save_to_file(unsigned int event_count, event_t* events);

//Работа с загрузкой событий из файла
void load_from_file(unsigned int *event_count, unsigned int *count_id, event_t* events);


int main (void)
{
    unsigned int max_event_count = EVENT_MAX_SIZE;
    unsigned int event_count = 0; //количество элементов в массиве
    unsigned int count_id = 0; //счетчик id
    event_t *events;

    events = init_events(max_event_count);
    if (events == NULL) {
        return 1;
    }


    unsigned short choice = 0;
    //int temp = 0;

    do {
        print_menu();
        scanf("%hu", &choice);
        flush_input();

        switch (choice) {
            case 1:
                event_t new_event;

                //Сканирую и заполняю поля события в new_event
                scan_data_for_event(&new_event, count_id);

                //Проверка нуждается ли массив в расширении
                events = realloc_events(events, event_count, &max_event_count);
                if (events == NULL) {
                    printf("Ошибка расширения массива!");
                    break;
                }

                //Добавление события в массив
                if (add_event(events, &new_event, event_count, count_id) == 0) {
                    event_count++;
                    count_id++;
                    printf("Событие успешно добавлено!\n");
                } else {
                    printf("ОШибка при добавлении события");
                }
                break;
            case 2:
                show_all_events(event_count, events);
                break;
            case 3:
                if (sort_events(event_count, events) == 0) {
                    printf("СОБЫТИЯ ОТСОРТИРОВАНЫ ПО ДАТЕ И ВРЕМЕНИ\n");
                }
                break;
            case 4:
                show_by_type(event_count, events);
                break;
            case 5:
                find_by_id(event_count, events);
                break;
            case 6:
                if (event_count == 0) {
                    printf("УДАЛЯТЬ НЕЧЕГО!!!\n");
                    break;
                }

                int input_id_delete;
                printf("Введите id события: ");
                scanf("%d", &input_id_delete);
                
                if(delete_by_id(event_count, input_id_delete, events) == 0) {
                    printf("Событие с id = %d успешно удалено\n", input_id_delete);
                    event_count--;
                } else {
                    printf("Не удалось удалить событие с id = %d\n", input_id_delete);
                }
                break;
            case 7:
                save_to_file(event_count, events);
                break;
            case 8:
                load_from_file(&event_count, &count_id, events);
                break;
            case 0:
                break;
            default:
                printf("Неверный ввод! Пожалуйста, введите число.\n");
                break;
        } 

    } while (choice != 0);

    free(events);
    return 0;
}




//============================================ РАБОТА С ДОБАВЛЕНИЕМ СОБЫТИЯ =========================================================================================================

//Функция сканирования данных для заполнения полей структуры
int scan_data_for_event(event_t *event, int count_id) {
    //Заполнение id
    event->id = count_id + 1;

    //Заполнение даты
    do {
        printf("\nВведите дату (в формате ГОД МЕСЯЦ ДЕНЬ через пробел): ");
        scanf("%d %hu %hu", &event->date.year, &event->date.mon, &event->date.day);
        flush_input();
    } while (validate_date(event->date.year, event->date.mon, event->date.day) == 1);

    //Заполнение времени
    do {
        printf("\nВведите время (в формате ЧАС МИНУТА СЕКУНДА через пробел): ");
        scanf("%hu %hu %hu", &event->time.hour, &event->time.min, &event->time.sec);
        flush_input();
    } while (validate_time(event->time.hour, event->time.min, event->time.sec) == 1);

    //Заполнение типа
    char type_event[16];
    do {
        printf("\nВведите тип события (info, warn, error): ");
        scanf("%15s", type_event);
        event->type = validate_type(type_event);
        flush_input();
    } while (validate_type(type_event) == 100);

    //Заполнение сообщения
    do {
        printf("\nВведите сообщение события длиной не более 255 символов: ");
        scanf("%255s", event->message);
        flush_input();
    } while (validate_message(event->message) == 1);

    return 0;
}

// Функция валидации даты

int validate_date(int year, unsigned short mon, unsigned short day) {
    if (year < 0 || mon < 0 || day < 0) {
        printf("Элементы даты не могут быть отрицательными!\n");
        return 1;
    }
    if (year > 2026 || mon > 12 || day > 31) {
        printf("Проверьте коректность ввода даты!\n");
        return 1;
    } 
    return 0;
}

// Функция валидации времени
int validate_time(unsigned short hour, unsigned short min, unsigned short sec) {
    if (hour < 0 || min < 00 || sec < 0) {
        printf("Элементы времени не могут быть отрицательными!\n");
        return 1;
    }

    if (hour > 23 || min > 60 || sec > 60) {
        printf("Проверьте коректность ввода времени!\n");
        return 1;
    }

    return 0;
}

// Функция валидации типа
int validate_type(char *type) {
    if (strcmp(type, "info") == 0) {
        return 0;
    }
 
    if (strcmp(type, "warn") == 0) {
        return 1;
    }

    if (strcmp(type, "error") == 0) {
        return 2;
    }

    printf("Тип события введен некоректно!\n");
    return 100;
}

// Функция валидации сообщения
int validate_message(char *message) {
    if (strcmp(message, "") == 0) {
        printf("Сообщение не может быть пустым");
        return 1;
    }
    return 0;
}

// Функция расширения массива
event_t* realloc_events(event_t* events, int event_count, unsigned int *max_event_count) {
    if (event_count < *max_event_count) {
        return events;
    }

    if (event_count == *max_event_count) {
        *max_event_count *= 2;
        event_t* temp = (event_t*) realloc(events, *max_event_count * sizeof(event_t));
        if (temp == NULL) {
            return NULL;
        }
        events = temp;

        return events;
    }

    return NULL;
}

// Функция добавления структуры в массив
int add_event(event_t* events, event_t* new_event, int event_count, int count_id) {
    events[event_count].id = count_id + 1;
    events[event_count].date = new_event->date;
    events[event_count].time = new_event->time;
    events[event_count].type = new_event->type;
    strncpy(events[event_count].message, new_event->message, EVENT_MSG_MAX_LEN);

    return 0;
}


//============================================ РАБОТА С ПОКАЗОМ СОБЫТИЯ =========================================================================================================

// Функция показа всех событий
int show_all_events(int event_count, event_t* events) {
    if (event_count == 0) {
        printf("ВЫВОДИТЬ НЕЧЕГО!!!\n");
        return 0;
    } else {
        printf("\n = = = Вывод всех (%d) событий = = = \n", event_count);
        for (int i = 0; i < event_count; i++) {
            print_event(&events[i]);
        }
        printf("На данный момент это все события\n");

        return 0;
    }
}

// Функция для печати полей структуры в терминале
void print_event(event_t *event) {
    char tmp[16] = {0};
    event_type_to_str(event->type, tmp);

    printf("ID: %u     ", event->id);
    printf("Дата: %d-%hu-%hu     ", event->date.year, event->date.mon, event->date.day);
    printf("Время: %hu:%hu:%hu     ", event->time.hour, event->time.min, event->time.sec);
    printf("Тип события: %s     ", tmp);
    printf("Сообщение: %s     \n", event->message);
    printf(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
}

// Функция рашифровки типа события
void event_type_to_str(event_type_t type, char* in) {
    switch (type) {
        case ET_INFO:
            strcpy(in, "info");
            break;
        case ET_WARN:
            strcpy(in, "warn");
            break;
        case ET_ERROR:
            strcpy(in, "error");
            break;
    }
}


//============================================ РАБОТА С СОРТИРОВКОЙ СОБЫТИЙ =========================================================================================================

// функция сортировки событи по дате и времени
int sort_events(int event_count, event_t* events) {
    if (event_count == 0) {
        printf("ОРТИРОВАТЬ НЕЧЕГО!!!\n");
        return 0;
    }
    if (event_count < 2) {
        printf("СОБЫТИЙ МЕНЬше 2, НЕВОЗМОШНО ОСУЩЕСТВИТЬ СОРТИРОВКУ!!!\n");
        return 0;
    }

    for (int i = 0; i < event_count; i++) {
        for (int j = 0; j < event_count - i; j++) {
            if (events[j].date.year < events[j + 1].date.year) {
                moving_elements_array(j, events);
            } else if (events[j].date.mon < events[j + 1].date.mon) {
                moving_elements_array(j, events);
            } else if (events[j].date.day < events[j + 1].date.day) {
                moving_elements_array(j, events);
            }

            if (events[j].date.year == events[j + 1].date.year && events[j].date.mon == events[j + 1].date.mon && events[j].date.day == events[j + 1].date.day) {
                if (events[j].time.hour < events[j + 1].time.hour) {
                    moving_elements_array(j, events);
                } else if (events[j].time.min < events[j + 1].time.min) {
                    moving_elements_array(j, events);
                } 
                else if (events[j].time.sec < events[j + 1].time.sec) {
                    moving_elements_array(j, events);
                }
            }
        }
    } 
    
    return 0;

}

// Функция которая меняет местами элементы массива
void moving_elements_array(int j, event_t* events) {
    event_t temp = events[j];
    events[j] = events[j + 1];
    events[j + 1] = temp;
}


//============================================ РАБОТА С ВЫВОДОМ ЗАПИСЕЙ СОБЫТИЙ ПО ТИПУ =========================================================================================================

// Функция вывода записей по типу
void show_by_type(int event_count, event_t* events) {
    if (event_count == 0) {
        printf("ВЫВОДИТЬ НЕЧЕГО!!!\n");
        return;
    }

    char input_type[16];
    int index_type = 0;

    do {
        printf("Введите тип события: ");
        scanf("%15s", input_type);
        index_type = validate_type(input_type);
    } while (index_type == 100);

    int count_event_type = 0;
    for (int i = 0; i < event_count; i++) {
        if (events[i].type == index_type) {
            print_event(&events[i]);
            count_event_type++;
        }
    }
    printf("На данный момент это все(%d) события типа %s\n", count_event_type, input_type);
}


//============================================ РАБОТА С ПОИСКОМ СОБЫТИЙ ПО ID =========================================================================================================

// Функция поиска событий по ID
void find_by_id(int event_count, event_t* events) {
    if (event_count == 0) {
        printf("ВЫВОДИТЬ НЕЧЕГО!!!\n");
        return;
    }

    int input_id = 0;
    printf("Введите id события: ");
    scanf("%d", &input_id);
    for (int i = 0; i < event_count; i++) {
        if (events[i].id == input_id) {
            print_event(&events[i]);
            break;
        }
    }
}


//============================================ РАБОТА С УДАЛЕНИЕМ СОБЫТИЙ ПО ID =========================================================================================================

// Функция удаления события по ID
int delete_by_id(int event_count, int input_id_delete, event_t* events) {
    for (int i = 0; i < event_count; i++) {
        if (events[i].id == input_id_delete) {
            printf("Удаление события");
            print_event(&events[i]);

            for (; i < event_count; i++) {
                // id
                events[i].id = events[i + 1].id;

                // Дата
                events[i].date.year = events[i + 1].date.year;
                events[i].date.mon = events[i + 1].date.mon;
                events[i].date.day = events[i + 1].date.day;

                // Время
                events[i].time.hour = events[i + 1].time.hour;
                events[i].time.min = events[i + 1].time.min;
                events[i].time.sec = events[i + 1].time.sec;
                
                // Тип
                events[i].type = events[i + 1].type;

                // Сообщение
                strncpy(events[i].message, events[i + 1].message, EVENT_MSG_MAX_LEN - 1);
                events[i].message[EVENT_MSG_MAX_LEN - 1] = '\0';
            }
            return 0;
        }
    }
    return 1; // если нужное событие не будет найдено
}


//============================================ РАБОТА С СОХРАНЕНИЕМ СОБЫТИЙ В БИНАРНЫЙ ФАЙЛ =========================================================================================================

// Функция сохрания событий в бинарный файл
void save_to_file(unsigned int event_count, event_t* events) {
    if (event_count == 0) {
        printf("СОХРАНЯТЬ НЕЧЕГО!!!\n");
        return;
    }

    FILE *file = fopen("events.dat", "wb");  // fopen - функция открытия файла, wb означает что файл либо будет создан, либо будет полностью переписан 
    if (file == NULL) {
        printf("Файл не создался или же ошибка с перезаписью созданного файла\n");
        return;
    }

    // Сохраняю количество элементов структуры, чтобы знать сколько их хранится в бинарном файле
    fwrite(&event_count, sizeof(int), 1, file); // fwrite(переменная с числом объектов, размер типа данных переменной, сколько блоков, указатель на структуру FILE*)

    // Записываю уже элементы структуры в бинврный файл
    fwrite(events, sizeof(event_t), event_count, file);
    
    fclose(file);
    printf("События(%d) сохранены в файл 'events.dat'\n", event_count);
}


//============================================ РАБОТА С ЗАГРУЗКОЙ СОБЫТИЙ ИЗ БИНАРНОГО ФАЙЛА =========================================================================================================

// Функция загрузки событий из бинарного файла
void load_from_file(unsigned int *event_count, unsigned int *count_id, event_t* events) {
    FILE *file = fopen("events.dat", "rb");

    if (file == NULL) {
        printf("Файл не найден!\n");
        return;
    }

    int event_count_in_bin;
    if (fread(&event_count_in_bin, sizeof(int), 1, file) != 1) { // Читаю сколько структур записано в бинарном файле
        printf("Ошибка чтения количества событий\n");
        fclose(file);
        return;
    }

    if (event_count_in_bin <= 0) {
        printf("Файл пуст или поврежден\n");
        fclose(file);
        return;
    }

    if (events != NULL) {
        free(events);
        events = NULL;
    }

    events = (event_t*) malloc(event_count_in_bin * sizeof(event_t));

    if (events == NULL) {
        printf("Ошибка выделения памяти!\n");
        fclose(file);
        return;
    }

    int items_read = fread(events, sizeof(event_t), event_count_in_bin, file);

    fclose(file);

    if (items_read != event_count_in_bin) {
        printf("Ошибка чтения данных! Прочитано %d из %d событий\n", items_read, event_count_in_bin);
        free(events);
        events = NULL;
        *event_count = 0;
        return;
    }

    int max_id = 1;
    for (int i = 0; i < *event_count; ++i) {
        if (events[i].id > max_id) {
            max_id = events[i].id;
        }
    }

    *count_id = max_id;
    *event_count = event_count_in_bin;

    printf("События(%d) загружены из файла\n", *event_count);
}


//============================================ ВТОРОСТЕПЕННЫЕ ФУНКЦИИ =========================================================================================================

// Функция печати списка меню в терминале
void print_menu(void) {
    printf("\n");
    printf("1. Добавить новое событие.\n");
    printf("2. Вывести все события.\n");
    printf("3. Отсортировать события по дате и времени.\n");
    printf("4. Вывести записи отфильтрованными по типу.\n");
    printf("5. Найти событие по ID.\n");
    printf("6. Удалить событие по ID.\n");
    printf("7. Сохранить события в файл.\n");
    printf("8. Загрузить события из файла.\n");
    printf("0. Выход.\n\n");
    printf("Выберите действие: ");
}
        
// Функция инициализации массива
event_t* init_events(unsigned int max_size) {
    event_t *events = (event_t*) malloc(max_size * sizeof(event_t)); //выделяю в куче массив на 16 эолементов со значением структуры
    if (events == NULL) {
        printf("Ошибка выделения памяти!\n");
        return NULL;
    }
    return events;
}

// Функция очистки буфера после считывания функцией scanf()
void flush_input() {
    int c;
    do {
        c = getchar(); // Чтение символа
    } while (c != '\n' && c != EOF); 
}


