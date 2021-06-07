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

pid_t* pid_child;
int pnum;
static void kill_all(int sig) //функция для "убийства" всех дочерних процессов
{
    for (int i = 0 ; i < pnum; i++) 
        kill(pid_child[i], SIGKILL);   
    printf("TIMEOUT\n");
}

int main(int argc, char **argv) 
{
  int timeout = -1;
  int seed = -1; 
  int array_size = -1; 
  pnum = -1; 
  bool with_files = false; 

  while (true) 
  {
    int current_optind = optind ? optind : 1;  //  Если oprind == true, то optind, иначе 1

    //Переменная optind — это индекс следующего обрабатываемого элемента argv. Система инициализирует это значение 1. 
    //Вызывающий может сбросить его в 1 для перезапуска сканирования того же argv, или при сканировании нового вектора аргументов.

    //Комменты для понимания
    //struct option
    //}
    //const char *name;
    //int has_arg; -> no_argument, required_argument, optional_argument
    //int *flag; -> Если равен NULL, то возвращает val, иначе возвращает 0, 
    //              а переменная на которую указывает flag заполняется значением val
    //int val; -> значение, которое возвращается или загружается в переменную, на которую указывает flag.
    //}
    
    static struct option options[] = {{"seed", required_argument, 0, 0}, 
                                      {"array_size", required_argument, 0, 0}, 
                                      {"pnum", required_argument, 0, 0}, // кол-во процессов
                                      {"by_files", no_argument, 0, 'f'},  
                                      {"timeout", required_argument, 0 , 0}, // lab4
                                      {0, 0, 0, 0}}; // Последняя строка всегда нули
    int option_index = 0; 
    int c = getopt_long(argc, argv, "f", options, &option_index);

  {//Комменты для понимания
    // Аргументы(int argc, char *argv[], const char *optsting, const struct option *longotps, int *longindex)

    // *optsting краткие названия опций; *longotps - список(массив) длинных опций, доступных для этой функции;
    // Если longindex не равен NULL, то он указывает на переменную, содержащую индекс длинного параметра в соответствии с longopts.
    // Если опция принимает аргумент, то указатель на него помещается в переменную optarg
  } 
    
    if (c == -1) break; // значит функция getopt_long приняла все опции(не приняла никакой новой)

    switch (c) { 
      case 0:
        switch (option_index) { 
          case 0:
            seed = atoi(optarg);
            if (seed <= 0)
            {
		        printf("seed should be positive!\n\n");
		        return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0)
            {
		        printf("array_size should be positive!\n\n");
		        return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0)
            {
		        printf("pnum should be positive!\n\n");
		        return 1;
            }
            break;
          case 3:
            with_files = true;
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
        FILE* MyFile = fopen("file.txt", "w");
        fclose(MyFile);
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c); // %o восьмиричное число без знака
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");// Имеет по крайней мере один аргумент без опции
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  } // Вывод инструкции, если seed, array_size, pnum не менялись
 
  int *array = malloc(sizeof(int) * array_size); 
  GenerateArray(array, array_size, seed); 
  int active_child_processes = 0; 

  struct timeval start_time;
  gettimeofday(&start_time, NULL); //Запишет время от 01.01.1970 в структуру start_time
  pid_t *pid_child = (pid_t*) calloc(1,sizeof(pid_t)); 
  int c = 0;
  pid_t f_pid;


  int pipefd[2]; // статический массив с элементами 1 и 0
{//Комменты для понимания
  // pipefd[0] указывает на конец канала для чтения;
  // pipefd[1] указывает на конец канала для записи
}

  pipe(pipefd);  
  int number_segment = array_size / pnum;
  struct MinMax MyMinMax;

  if(timeout != -1)
  {
      printf("\nSet allarm on %d sec\n",timeout);
      alarm(timeout); //Через 10 секунд выполняет доставку сигнала SIGALRM
      signal(SIGALRM, kill_all); // Перехватывает сигнал и передаёт на обработку функции kill_all
  }

  for (int i = 0; i < pnum; i++) 
  {// fork и выполнение работы дочерними процессами
    pid_t pid = fork(); 

    //PID(Process IDentifier), PPID(Parent Process IDentifier) 
    // pid_t - это целый тип, размерность которого зависит от конкретной системы.

    // fork() работает так: 
    // Возвращает -1 для ошибок, 0 для нового процесса (дочернего) 
    // и идентификатор процесса нового процесса (дочернего) для старого процесса (родительского).
    
    if (pid == 0)
    {//Получаем pid дочерних процессов
        pid_child[c] = getpid();
        printf("Это pid дочернего процесса - %d\n",pid_child[c]);
        c++;
        pid_child = (pid_t*) realloc(pid_child,sizeof(pid_t) * (c + 1));
    }

    if (pid >= 0) 
    {// Дочерние процессы выполняют работу
      active_child_processes += 1; 
      if (pid == 0)
      {
        if (i != pnum - 1)
        	MyMinMax = GetMinMax(array, i * number_segment, (i+1) * number_segment);
	    else
		    MyMinMax = GetMinMax(array, i * number_segment, array_size);
            
        if (with_files) 
        {
          FILE* MyFile = fopen("file.txt", "a");
	      fwrite(&MyMinMax, sizeof(struct MinMax), 1, MyFile);
	      fclose(MyFile);
        } else 
        {
          close(pipefd[0]);
          write(pipefd[1], &MyMinMax, sizeof(struct MinMax));
          close(pipefd[1]);
        }
        exit(0);
      }
    } 
    else 
    {
      printf("Fork failed!\n");
      return 1;
    }
  }

  printf("Timeout now: %d\n" ,timeout);// выводит время перед началом таймаута
  while (active_child_processes > 0) 
  {
      wait(NULL);
      active_child_processes--; 
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
      FILE* MyFile = fopen("file.txt", "r");
      fseek(MyFile, i * sizeof(struct MinMax), SEEK_SET);    
{ // Пояснение про fseek
      // fseek задаёт смещение в файле MyFile, на i*sizeof(...) байт;  
      // Позиция указателя перед смещением задаётся выражениями
      //SEEK_SET	Начало файла
      //SEEK_CUR	Текущее положение файла
      //SEEK_END	Конец файла
}
      fread(&MyMinMax, sizeof(struct MinMax), 1, MyFile);
{// Пояснение про fread
      // Функция fread считывает массив размером — count элементов, каждый из которых имеет 
      // размер size байт, из потока, и сохраняет его в блоке памяти, на который указывает ptrvoid.
}
      fclose(MyFile);
    } 
    else 
    {
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

  free(pid_child);
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
