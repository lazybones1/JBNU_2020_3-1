#include<stdio.h>
#include<stdlib.h>

#define buf_size 3
int mutex=1,x=0; //상호배재, 버퍼크기 제어 변수, 아이템 수
int p_pos=0,c_pos=0; //생산자, 소비자 위치
int* buf; //버퍼, 처음은 0으로 모두 초기화

int main()
{
	int n;
  buf = (int*)malloc(sizeof(int)*buf_size);
	void producer();
	void consumer();
  void show_status();
	int wait(int);
	int signal(int);
	printf("\n1.Producer\n2.Consumer\n3.Show_Status\n4.Exit");
	while(1)
	{
		printf("\nEnter your choice:");
		scanf("%d",&n);
		switch(n)
		{
			case 1:	if((mutex==1)&&(x!=buf_size))
						producer();
					else
						printf("Buffer is full!!");
					break;
			case 2:	if((mutex==1)&&(x!=0))
						consumer();
					else
						printf("Buffer is empty!!");
					break;
      case 3: if(mutex==1)
          show_status();
          break;
			case 4:
					exit(0);
					break;
		}
	}

	return 0;
}

int wait(int s)
{
	return (--s);
}

int signal(int s)
{
	return(++s);
}

void show_status(){
  int i;
  mutex = wait(mutex);
  printf("-------Buffer status------\n");
  for(i=0; i<buf_size; i++){
    printf("Buffer %d : %d | ", i+1, buf[i]); // 버퍼의 상태 (1 = 아이템 있음, 0 = 아이템 없음)
  }
  printf("\n");
  printf("Buffer size : %d\n",buf_size); //총 버퍼 크기
  printf("Producer position : %d\n", p_pos); //생산자 위치
  printf("Customer position : %d\n", c_pos); //소비자 위치
  printf("Number of items : %d\n", x); //아이템 수
  printf("Empty slot : %d\n", buf_size-x); //빈 슬롯 수
  printf("---------------------------\n");
  mutex=signal(mutex);
}

void producer()
{
	mutex=wait(mutex); //상호 배제 on
  if(p_pos!=buf_size) p_pos++; //생산자의 위치 조정
  else p_pos = 1;
  buf[p_pos-1] = 1; // 버퍼에 값 추가
	x++; //아이템 수 증가
	printf("\nProducer produces the item %d\n",x);
	mutex=signal(mutex); //상호 배제 off
}

void consumer()
{
	mutex=wait(mutex); //상호 배제 on
  buf[c_pos] = 0; //버퍼에 값 제거
  if(c_pos!=buf_size) c_pos++; //소비자의 위치 조정
  else c_pos = 1;
	printf("\nConsumer consumes item %d\n",x);
	x--; //아이템 수 감소
	mutex=signal(mutex); //상호 배제 off
}
