#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define BSIZE 2
int *maximum;
int *ptr;
int *ptr_2;
pthread_t storage[2];//used to store id

//structure to maintain items
typedef struct {
  int que[BSIZE];//buffer/queue to hold items
  int stock;//num currently in the queue
  int nextin;//next to be filled
  int nextout;//next to be evacuated
  pthread_mutex_t mu;//mutex
  pthread_cond_t sig_con;
  pthread_cond_t sig_prod;
} queue_t;

queue_t b = {.mu = PTHREAD_MUTEX_INITIALIZER, .sig_con =  PTHREAD_COND_INITIALIZER, .sig_prod =  PTHREAD_COND_INITIALIZER};


int get_max(){

  return(*maximum);
}

void *producer(void *d)
{
  
  int i,big;
  
  big = get_max();
  int item = 0;
  for(i=0; i<= big; i++){
    
  pthread_mutex_lock(&b.mu);  
  while (b.stock >= BSIZE){
    pthread_cond_wait(&b.sig_prod, &b.mu);
  }
  assert(b.stock < BSIZE);
  b.que[b.nextin++] = item;
  item++;
  b.nextin %= BSIZE;
  b.stock++;
  printf("Producer inserting %d:\n", i);
  // printf("%d\n", b.stock);
  pthread_cond_signal(&b.sig_con);
  pthread_mutex_unlock(&b.mu);
  if(item == big){
    break;

  }
  }
  pthread_exit(0);

}


void *consumer(void*  d)
{
  int i,big,item,test_1, test_2;
  int count_1 = 0;
  int count_2 = 0;
  int catchup = 0;
  big = get_max();
  while(item <= big){
    
  pthread_mutex_lock(&b.mu);
  while(b.stock <= 0){
    pthread_cond_wait(&b.sig_con, &b.mu);
  }
  assert(b.stock > 0);
  item = b.que[b.nextout++];
  b.nextout %= BSIZE;
  b.stock--;
  //  printf("consumer #%d extracts %d\n",pthread_self(), item);
  
  pthread_cond_signal(&b.sig_prod);
  // printf("consumer #%d extracts %d\n",pthread_self(), item);     
  pthread_mutex_unlock(&b.mu);
  if(storage[0] == 0){
    storage[0] = pthread_self();
  }
  else if(storage[1] == 0 && storage[0] != pthread_self()){
    storage[1] = pthread_self();
  }

  else{}
  printf("consumer #%d extracts %d\n",pthread_self(), item);     
  
  
 
  test_1 = pthread_equal(pthread_self(), storage[0]);
  test_2 =  pthread_equal(pthread_self(), storage[1]);
  if(test_1  == 1){
    count_1++;
  }
 
  if(test_2 == 1){
    count_2++;
   }
  
  
  if(item == big){
  *ptr = count_1;
  *ptr_2 = count_2;
  break;
  }
  }

}

int main(int argc, char* argv[]){
  pthread_t a[3];
  int  con, con_2, prod;
  
  ptr = (int*)malloc(sizeof(int));
  ptr_2 = (int*)malloc(sizeof(int));
  maximum = (int*)malloc(sizeof(int));
  if(argc > 0){
    *maximum = atoi(argv[1]); 
    
    prod =  pthread_create(&a[0], NULL, producer, NULL);
    con =  pthread_create(&a[1], NULL, consumer, NULL);
    con_2 =  pthread_create(&a[2], NULL, consumer, NULL);
 
    pthread_join(a[0], NULL);
                                                                                                                                                                               
    int dif = get_max();                                                                                                                                                          
    dif = dif - *ptr;                                                                                                                                                                 int dif_2 = get_max();                                                                                                                                                        
    dif_2 = dif_2 - *ptr_2;                                                                                                                                                      
    if(*ptr > 0){                                                                                                                                                              
    printf("Consumer #%d, consumes %d times\n", storage[0], *ptr);                                                                                                             
    printf("Consumer #%d consumes %d times\n", storage[1] , dif );                                                                                                                
    }                                                                                                                                                                             
    if(*ptr_2 > 0){                                                                                                                                                              
      printf("Consumer #%d, consumes %d times\n", storage[0], dif_2);                                                                                                             
      printf("Consumer #%d consumes %d times\n", storage[1] , *ptr_2);                                                                                                           
                                                                                                                                                                                  
    }                                                                                                                                                                              
    
    pthread_cancel(a[1]);
    pthread_cancel(a[2]);
  
     
  free(maximum);
  free(ptr);
  free(ptr_2);
  }
  pthread_exit(0);
  return 0;
}
