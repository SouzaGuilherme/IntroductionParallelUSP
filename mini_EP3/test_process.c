#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <math.h>
#include <sys/ipc.h>

#define CEIL(a, b) (((a) + (b) - 1)/(b))

void pi_by_pi(int start, int end, double N, double* point_acess){
    double acc = 0; // Thread's local integration variable
    double interval_size = 1.0 / N; // The circle radius is 1.0

    // Integrates f(x) = sqrt(1 - x^2) in [t->start, t->end[
    for(int i = start; i < end; ++i) {
        double x = (i * interval_size) + interval_size / 2;
        acc += sqrt(1 - (x * x)) * interval_size;
        printf("ACC = %lu\n", acc);
    }

    (*point_acess)= acc;
};

int main(){
  int i=0, var=10;
  int status;
  pid_t pid;
  int store[3];
  int save_pid;
  unsigned number_process = 5;
  unsigned val=10;
  int work_mod = val % (number_process-1);
  int flag_work_more = 0;

  /* Create memory shared */
  //key_t key = 1027153;
  int shmid;
  size_t size_memory = 1024;
  double *vector;
  double *point_acess;
  int flag = 0;

  /* crio a posição de memoria */
  shmid = shmget(IPC_PRIVATE, sizeof(double*), IPC_CREAT | S_IRUSR | S_IWUSR);
  if(shmid == -1){
      printf("ERRO CREATE\n");
      exit(1);
  }

  /* Pego o ponteiro da posiçõ criada */
  point_acess = shmat(shmid, 0, flag);
  if(point_acess == (double*)-1){
      printf("ERRO AQUIIII\n");
      exit(1);
  }

  if(work_mod != 0)
      flag_work_more = 1;


  /* number process order */
  /* Fit in numeber of process */
  number_process -= 2;

  /* First Fork, created father and son */
  if((pid = fork()) < 0){
      perror("fork");
      exit(1);
  }
  /* Save PID father */
  if(pid != 0)
      save_pid = getpid();
  /* Father create process remaining */
  for(int b =0; b < number_process; b++){
      if(getpid() == save_pid){
          int work_for_process = val / (number_process);
          if(b < work_mod)
              work_for_process += 1;
          int start = b * work_for_process;
          int end = (b + 1) * work_for_process;
          printf("START %d\n", start);
          printf("END %d\n", end);
          if((pid = fork()) < 0){
              perror("fork");
              exit(1);
          }

          /* functions for son */
          if(getpid() != save_pid)
              pi_by_pi(start, end, val, (point_acess+b));
      }
  }

  if(save_pid != getpid())
      printf("Vou Morrer Pai\n");
  else{
        waitpid(pid, &status, 0);
        printf("Vendo os filhos morrerem!\n");
        /* somo os vlores da memoria compartilhada */
        printf("Sou Solitario AGORA\n\n");
        double resultado_geral = 0;
        for(int c = 0; c < number_process; c++){
            resultado_geral += *(point_acess+c);
            printf("position[%lu] = %f\n", c, *(point_acess+c));
        }
        printf("ACABEI\n\n");
        printf("RESULTADO GERAL: %lu\n\n", resultado_geral);
  }

  return 0;
};
