#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <windows.h>

void main()
{
    //변수선언
    int n; //1~9 까지 프로세스의 개수
    int b[10]; //실행 시간
    int w[10]; //대기시간
    int a[10]; // 프로세스 번호
    int i, j;   //for문 변수
    int h;     //간트차트 표시 변수
    int t, tt, ttt, tmp=0;  //임시 변수 (순서 변환)
    int stime[10]; //도착시간
    float avg = 0;  // 모든 프로세스의 대기시간의 합

     //메뉴화면
    system("cls");  //clear screen
    printf("\n\tJOB SCHEDULING ALGORITHM[SJF]");
    printf("\n\t*****************************************************\n");
    printf("\nEnter howmany jobs:");
    scanf("%d", &n);

    //각각의 프로세스의 실행시간 입력
    printf("\nEnter burst time for corresponding job....\n");
    for (i = 1; i <= n; i++)
    {
        printf("\nProcess %d:", i);
        scanf("%d", &b[i]);
        a[i] = i;
    }

    //도착시간 입력 (프로세스 1은 0으로 고정)
    printf("\nEnter Arrived time for corresponding job....\n");
    stime[1] = 0;
    for (i = 2; i <= n; i++)
    {
        printf("\nProcess %d:", i);
        scanf("%d", &stime[i]);
    }

    //우선순위 변경 (실행시간이 적을수록 우선순위가 높음)
    for (i = 2; i <= n; i++) {
        for (j = i + 1; j <= n; j++) {
            if (b[i] > b[j]){
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

    //1번째 프로세스는 대기시간이 없음
    w[1] = 0;
    printf("\nprocess %d waiting time is 0", a[1]);

    //대기시간 구하기 (대기시간 = 이전 프로세스 실행시간 + 이전 프로세스 시작까지의 시간)
    for (i = 2; i <= n; i++){
        if (tmp == 0) {
            w[i] = b[i - 1] + w[i - 1];
        }
        else {
            w[i] = b[i - 1] + tmp;
        }
        //우선순위가 가장 높은 일이 아직 도착하지 않았을 경우
        if (stime[i] > w[i]) {
            //도작한 프로세스중 가장 우선순위가 높은 프로세스 실행
            for (j = i + 1; j <= n; j++) {
                if (stime[j] <= w[i]) {
                    t = b[j];
                    tt = a[j];
                    ttt = stime[j];
                    for (int k = j - 1; k >= i; k--) {
                        b[k + 1] = b[k];
                        a[k + 1] = a[k];
                        stime[k + 1] = stime[k];
                    }
                    b[i] = t;
                    a[i] = tt;
                    stime[i] = ttt;
                    break;
                }
            }
        }
        //도착한 프로세스가 없을 경우 프로세스 도착까지 대기 후 우선순위가 높은 프로세스 실행
        if (stime[i] > w[i]) {
            tmp = stime[i];
            //도착과 동시에 실행되므로 대기시간 = 0;
            w[i] = 0;
        }
        else {
            tmp = w[i];
            w[i] = w[i] - stime[i];
        }
        printf("\nProcess %d waiting time: %d", a[i], w[i]);
        avg += w[i];
    }

    //모든 프로세스의 대기시간합, 평균대기시간 표기
    printf("\n\ntotal waiting time:%f", avg);
    printf("\n\nthe average waiting time is:%f\n", avg / n);

    //간트차트 표기
    printf("\nGaunt Chart\n***************************************\n\n\t");

    h = 22;
   // 실행시간 간트차트
    for (i = 1; i <= n; i++)
    {
        printf("P%d의 실행시간= %d \t", a[i], b[i]);
        for (j = 1; j <= b[i]; j++)
            printf("%c", h);
        printf("\n\t");
        Sleep(100);
    }
    printf("\n\t");

    //대기시간 간트차트
    for (i = 1; i <= n; i++)
    {
        printf("P%d의 대기시간= %d\t", a[i], w[i]);
        for (j = 1; j <= w[i]; j++)
            printf("%c", h);
        printf("\n\t");
        Sleep(100);
    }
    printf("\n***************************************\n\t");
    _getch();
    return;
}
