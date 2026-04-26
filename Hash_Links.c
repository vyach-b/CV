#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "math.h"
#include "time.h"
#define m 9551

//16384

typedef struct Hlist{
    int key;
    double val;
    struct Hlist* next;
    
} Hlist;

int hash1(int k){
    double q = (double)m;
    double h = (double)k;
    double A = (sqrt(5.0) - 1.0) / 2.0;
    return floor(q * ((h * A) - floor(h * A)));
}

int hash2(int k){
    return k % m;
}

int insert_table(Hlist** arr, int q, int key, double val, int(*hash)(int)){
    int h = hash(key);
    Hlist* link = malloc(sizeof(Hlist));
    link->key = key;
    link->val = val;
    Hlist* tmp = arr[h];
    arr[h] = link;
    arr[h]->next = tmp;
    free(tmp);
    return 0;
}




double find_table(Hlist** arr, int q, int key, int(*hash)(int)){
    int h = hash(key);
    if (arr[h]){
        Hlist* tmp = arr[h];
        while (tmp && tmp->key != key){
            tmp = tmp->next;
        }
        if(tmp){
            return 0.0;
        } // clear break!
    }
    return -1.0;
}

int remove_table(Hlist** arr, int q, int key, int(*hash)(int)){
    int h = hash(key);
    if (arr[h]){
        Hlist* tmp = arr[h];
        Hlist* prev = NULL;
        while (tmp && tmp->key != key){
            prev = tmp;
            tmp = tmp->next;
        }
        if (tmp){
            if (prev){
                prev->next = tmp->next;
                free(tmp);
            }
            else{
                arr[h] = tmp->next;
                free(tmp);
            }
            return 0;
        }
    }
    return -1;
}

int main(){
    Hlist** array = (Hlist**)malloc(sizeof(Hlist*) * m);
    double secs1 = 0.0; double secs2 = 0.0;
    for (int i = 0; i < m; i++){
        array[i] = NULL;
    }
    clock_t start_1, end_1, start_2, end_2;
    int i;
    for (i = 0; i < m; i++){
        insert_table(array, m, (int)rand(), rand(), hash2);
    }
    start_1 = clock();
    find_table(array, m, rand(), hash2);
    end_1 = clock();
    start_2 = clock();
    remove_table(array, m, rand(), hash2);
    end_2 = clock();
    secs1 += (double)(end_1 - start_1)/ CLOCKS_PER_SEC;
    secs2 += (double)(end_2 - start_2)/ CLOCKS_PER_SEC;
    printf("%lf\n", secs1);
}





/* "bad" parameters: m being a simple number, 1048579 in case; secs (approximately) = 0.16 (based on 12 runs)*/
/* "good" parameters: m = 2^n (n = 20 in test); secs (approximately) = 0.15 (based on 10 runs) */
