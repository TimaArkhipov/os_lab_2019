#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int*  PID_id;
int pnum;
void kill_all(int sig) //функция для "убийства" всех дочерних процессов
{
    for (int i = 0 ;i < pnum; i++)   //для всех процессов
    {
        kill(PID_id[i], SIGKILL);   //передирает id процессов и каждому посылает сигнал SIGKILL
    }
    printf("TIMEOUT\n");
}

int main(int argc, char **argv) 
{
  int timeout = -1;
  int seed = -1; //Инициализируем число -1
  int array_size = -1; // -||-
  pnum = -1; // -||-
  bool with_files = false; // Инициализация булевого числа 0

  while (true) { //Начало цикла
    int current_optind/*текущий optind*/ = optind ? optind : 1; // Тернарная инициализация переменной
                                              //  Если oprind == true, то optind, иначе 1

//Переменная optind — это индекс следующего обрабатываемого элемента argv. Система инициализирует это значение 1. 
//Вызывающий может сбросить его в 1 для перезапуска сканирования того же argv, или при сканировании нового вектора аргументов.



    /*struct option
    {
    const char *name; //Длинное название опции

    int has_arg; // может быть: no_argument (или 0), если параметр не требует значения; 
    // required_argument (или 1), если параметр требует значения; 
    // optional_argument (или 2), если параметр может иметь необязательное значение.

    int *flag; //Если равен NULL(0 может тоже подойдёт), то возвращает val, 
                иначе возвращает 0, а переменная на которую указывает flag заполняется значением val

    int val; //значение, которое возвращается или загружается в переменную, на которую указывает flag.
    }*/
printf("It's ok01!\n");
    static struct option options[] = {{"seed", required_argument, 0, 0}, //Объявление статической структуры option
                                      {"array_size", required_argument, 0, 0}, // размер массива
                                      {"pnum", required_argument, 0, 0}, // наверное количество процессов?
                                      {"by_files", no_argument, 0, 'f'}, // 
                                      {"timeout", optional_argument, 0 , 0}, //lab4
                                      {0, 0, 0, 0}}; //Последняя строка всегда нули
                                    

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);
    printf("It's ok02!\n");
    // Аргументы(int argc, char *argv[], const char *optsting, const struct option *longotps, int *longindex)
    // argc, argv - понятно; *optsting краткие названия опций; *longotps - список(массив) длинных опций, доступных для этой функции;
    // Если longindex не равен NULL, то он указывает на переменную, содержащую индекс длинного параметра в соответствии с longopts.
    // Если опция принимает аргумент, то указатель на него помещается в переменную optarg
    
    if (c == -1) break; // значит функция getopt_long приняла все опции(не приняла никакой новой)
    switch (c) { //обычный switch
      case 0:
        switch (option_index) { //switch в switch'e
          case 0:
            seed = atoi(optarg);
            // your code here
            if (seed <= 0)
            {
		    printf("seed should be positive!\n\n");
		    return 1;
            }
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            if (array_size <= 0)
            {
		    printf("array_size should be positive!\n\n");
		    return 1;
            }
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            if (pnum <= 0)
            {
		    printf("pnum should be positive!\n\n");
		    return 1;
            }
            // error handling
            break;
          case 3:
            with_files = true;
            //printf("It's ok0");
            FILE* MyFile = fopen("file.txt", "w");
            fclose(MyFile);
            break;
          case 4:
            timeout = atoi(optarg);
            if (timeout <= 0)
            {
		    printf("timeout should be positive!\n\n");
		    return 1;
            }
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        // printf("It's ok01");
        FILE* MyFile = fopen("file.txt", "w");
        fclose(MyFile);
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c); // %o восьмиричное число без знака
    }
  }
    printf("It's ok1!\n");
  if (optind < argc) {
    printf("Has at least one no option argument\n");// Имеет по крайней мере один аргумент без опции
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  } // Вывод инструкции, если seed, array_size, pnum не менялись
    printf("It's ok2!\n");

  int *array = malloc(sizeof(int) * array_size); // Создание, выделение памяти под динамический массив
  GenerateArray(array, array_size, seed); // Заполнение массива случайными числами
  int active_child_processes = 0; // Количество активных дочерних процессов

  struct timeval start_time;

/*
struct timeval {
long    tv_sec;       //   секунды 
long    tv_usec;      //  микросекунды 
};
*/

  gettimeofday(&start_time, NULL); //Запишет время от 01.01.1970 в структуру start_time
  int *array_of_pipes_read = malloc(sizeof(int) * pnum);

  printf("It's ok2!\n");

  int pipefd[2]; // статический массив с элементами 1 и 0

  // pipefd[0] указывает на конец канала для чтения;
  // pipefd[1] указывает на конец канала для записи

  pipe(pipefd);  
  int number_segment = array_size / pnum;
  struct MinMax MyMinMax;

  for (int i = 0; i < pnum; i++) //Какой-то цикл с количеством итераций = pnum
  {
    pid_t child_pid = fork(); // PID(Process IDentifier), PPID(Parent Process IDentifier)
                             // pid_t - это целый тип, размерность которого зависит от конкретной системы.
    // fork() работает так: 
    // Возвращает -1 для ошибок, 0 для нового процесса и идентификатор процесса нового процесса для старого процесса.

    if (child_pid >= 0) 
    {
      // successful fork (успешная вилка)
      active_child_processes += 1; 
      if (child_pid == 0) // дочерний процесс
      {
        // child process (дочерний процесс)
        // parallel somehow (как-то параллельно)
        if (i != pnum - 1)
        	MyMinMax = GetMinMax(array, i * number_segment, (i+1) * number_segment);
	    else
		    MyMinMax = GetMinMax(array, i * number_segment, array_size);
            
        //printf("Min: %d\nMax: %d\n",MyMinMax.min,MyMinMax.max);
        //printf("Promejutok (%d,%d)\n",i * number_segment, (i+1) * number_segment);
        if (with_files) 
        {
          // use files here (используем файлы здесь)
          FILE* MyFile = fopen("file.txt", "a");
	      fwrite(&MyMinMax, sizeof(struct MinMax), 1, MyFile);
	      fclose(MyFile);
          //printf("It's ok1!\n");
        } else 
        {
          // use pipe here (используем "трубы" здесь)
          close(pipefd[0]);
          write(pipefd[1], &MyMinMax, sizeof(struct MinMax));
          close(pipefd[1]);
        }
        
        return 0;
      }
      else // процесс-родитель
      { 
        array_of_pipes_read[i] = pipefd[0]; // сохраняем данные о трубе для данного процесса
      }

    } 
    else 
    {
      printf("Fork failed!\n");
      return 1;
    }
  }

printf("It's ok3!\n");

  printf("Timeout now: %d\n" ,timeout);   //выводит время перед началом таймаута
  if (timeout > 0)
  {
    PID_id = array_of_pipes_read;     //записывает id созданных дочерних процессов
    alarm(timeout);                   //отсчитывает время для таймаута
    signal(SIGALRM,kill_all);         //посылает сигнал всем дочерним процессам
    sleep(1);                         //ждет на случай если на передачу сигнала нужно время
  }
printf("It's ok4!\n");
  while (active_child_processes > 0) 
  {
    // your code here
    //close(pipefd[1]);
    wait(NULL); // дожидаемся пока процесс закончит работу и деактивируем его
    active_child_processes -= 1; 
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) 
  {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) 
    {
      // read from files
      FILE* MyFile = fopen("file.txt", "r");
      // printf("It's ok2!\n");
      fseek(MyFile, i * sizeof(struct MinMax), SEEK_SET);
      // printf("It's ok3!\n");
      // fseek задаёт смещение в файле MyFile, на i*sizeof(...) байт;  
      // Позиция указателя перед смещением задаётся выражениями
      //SEEK_SET	Начало файла
      //SEEK_CUR	Текущее положение файла
      //SEEK_END	Конец файла
      //fscanf(MyFile,"%d",&MyMinMax.min);
      //printf("It's ok3!\n");
      //fscanf(MyFile,"%d",&MyMinMax.max);
      fread(&MyMinMax, sizeof(struct MinMax), 1, MyFile);
      //printf("It's ok4!\n");
      // Функция fread считывает массив размером — 
      // count элементов, каждый из которых имеет 
      // размер size байт, из потока, и сохраняет его в блоке памяти, на который указывает ptrvoid.

      fclose(MyFile);
    } 
    else 
    {
      // read from pipes
      read(pipefd[0], &MyMinMax, sizeof(struct MinMax));
    }
    min = MyMinMax.min;
    max = MyMinMax.max;

    if (min < min_max.min) 
        min_max.min = min;
    if (max > min_max.max) 
        min_max.max = max;
  }

  struct timeval finish_time; 
  gettimeofday(&finish_time, NULL); // Конец отсчёта времени

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  
  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  
  return 0;
}

// make -f makefile clean
// make -f makefile parallel_min_max
// Здесь -f позволяет явно задать файл правил
// ./parallel_min_max --seed 8 --array_size 10 --pnum 5 --by_files 
