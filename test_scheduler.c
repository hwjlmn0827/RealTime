#include "math.h"
#include "sched.h"
#include "pthread.h"
#include "stdlib.h"
#include "semaphore.h" 
#include "stdio.h"
#include "unistd.h"

// https://medlen.blog.csdn.net/article/details/80772201?utm_medium=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromBaidu-1.control&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromBaidu-1.control
// EDF为可抢先式调度算法，其调度条件为sum(Ci/Ti) <= 1；
// RMS算法为不可抢先调度算法，其调度条件为sum(Ci/Ti) <= n(exp(ln(2)/n)-1)。

// pthread_create(tid,NULL,func,arg);
// 其中第一个参数是pthread_t型的指针，用于保存线程id；
// 第二个参数是pthread_attr_t的指针，用于说明要创建的线程的属性，NULL表示使用缺省属性；
// 第三个参数指明了线程的入口，是一个只有一个(void *)参数的函数；第四个参数是传给线程入口函数的参数

//为模拟调度算法，给每个线程设置一个等待锁，暂不运行的任务等待在相应的锁变量上。
//主线程按调度算法唤醒一个子线程，被选中线程执行一个时间单位，然后将控制权交给主线程判断是否需要重新调度。

// gcc -lpthread -lm test_scheduler.c -o scheduler.out
// ./scheduler.out

typedef struct{  //实时任务描述，此处的任务都为周期等于截止时间
	char task_id;
	int call_num;  //任务发生次数
	int ci;  // Ci 执行时间
	int ti;  //Ti 周期
	int ci_left;
	int ti_left; 
	int flag;  //任务是否活跃，0否，2是
	int arg;  //参数
	pthread_t th;  //任务对应线程
}task;

void proc(int* args);
void* idle();
int select_proc();
int task_num = 0;
int idle_num = 0;
int alg;  //所选算法，1 for EDF，2 for RMS
int curr_proc=-1;
int demo_time = 100;  //演示时间
task* tasks;
pthread_mutex_t proc_wait[100]; //互斥锁
pthread_mutex_t main_wait, idle_wait;
float sum=0;
pthread_t idle_proc;
FILE *outfile;

int main() //主函数mian()初始化相关数据，创建实时任务并对任务进行调度
{   
	outfile = fopen("char_schedual.txt","w");
	pthread_mutex_init(&main_wait,NULL); //初始化互斥锁
	pthread_mutex_lock(&main_wait);  //下次执行lock等待
	pthread_mutex_init(&idle_wait,NULL);
	pthread_mutex_lock(&idle_wait);  //下次执行lock等待
	FILE *fw=fopen("input.txt","r");
	fscanf(fw,"%d\n%d\n%d\n",&alg,&demo_time,&task_num);
	printf("%d\n%d\n%d\n",alg,demo_time,task_num);
	tasks = (task*)malloc(task_num*sizeof(task));
	int i;
	for(i=0;i<task_num;i++)
	{
		pthread_mutex_init(&proc_wait[i],NULL);
		pthread_mutex_lock(&proc_wait[i]);
	}
	for(i=0;i<task_num;i++)
	{
		fscanf(fw,"%c,%d,%d\n",&tasks[i].task_id,&tasks[i].ci,&tasks[i].ti);
		printf("%c,%d,%d\n",tasks[i].task_id,tasks[i].ci,tasks[i].ti);
		tasks[i].ci_left=tasks[i].ci;
		tasks[i].ti_left=tasks[i].ti;
		tasks[i].flag=2;
		tasks[i].arg=i;
		tasks[i].call_num=1; 
		sum=sum+(float)tasks[i].ci/(float)tasks[i].ti; 
	}
	fclose(fw);
	double r=1;  //EDF算法
	if(alg==2)
	{  //RMS算法
		r=((double)task_num)*(exp(log(2)/(double)task_num)-1);
		printf("r is %lf\n",r);
	}
	if(sum>r)
	{  //不可调度，一旦有任务在队列中就是不能调度
		printf("(sum=%lf > r=%lf) ,not schedulable!\n",sum,r);
		exit(2);
	}

	pthread_create(&idle_proc,NULL,(void*)idle,NULL); //创建闲逛线程
	for(i=0;i<task_num;i++)  //创建实时任务线程
		pthread_create(&tasks[i].th,NULL,(void*)proc,&tasks[i].arg);
	for(i=0;i<demo_time;i++)
	{
		int j; 
		if((curr_proc=select_proc(alg))!=-1)//有可运行任务，返回的就是当前任务
		{  //按调度算法选线程
			pthread_mutex_unlock(&proc_wait[curr_proc]);  //唤醒
			pthread_mutex_lock(&main_wait);  //主线程等待
		}
		else
		{   //无可运行任务，选择闲逛线程
			pthread_mutex_unlock(&idle_wait);  
			pthread_mutex_lock(&main_wait);
		}
		for(j=0;j<task_num;j++)
		{  //Ti--，为0时开始下一周期
			if(--tasks[j].ti_left==0)
			{
				tasks[j].ti_left=tasks[j].ti;
				tasks[j].ci_left=tasks[j].ci;
				pthread_create(&tasks[j].th,NULL,(void*)proc,&tasks[j].arg);
				tasks[j].flag=2;
			}
		}
	}
	fclose(outfile);
	printf("\n");
	// sleep(10); 
}

void proc(int* args)//被选中的任务(temp2)执行 线程的入口
{
	while(tasks[*args].ci_left>0)
	{
		pthread_mutex_lock(&proc_wait[*args]);  //等待被调度（锁上等
		if(idle_num!=0)//给之前的idle做一个总结
		{
			// fprintf(outfile,"idle(%d)",idle_num);
			idle_num=0;
		}
		// fprintf(outfile,"%c%d",tasks[*args].task_id,tasks[*args].call_num);
		fprintf(outfile,"%c",tasks[*args].task_id);
		tasks[*args].ci_left--;  //执行一个时间单位
		if(tasks[*args].ci_left==0)//走完了全程就用(%d)表示
		{
			// printf("(%d)",tasks[*args].ci);
			tasks[*args].flag=0;
			tasks[*args].call_num++;
		}
		pthread_mutex_unlock(&main_wait); //唤醒主线程
	}
};

void* idle()//没有任务时执行idle()
{
	while(1)
	{
		pthread_mutex_lock(&idle_wait);  //等待被调度
		printf("->");  //空耗一个时间单位
		idle_num++;
		pthread_mutex_unlock(&main_wait);  //唤醒主控线程
	}
}

int select_proc(int alg)//调度算法选择线程
{
	int j;
	int temp1,temp2;
	temp1=10000;
	temp2=-1;
	if((alg==2)&&(curr_proc!=-1)&&(tasks[curr_proc].flag!=0))//RMS + 已经在上面判断过 + 运行中
		return curr_proc; 

	for(j=0;j<task_num;j++)
	{
		if(tasks[j].flag==2)//初始化的时候flag就是2
		{
			switch(alg)
			{
				case 1:    //EDF算法
					if(temp1>tasks[j].ci_left)//取到所有task中最小的执行时间，temp2是最小时间的任务序
					{
						temp1=tasks[j].ci_left;
						temp2=j;
					}
				case 2:    //RMS算法
					if(temp1>tasks[j].ti)
					{
						temp1=tasks[j].ti;
						temp2=j;
					}
			}
		}
		// printf("temp1:%d temp2:%d \n",temp1 ,temp2);
	}
	return temp2;
}
