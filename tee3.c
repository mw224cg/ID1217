#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

/*Linux tee command:
-Producer skriver till två buffers, en per konsument
- varje konsument läser från sin egen buffer 
- varje konsument har sin egen semafor
- i början av programmet läggs konsumenterna i väntlistan "_full"
- stdinReader skriver till buffer och väcker motsvarande konsument mha post(konsumentNamn_full)
- varje konsument läser från sin egen buffer + skriver och väcker sedan stdinReader

*/

#define BUFFER_SIZE 1024

char stdout_buffer[BUFFER_SIZE];
int stdout_length;
char file_buffer[BUFFER_SIZE];
int file_length;
int eof = 0;

sem_t stdout_empty, stdout_full;
sem_t file_empty, file_full;

/* Reader thread function 
 - Reads from stdin and writes to both stdout and file buffers 
 - wakes up the writer threads until EOF */
void* stdinReader(void* arg){
    char local_buffer[BUFFER_SIZE];

    while(1){
        char* data = fgets(local_buffer, BUFFER_SIZE, stdin); //vänta på input från stdin

        sem_wait(&stdout_empty); // vänta tills stdout buffer är tom, stdoutWriter signal
        if(data == NULL || strcmp(data, "EOF\n") == 0){ // EOF
            eof = 1;
        } else {
            strcpy(stdout_buffer, local_buffer);
            stdout_length = strlen(local_buffer);
        }
        sem_post(&stdout_full);

        sem_wait(&file_empty); // vänta tills file buffer är tom, fileWriter signal
        if(data == NULL || strcmp(data, "EOF\n") == 0){ // EOF
            eof = 1;
        } else {
            strcpy(file_buffer, local_buffer);
            file_length = strlen(local_buffer);
        }
        sem_post(&file_full);

        if(eof){
            break;
        }
    }
    return NULL;
}

void* stdoutWriter(void* arg) {
    while (1) {
        sem_wait(&stdout_full);

        if (eof)
            break;

        fwrite(stdout_buffer, 1, stdout_length, stdout);
        fflush(stdout);

        sem_post(&stdout_empty);
    }
    return NULL;
}

void* fileWriter(void* arg) {
    char* filename = (char*) arg;
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    while (1) {
        sem_wait(&file_full);

        if (eof){
            break;
        }
        fwrite(file_buffer, 1, file_length, file);
        fflush(file);

        sem_post(&file_empty);
    }
    fclose(file);
    return NULL;
}

/* Main function 
-hämta filnamn från argv
- init semaforer --> startvärde buffer kan skrivas till av stdinReader tråden
- skapa trådar + ange trådfunktioner + argument
- stäng trådar */
int main(int argc, char* argv[]){
    if(argc < 2){
        printf("Usage: %s filename\n", argv[0]);
        exit(1);
    }
    char* filename = argv[1];

    sem_init(&stdout_empty, 0, 1); // initialisera tom buffer
    sem_init(&stdout_full, 0, 0);  // initialisera full buffer

    sem_init(&file_empty, 0, 1); // initialisera tom buffer
    sem_init(&file_full, 0, 0);  // initialisera full buffer

    pthread_t reader_thread, stdout_thread, file_thread;

    pthread_create(&reader_thread, NULL, stdinReader, NULL);
    pthread_create(&stdout_thread, NULL, stdoutWriter, NULL);
    pthread_create(&file_thread, NULL, fileWriter, (void*)filename);

    pthread_join(reader_thread, NULL);
    pthread_join(stdout_thread, NULL);
    pthread_join(file_thread, NULL);

    return 0;
}