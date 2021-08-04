/**
 * Santa Claus problem - IOS 2nd project
 * @author Karel Norek, xnorek01
 * @date 26.04.2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/wait.h>
#include <signal.h>

// Allocate shared memory
#define MMAP(pointer) {(pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
// Destroy shared memory
#define UNMAP(pointer) {munmap((pointer), sizeof(*(pointer)));}
// Open semaphore
#define SEM_OPEN_CHECK(name, string, val) if ((name = sem_open(string, O_CREAT | O_EXCL, 0666, val)) == SEM_FAILED) \
    { perror("Couldn't initialise a semaphore"); return -1; }

struct arguments {
    unsigned NE; // Number of elves
    unsigned NR; // Number of raindeers
    unsigned TE; // Max wait time that elf can work alone (in ms)
    unsigned TR; // Max wait time that raindeer can be on vacation (in ms)
} args;

// Shared memory
int *counter = NULL;
int *elfCounter = NULL;
int *workshop = NULL;
unsigned *rainCounter = NULL;
unsigned *hitched = NULL;
// Semaphores
sem_t *output;
sem_t *santaSem;
sem_t *raindeerSem;
sem_t *elfSem;
sem_t *santaHelp;
sem_t *helped;
sem_t *mutex;

// Init shared memory and semaphores
int init() {
    MMAP(counter);
    MMAP(rainCounter);
    MMAP(elfCounter);
    MMAP(hitched);
    MMAP(workshop);
    SEM_OPEN_CHECK(output, "/xnorek01.ios.proj2.sem.out", 1);
    SEM_OPEN_CHECK(santaSem, "/xnorek01.ios.proj2.sem.santa", 0);
    SEM_OPEN_CHECK(raindeerSem, "/xnorek01.ios.proj2.sem.rain", 0);
    SEM_OPEN_CHECK(elfSem, "/xnorek01.ios.proj2.sem.elf", 1);
    SEM_OPEN_CHECK(santaHelp, "/xnorek01.ios.proj2.sem.elp", 0);
    SEM_OPEN_CHECK(helped, "/xnorek01.ios.proj2.sem.helped", 0);
    SEM_OPEN_CHECK(mutex, "/xnorek01.ios.proj2.sem.mutex", 1);
    *workshop = 1;
    return 0;
}

// Free semaphores
void cleanSem(sem_t *sem, char *name){
    sem_close(sem);
    sem_unlink(name);
}

// Clean shared memory and semaphores
void cleanup(FILE *file) {
    fclose(file);
    UNMAP(counter);
    UNMAP(rainCounter);
    UNMAP(elfCounter);
    UNMAP(hitched);
    UNMAP(workshop);
    cleanSem(output, "/xnorek01.ios.proj2.sem.out");
    cleanSem(santaSem, "/xnorek01.ios.proj2.sem.santa");
    cleanSem(raindeerSem, "/xnorek01.ios.proj2.sem.rain");
    cleanSem(elfSem, "/xnorek01.ios.proj2.sem.elf");
    cleanSem(santaHelp, "/xnorek01.ios.proj2.sem.elp");
    cleanSem(helped, "/xnorek01.ios.proj2.sem.helped");
    cleanSem(mutex, "/xnorek01.ios.proj2.sem.mutex");
}

// Santa process
void santa(FILE *file) {
    sem_wait(output);
    fprintf(file, "%d: Santa: going to sleep\n", ++(*counter));
    fflush(file);
    sem_post(output);

    int alreadyGone = 0;
    while(*hitched != args.NR){
        sem_wait(santaSem);
        sem_wait(mutex);
        if (*rainCounter == args.NR && alreadyGone == 0) {
            alreadyGone = 1;
            sem_wait(output);
            fprintf(file, "%d: Santa: closing workshop\n", ++(*counter));
            fflush(file);
            sem_post(output);

            for (unsigned i = 0; i < args.NR; i++) {
                sem_post(raindeerSem);
            }

            *workshop = 0;
            for (unsigned i = 0; i < args.NE; i++) {
                sem_post(santaHelp);
            }
            
        }else if (*elfCounter == 3 && alreadyGone == 0){
            sem_wait(output);
            fprintf(file, "%d: Santa: helping elves\n", ++(*counter));
            fflush(file);
            sem_post(output);

            for (int i = 0; i < *elfCounter; i++){
                sem_post(santaHelp);
            }

            for (int i = 0; i < 3; i++){
                sem_wait(helped);
            }     
            
            sem_wait(output);
            fprintf(file, "%d: Santa: going to sleep\n", ++(*counter));
            fflush(file);
            sem_post(output);

        }
        sem_post(mutex);
    }

    sem_wait(output);
    fprintf(file, "%d: Santa: Christmas started\n", ++(*counter));
    fflush(file);
    sem_post(output);
 
    exit(0);
}

// Raindeer process
void raindeer(FILE *file, unsigned i){
    sem_wait(mutex);
    sem_wait(output);
    fprintf(file, "%d: RD %d: rstarted\n", ++(*counter), ++i);
    fflush(file);
    sem_post(output);

    sem_post(mutex);
    usleep(rand() % (args.TR + 1) * 1000);
    sem_wait(mutex);
    
    sem_wait(output);
    fprintf(file, "%d: RD %d: return home\n", ++(*counter), i);
    fflush(file);
    sem_post(output);
    (*rainCounter)++;

    if (*rainCounter == args.NR) {
        sem_post(santaSem);
    }
    sem_post(mutex);

    sem_wait(raindeerSem);

    sem_wait(output);
    fprintf(file, "%d: RD %d: get hitched\n", ++(*counter), i);
    fflush(file);
    (*hitched)++;
    if (*hitched == args.NR){
        sem_post(santaSem);
    }
    sem_post(output);

    
    exit(0);
}

// Elf process
void elf(FILE *file, unsigned i) {
    sem_wait(output);
    fprintf(file, "%d: Elf %d: started\n", ++(*counter), ++i);
    fflush(file);
    sem_post(output);
    int gotHelp = 0;
    while (*workshop == 1){
        sem_wait(elfSem);
        usleep(rand() % (args.TE + 1) * 1000);
        sem_wait(mutex);

        
        
        sem_wait(output);
        (*elfCounter)++;
        fprintf(file, "%d: Elf %d: need help\n", ++(*counter), i);
        fflush(file);
        sem_post(output);
        gotHelp = 1;

        if (*elfCounter == 3 && *workshop != 0){
            sem_post(santaSem);
        }else  sem_post(elfSem);
        sem_post(mutex);

        sem_wait(santaHelp);
        if (*workshop == 0){
            sem_wait(output);
            (*elfCounter)--;
            if (*elfCounter == 0){
                sem_post(elfSem);
            }
            sem_post(output);
            break;
        }

        sem_wait(output);
        fprintf(file, "%d: Elf %d: get help\n", ++(*counter), i);
        fflush(file);
        sem_post(output);

        sem_post(helped);
        
        sem_wait(mutex);
        sem_wait(output);
        (*elfCounter)--;
        if (*elfCounter == 0){
            sem_post(elfSem);
        }
        gotHelp = 0;
        sem_post(output);
        sem_post(mutex);
    }
    if (gotHelp == 0){
        sem_wait(output);
        fprintf(file, "%d: Elf %d: need help\n", ++(*counter), i);
        fflush(file);
        sem_post(output);
    }
    sem_wait(output);
    fprintf(file, "%d: Elf %d: taking holidays\n", ++(*counter), i);
    fflush(file);
    sem_post(output);

    exit(0);
}

// Wait for child processes to end
int waitForChildren(pid_t santa, pid_t elfs[args.NE], pid_t deers[args.NR]){
    int retCode = 0;
    int childStatus;

    for (unsigned i = 0; i < args.NR; i++) {
        waitpid(deers[i], &childStatus, 0);
        if (!WIFEXITED(childStatus)) {
            fprintf(stderr, "Raindeer %d didn't exit normally.\n", i + 1);
            kill(santa, SIGKILL);
            retCode = 1;
        }
    }

    for (unsigned i = 0; i < args.NE; i++) {
        waitpid(elfs[i], &childStatus, 0);
        if (!WIFEXITED(childStatus)) {
            fprintf(stderr, "Elf %d didn't exit normally.\n", i + 1);
            kill(santa, SIGKILL);
            retCode = 1;
        } 
    }

    waitpid(santa, &childStatus, 0);
    if (!WIFEXITED(childStatus)) {
        fprintf(stderr, "The santa process didn't exit normally.\n");
        retCode = 1;
    }

    return retCode;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Wrong number of arguments\n");
        return 1;
    }

    char *ptr = NULL;
    args.NE = strtoul(argv[1], &ptr, 10);
    args.NR = strtoul(argv[2], &ptr, 10);
    args.TE = strtoul(argv[3], &ptr, 10);
    args.TR = strtoul(argv[4], &ptr, 10);
    if (*ptr != 0 || args.NE < 1 || args.NE >= 1000 || args.NR >= 20 || args.NR < 1 || args.TE > 1000 || args.TR > 1000) {
        fprintf(stderr, "Wrong arguments\n");
        return 1;
    }

    FILE *file;
    if (!(file = fopen("proj2.out", "w"))) {
        fprintf(stderr, "Couldn't open file\n");
        return 1;
    }

    if (init() == -1) {
        cleanup(file);
        return 1;
    }

    // Generate Santa process
    pid_t pidSanta = fork();
    if (pidSanta == 0) {
        santa(file);
    } else if (pidSanta == -1) {
        fprintf(stderr, "Couldn't spawn the santa process.\n");
        cleanup(file);
        return 1;
    }

    // Generate number of elf processes
    pid_t pidElf[args.NE];
    for (unsigned i = 0; i < args.NE; i++){
        pidElf[i] = fork();
        if (pidElf[i] == 0) {
            elf(file, i);
        } else if (pidElf[i] == -1) {
            fprintf(stderr, "Couldn't spawn the elf process.\n");
            kill(pidSanta, SIGKILL);
            cleanup(file);
            return 1;
        }
    }

    // Generate number of raindeer processes
    pid_t pidDeer[args.NR];
    for (unsigned i = 0; i < args.NR; i++){
        pidDeer[i] = fork();
        if (pidDeer[i]  == 0) {
            raindeer(file, i);
        } else if (pidDeer[i]  == -1) {
            fprintf(stderr, "Couldn't spawn the raindeer process.\n");
            kill(pidSanta, SIGKILL);
            cleanup(file);
            return 1;
        }
    }

    if (waitForChildren(pidSanta, pidElf, pidDeer)){
        cleanup(file);
        return 1;
    }else {
        cleanup(file);
        return 0;
    }
}
