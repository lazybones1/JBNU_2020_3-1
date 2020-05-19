#include <pthread.h>  // 스레드 헤더파일
#include <semaphore.h>  //세마포어 헤더파일
#include <stdio.h>
#include <unistd.h>//sleep 사용을 위한 헤더파일 (repl.it에서 windows.h 오류)

#define N 5 //철학자 수
#define THINKING 2 //철학자 상태
#define HUNGRY 1 //철학자 상태
#define EATING 0 //철학자 상태
#define LEFT (phnum + 4) % N // 철학자 수가 5명이므로 왼쪽
#define RIGHT (phnum + 1) % N // 철학자 수가 5명이므로 오른쪽

int state[N]; //철학자의 상태
int phil[N] = { 0, 1, 2, 3, 4 };

sem_t mutex; //상호 배제
sem_t S[N]; //세마포어

void show_state(){
   for(int i = 0; i<N; i++){
    printf(" |phil %d : %d|",i+1, state[i]);
  }
  printf("\n");
}

void show_graph(int i){
  sem_wait(&mutex);
  printf(" -----------------------------------------\n");
  printf(" phil : %d end\n", i+1);
  show_state();
  for(int i = 0; i<N; i++){
    int r = (i+1)%5;

    if(state[i] == 0){
      printf(" ───> |phil %d| <─── ",i+1);

    }else if(state[i] == 1){
      printf(" ---> |phil %d| <--- ",i+1);
    }else if(state[i] == 2){
      printf(" |phil %d| ",i+1);
    }
    printf("|fork %d|", i+1);
  }
  printf("\n");
  printf(" -----------------------------------------\n");
  sem_post(&mutex);
}

void test(int phnum)
{
	if (state[phnum] == HUNGRY
		&& state[LEFT] != EATING
		&& state[RIGHT] != EATING) {
		// state that eating
		state[phnum] = EATING;
		sleep(2);
		printf("Philosopher %d takes fork %d and %d\n",
					phnum + 1, LEFT + 1, phnum + 1);
		printf("Philosopher %d is Eating\n", phnum + 1);
		// sem_post(&S[phnum]) has no effect
		// during takefork
		// used to wake up hungry philosophers
		// during putfork
		sem_post(&S[phnum]); //세마포어 증가
	}
}

// take up chopsticks
void take_fork(int phnum)
{
	sem_wait(&mutex); //세마포어 감소
	// state that hungry
	state[phnum] = HUNGRY; //배고픔 상태로 변경
	printf("Philosopher %d is Hungry\n", phnum + 1);
	// eat if neighbours are not eating
	test(phnum); //양옆에서 먹고 있는 경우 세마포어 증가
	sem_post(&mutex); //세마포어 증가
	// if unable to eat wait to be signalled
	sem_wait(&S[phnum]); //세마포어 감소
	sleep(1);
}

// put down chopsticks
void put_fork(int phnum)
{
	sem_wait(&mutex); //세마포어 감소
	// state that thinking
	state[phnum] = THINKING; //생각상태로 변경
	printf("Philosopher %d putting fork %d and %d down\n",
		phnum + 1, LEFT + 1, phnum + 1);
	printf("Philosopher %d is thinking\n", phnum + 1);

	test(LEFT);
	test(RIGHT);
	sem_post(&mutex); //세마포어 증가
}

void* philospher(void* num)
{
	while (1) {
		int* i = num;
		sleep(1);
		take_fork(*i); //포크 잡기
		sleep(0);
		put_fork(*i); //포크 두기
    show_graph(*i);
	}
}

int main()
{
	int i;
	pthread_t thread_id[N]; //스레드형 배열

	sem_init(&mutex, 0, 1); // initialize the semaphores
	for (i = 0; i < N; i++) // initialize the semaphores
		sem_init(&S[i], 0, 0);

	for (i = 0; i < N; i++) { 	// create philosopher processes
			pthread_create(&thread_id[i], NULL,
					philospher, &phil[i]);
		printf("Philosopher %d is thinking\n", i + 1);
	}

	for (i = 0; i < N; i++) // 쓰레드 종료 대기
		pthread_join(thread_id[i], NULL);
}
