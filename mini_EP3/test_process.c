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
    printf("interval_size %lf\n", interval_size);
    // Integrates f(x) = sqrt(1 - x^2) in [t->start, t->end[
    for(int i = start; i < end; ++i) {
        double x = (i * interval_size) + interval_size / 2;
        acc += sqrt(1 - (x * x)) * interval_size;
    }

    printf("ACC = %lf\n", acc);
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
  struct shmid_ds buff;
  int shmid;
  size_t size_memory = 1024;
  double vector[number_process];
  double *point_acess;
  int flag = 0;

  /* crio a posição de memoria */
  shmid = shmget(IPC_PRIVATE, sizeof(vector), IPC_CREAT | S_IRUSR | S_IWUSR);
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

  /* Copy information of memory shared */
  int check = shmctl(shmid, IPC_STAT, &buff);
  if(check == -1){
      printf("Erro in copy information of memory shared\n");
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
          if(getpid() != save_pid){
              printf("VAL %d\n", val);
              pi_by_pi(start, end, val, (point_acess+b));
          }
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
        double tmp = 0;
        for(int c = 0; c < number_process; c++){
            tmp = *(point_acess+c);
            resultado_geral += tmp;
            printf("TMP: %lf\n", tmp);
            printf("position[%d] = %lf\n", c, *(point_acess+c));
        }

        printf("ESTADO DO SEGMENTO DE MEMORIA COMPARTILHADA %d\n",shmid) ;
        printf("ID do usuario proprietario: %d\n",buff.shm_perm.uid) ;
        printf("ID do grupo do proprietario: %d\n",buff.shm_perm.gid) ;
        printf("ID do usuario criador: %d\n",buff.shm_perm.cuid) ;
        printf("ID do grupo criador: %d\n",buff.shm_perm.cgid) ;
        printf("Modo de acesso: %d\n",buff.shm_perm.mode) ;
        printf("Tamanho da zona de memoria: %d\n",buff.shm_segsz) ;
        printf("pid do criador: %d\n",buff.shm_cpid) ;
        printf("pid  (ultima operacao): %d\n",buff.shm_lpid) ;
        printf("aqui %d\n", buff.shm_nattch);

        /* Destroy a memory shered */
        int check_destroy = shmctl(shmid, IPC_RMID, NULL);
        if(check_destroy == -1){
            printf("Erro in destroy memory shared\n");
            exit(1);
        }
        if(buff.shm_perm.mode == SHM_DEST)
            printf("ok\n");

        printf("ACABEI\n\n");
        printf("RESULTADO GERAL: %.12f\n\n", resultado_geral* 4);
        printf("RESULTADO TMP: %lf\n\n", tmp);
  }

  return 0;
};
