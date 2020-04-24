#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <windows.h>

void main()
{
    int n;  //프로세스 개수 10개로 제한되야함 (b, w의 크기가 10)
    int b[10];  //실행 시간
    int w[10];  //대기 시간
    int a[10]; //프로세스 번호
    int i, j;    // for문 변수
    int t, tt, ttt; // 도착시간에 따른 우선순위 변경을 위한 임시 변수
    int h;     //간트차트 표시 변수
    int stime[10]; // 도착시간
    float avg = 0; //모든 프로세스의 대기시간의 합

    //메뉴 화면
    system("cls");  //clear screen
    printf("\n\tJOB SCHEDULING ALGORITHM[FCFS]");
    printf("\n\t*****************************************************\n");
    printf("\nEnter howmany jobs:");
    scanf("%d", &n); // n= 일의 개수

    //각각의 프로세스의 실행시간 입력
    printf("\nEnter burst time for corresponding job....\n");
    for (i = 0; i < n; i++)
    {
        printf("\nProcess %d:", i + 1);
        scanf("%d", &b[i]);
        a[i] = i+1;
    }

      //도착시간 입력 (프로세스 0은 0으로 고정)
    printf("\nEnter Arrived time for corresponding job....\n");
    stime[0] = 0;
    for (i = 1; i < n; i++)
    {
        printf("\nProcess %d:", i + 1);
        scanf("%d", &stime[i]);
    }

    //도착시간이 빠른것이 우선순위가 높음
    for (i = 1; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (stime[i] > stime[j])
            {
                t = b[i];
                tt = a[i];
                ttt = stime[i];
                b[i] = b[j];
                a[i] = a[j];
                stime[i] = stime[j];
                b[j] = t;
                a[j] = tt;
                stime[j] = ttt;
            }
        }
    }

    //처음 시작하는 프로세스는 대기시간이 없음
    w[0] = 0; // w= waiting time
    printf("\nprocess 1 waiting time is 0");

    // 각각의 대기시간 구하기 //(대기시간 = 이전 프로세스 실행시간 + 이이전 프로세스 시작까지의 시간)
    t = 0;
    for (i = 1; i < n; i++)
    {
        if (t == 0) {
            w[i] = b[i - 1] + w[i - 1];
        }
        else {
            w[i] = b[i-1] + t;
        }
        //프로세스가 아직 도착하지 않은 경우 도착할때까지 대기
        if (w[i] < stime[i]) {
            //도착과 동시에 실행되므로 대기시간 = 0;
            t = stime[i];
            w[i] = 0;
        }
        else {
            t = w[i];
            w[i] = w[i] - stime[i];
        }
        printf("\nProcess %d waiting time: %d", a[i], w[i]);
        avg += w[i];
    }


    //모든 프로세스의 대기시간 합 표기, 평균 대기시간 표기
    printf("\n\ntotal waiting time:%f", avg);
    printf("\n\nthe average waiting time is:%f\n", avg / n);

    //간트차트 표시
    printf("\nGauntChart\n***************************************\n\n\t");
    h = 22;  //(synchronous idle = 작은네모모양)

   // 실행시간 간트차트
    for (i = 0; i < n; i++)
    {
        printf("P%d의 실행시간= %d \t", a[i], b[i]);
        for (j = 1; j <= b[i]; j++)
            printf("%c", h);
        printf("\n\t");
        Sleep(100);
    }
    printf("\n\t");

    //대기시간 간트차트
    for (i = 0; i < n; i++)
    {
        printf("P%d의 대기시간= %d\t", a[i],w[i]);
        for (j = 1; j <= w[i]; j++)
            printf("%c", h);
        printf("\n\t");
        Sleep(100);
    }
    printf("\n***************************************\n\t");
    _getch();
    return;
}
