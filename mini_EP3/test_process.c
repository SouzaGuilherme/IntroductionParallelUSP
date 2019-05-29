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

/* 
 * Function Pi and division work, belongs to 
 * Matheus Bernadino and Giuliano Belinasse
 * Link: https://github.com/matheustavares/pi
 * Acess: 29/05/19
 *
 */

void pi_by_pi(int start, int end, double N, double* point_acess){
    double acc = 0; // Thread's local integration variable
    double interval_size = 1.0 / N; // The circle radius is 1.0

    // Integrates f(x) = sqrt(1 - x^2) in [t->start, t->end[
    for(int i = start; i < end; ++i) {
        double x = (i * interval_size) + interval_size / 2;
        acc += sqrt(1 - (x * x)) * interval_size;
    }

    (*point_acess)= acc;
};

int main(int argc, char **argv){
    if(argc != 3 || atoi(argv[1]) < 2 || atoi(argv[2]) < 1){
        printf("Error Arguments\n\n");
        exit(1);
    }

    pid_t pid;
    int save_pid, status;
    unsigned number_process = atoi(argv[1]), val = atoi(argv[2]);


    /* Set variables for memory shared */
    struct shmid_ds buff;
    int shmid;
    double vector, *point_acess;

    /* created memory shared */
    shmid = shmget(IPC_PRIVATE, (sizeof(vector) * number_process), IPC_CREAT | S_IRUSR | S_IWUSR);
    if(shmid == -1){
        printf("Erro in create momory shared\n");
        exit(1);
    }

    /* Get pointer of th position first of emory shared */
    point_acess = shmat(shmid, 0, 0);
    if(point_acess == (double*)-1){
        printf("Erro in get position first of memory shared\n");
        exit(1);
    }

    /* Copy information of memory shared */
    int check = shmctl(shmid, IPC_STAT, &buff);
    if(check == -1){
        printf("Erro in copy information of memory shared\n");
        exit(1);
    }

    /* Save PID father */
    save_pid = getpid();

    int work_mod = val % (number_process-1);
    /* Father create process son*/
    for(int b =0; b < number_process; b++){
        if(getpid() == save_pid){
            int work_for_process = val / (number_process);
            if(b < work_mod)
                work_for_process += 1;
            int start = b * work_for_process;
            int end = (b + 1) * work_for_process;
            if((pid = fork()) < 0){
                printf("Erro in fork\n");
                exit(1);
            }

            /* functions for son */
            if(getpid() != save_pid){
                pi_by_pi(start, end, val, (point_acess+b));
            }
        }
    }

    if(save_pid != getpid())
        goto fim;
    else{
        waitpid(pid, &status, 0);

        /* sum values in memory shared */
        double result_all = 0;
        double tmp = 0;
        for(int c = 0; c < number_process; c++){
            tmp = *(point_acess+c);
            result_all += tmp;
        }

        /* Destroy a memory shared */
        int check_destroy = shmctl(shmid, IPC_RMID, NULL);
        if(check_destroy == -1){
            printf("Erro in destroy memory shared\n");
            exit(1);
        }

        printf("\nPI ~= %.12f\n\n", result_all * 4);
    }

fim:
    return 0;
};
