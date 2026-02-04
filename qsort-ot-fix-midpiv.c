/*Denna variant av quicksort:
-FAST tröskel för att skapa tasks
-Pivot elementet väljs alltid i mitten
-Tröskel uppnådd => största vektor som task, mindre vektor utförs av nuvarande tråd*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h> //För rand seed

#define TASK_THRESHOLD 100000
int task_threshold;


//Byt plats på två element
//skicka in som pointers -> annars ändras ej värden utan funktionen körs enbart på kopior
void swap(int* a, int*b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int chose_pivot(int low, int high){
    return (low + high)/2;
}

int split(int arr[], int low, int high){
    //välj ett slumpmässigt element
    int chosen_element_index = chose_pivot(low, high);

    //flytta valt element till sista platsen i arr
    swap(&arr[chosen_element_index], &arr[high]);
    int chosen_element = arr[high];

    int index = low -1;

    //Gå igenom arr[] från low till high, placera mindre element till v. om valt element
    for(int i = low; i < high; i++){
        if(arr[i] < chosen_element){
            index++;
            swap(&arr[index], &arr[i]);
        }
    }

    //flytta tillbaka det valda elementet och returnera dess index
    swap(&arr[index+1],&arr[high]);
    return index+1;
}

void quick_sort(int arr[], int low, int high){
    
    if(low < high){
        int partition_index = split(arr, low, high);
        int left_size = partition_index -1 -low;
        int right_size = high - (partition_index +1);

        if(task_threshold < high - low){ //tillräckligt stort problem => tasks
            if(left_size > right_size){
                #pragma omp task //utförs av annan tråd
                quick_sort(arr, low, partition_index -1);

                quick_sort(arr, partition_index +1, high); //utförs av tråden
            } else {
                #pragma omp task
                quick_sort(arr, partition_index +1, high);

                quick_sort(arr, low, partition_index -1);
            }

        } else{                 //sekventiellt
            quick_sort(arr, low, partition_index-1);
            quick_sort(arr, partition_index +1, high);
        }
    }


}

int main(int argc, char* argv[]){

    int num_loops = 10;
    int num_threads = 4;
    double total_time = 0.0;
    int arr_size = 10000000;
    srand(time(NULL));

    
    switch (argc)
    {
    case 1:
        printf("Usage: ./QSOT-fix-midpiv + 'number of cores' + 'array size'\n");
        printf("Number of threads: %d, Array size: %d\n", num_threads, arr_size);
        break;

    case 2:
        num_threads = atoi(argv[1]);
        printf("Number of threads: %d, Array size: %d\n", num_threads, arr_size);
        break;

    case 3:
        num_threads =atoi(argv[1]);
        arr_size = atoi(argv[2]);
        printf("Number of threads: %d, Array size: %d\n", num_threads, arr_size);
        break;

    default:
        break;
    }

    omp_set_num_threads(num_threads);

    task_threshold = TASK_THRESHOLD;

    //printf("Array size: %d\n", arr_size);

    for(int run = 0; run < num_loops; run++){

        double start_time, run_time;
        int* array = malloc(arr_size* sizeof(int));
        
        for (int i = 0; i < arr_size; i++){ // Init array med random ints
            array[i] = rand() % 10000;
        }

        start_time = omp_get_wtime();

        #pragma omp parallel
        { 
            #pragma omp single
            {
                quick_sort(array, 0, arr_size-1);
            }
            //Barriär
        }
        run_time = omp_get_wtime() - start_time;
        total_time += run_time;
        
        //printf("Run number %d -- Time: %f\n", run, run_time);
    
        /*if(run == num_loops -1){
            printf("First 10 integers in sorted array: ");
            for(int j = 0; j < 9; j++){
                printf("%d ", array[j]);
            }
            printf("\n");
        }*/

        free(array);
        

    }

    double avg_time = total_time / num_loops;
    printf("QSOT-FIX-MP: Average time: %f - Number of cores: %d\n", avg_time, num_threads);
    printf("Threshold: %d\n", task_threshold);

}

// https://www.geeksforgeeks.org/dsa/quick-sort-using-multi-threading/
/*

BAKGRUND:
QuickSort:
1. Välj random element i arr
2. Sortera arr så att alla element < valt element placeras till vänster om elementet
& alla element > valt elemt placeras till höger om elementet (partitioning)
3. Anropa quicksort rekursivt på vänstra och högra sub-arrays
4. Basfall: om array size är 0 eller 1, returnera

Rekursiv algoritm --> vet ej i förväg hur många gånger koden kommer utföras från start

#pragma omp task
rekursiv_funk()

=> skapa ett task objekt (pekare till funk + arg) som läggs i task-queue tillhörande den tråd
som skapade tasken. Färdiga trådar hämtar lediga tasks från kön

#pragma omp parallel
{
kodblock
}
=> Alla trådar utför koden inom {}, antal trådar sätts av export OMP_NUM_THREADS 'n'

#pragma omp parallel for schedule(static, (static, chunk), dynamic)
for (int i = 0; i < N; i++) {
    work(i);
}
=> delar upp blocket till trådarna
-static -> varje tråd får 1 lika stort block
-static,chunk -> varje tråd får 1/flera block av storlek 'chunk'
-dynamic -> varje tråd hämtar 1 iteration åt gången (större overhead - bra för variarande arbetsmängd)
-dynamic, chunk -> Block läggs på en kö som hämtas dynamiskt av trådarna

#pragma omp single
{
kodblock
}
=> Endast en tråd kör kod inom blocket men andra trådar kan fortfarande arbeta

#pragma omp taskwait
=> tråd som når taskwait väntar tills att alla tasks tråden skapat är klara innan ==> nästa rad




*/