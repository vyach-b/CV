//
//  HashOpenAdress.c
//  informatrices
//
//  Created by Slaviq on 24.05.2025.
//

#include "stdio.h"
#include "stdlib.h"
#include "limits.h"
#include "time.h"
#include "math.h"
#include "stdbool.h"
#define NEIL -1.0
#define DELL -2.0
#define m 9551


int hash_one(int k){
    return k % m;
}

int hash_two(int k){
    return 1 + (k % (m - 1));
}

int hash_double(int k, int i){
    int h1 = hash_one(k);
    int h2 = hash_two(k);
    return (h1 + i * h2) % m;
} // for any K {hash_double(k, 0), hash_double(k, 1), ... hash_double(k, m-1)} == {0, 1, 2, ... , m-1}

int hash_quadratic(int k, int i){
    int h_ = hash_one(k);
    int c1 = 3, c2 = 16386 ;// why?????????? /* по полученным в ходетестирования результатам при большой разности чисел заполнение происходит быстрее*/
    return (h_ + i* c1 + i * i * c2) % m;
}


int insert_table(double* arr, int key, double value, int(*hfunc)(int, int)){
    int i;
    for (i = 0; i < m; i++){
        int h = hash_double(key, i);
        if (arr[h] == NEIL){
            arr[h] = value;
            return 0;
        }
    }
    return -1;
}
int search_table(double* arr, int key, int(*hfunc)(int, int)){
    int i;
    for (i = 0; i < m; i++){
        int h = hfunc(key, i);
        if (arr[h] != NEIL){
            return 0;
        }
    }
    return -2;
}

int remove_table(double* arr, int key, int(*hfunc)(int, int)){
    for (int i = 0; i < m; i++){
        int h = hfunc(key, i);
        if (arr[h] != NEIL){
            arr[h] = NEIL;
            return 0;
        }
    }
    return -3;
}




int main(){
    double* arr1 = (double*)malloc(m * sizeof(double));
    double* arr2 = (double*)malloc(m * sizeof(double));
    double seconds = 0.0, secs = 0.0;
    for (int p = 0; p < m; p++){
        arr1[p] = NEIL;
        arr2[p] = NEIL;
    }
    clock_t start1, start2;
    clock_t end1, end2;
    /*start1 = clock();
    insert_table(arr1, (int)rand(), rand(), hash_double);
    search_table(arr1, (int)rand(), hash_double);
    remove_table(arr1, rand(), hash_double);
    end1 = clock();
    seconds += (double)(end1 - start1)/ CLOCKS_PER_SEC;*/
    start2 = clock();
    insert_table(arr2, (int)rand(), rand(), hash_quadratic);
    search_table(arr2, (int)rand(), hash_quadratic);
    remove_table(arr2, (int)rand(), hash_quadratic);
    end2 = clock();
    secs += (double)(end2 - start2) / CLOCKS_PER_SEC;
    printf("%lf/n", seconds);
    printf("%lf/n", secs);
}
/* "good" parameters: for double hashing, h1 and h2 should be mutually simple, size of m is irrelevant; for quadratic hashing, c1 and c2 should be mutually simple and have a difference of several orders of magintude, i. e., 3 and 16386, m should have mod 2 == 0 */
/* "bad" parameters: for double hashing, h1 mod h2 == 0, h1 mod m = h2 mod m = 0; for quadratic hashing, c1 and c2 should be close (i. e., 15 & 16), m size is irrelevant*/
/* results:
    "good": double: approx. 0.0015 secs for 10 runs, quadratic: approx 0.1 for 10 runs
    "bad": double: approx 0.002 secs for 11 runs, quadratic: approx 0.2 for 11 runs
 */
