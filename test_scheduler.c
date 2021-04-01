#include "math.h"
#include "sched.h"
#include "pthread.h"
#include "stdlib.h"
#include "semaphore.h" 
#include "stdio.h"
#include "unistd.h"
#include "time.h"



typedef struct{  //实时任务描述，此处的任务都为周期等于截止时间
	char task_id[4];
	int call_num;  //任务发生次数
	int ei;  // ei 执行时间
	int di;  //new add 延迟时间
	int pi;  //pi 周期
	int ri;  //new add release time
	int ei_left;  //剩下的执行时间
	int di_left;
	int pi_left; 
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
    clock_t start, finish;
    start = clock();
    double cost;  
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
		fscanf(fw,"%s",tasks[i].task_id);
		fscanf(fw,"%d,%d,%d,%d\n",&tasks[i].ei,&tasks[i].di,&tasks[i].pi,&tasks[i].ri);
		printf("%s,%d,%d,%d,%d\n",tasks[i].task_id,tasks[i].ei,tasks[i].di,tasks[i].pi,tasks[i].ri);
		tasks[i].ei_left=tasks[i].ei;
		tasks[i].di_left=tasks[i].di;
		tasks[i].pi_left=tasks[i].pi;
		tasks[i].flag=2;//初始化为活跃的
		tasks[i].arg=i;
		tasks[i].call_num=1; 
		sum=sum+(float)tasks[i].ei/(float)tasks[i].pi; 
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
		pthread_create(&tasks[i].th,NULL,(void*)proc,&tasks[i].arg);//触发第一次proc任务
	for(i=0;i<demo_time;i++)
	{
		if((curr_proc=select_proc(alg,i))!=-1)//有可运行任务，返回的就是当前任务
		{  //按调度算法选线程
			pthread_mutex_unlock(&proc_wait[curr_proc]);  //唤醒
			pthread_mutex_lock(&main_wait);  //主线程等待
		}
		else
		{  //无可运行任务，选择闲逛线程
			pthread_mutex_unlock(&idle_wait);  
			pthread_mutex_lock(&main_wait);
		}
		int j; 
		for(j=0;j<task_num;j++)//为什么要无差别周期减?每个task的周期left都要-1
		{  //pi--，为0时开始下一周期
			if (i>=tasks[j].ri)
			{
				if(--tasks[j].pi_left==0)
				{
					tasks[j].pi_left=tasks[j].pi;
					tasks[j].di_left=tasks[j].di;
					tasks[j].ei_left=tasks[j].ei;
					pthread_create(&tasks[j].th,NULL,(void*)proc,&tasks[j].arg);//再次触发
					tasks[j].flag=2;
				}
			}
		}

	}
	fclose(outfile);
	finish = clock();
	cost=(double)(finish - start);  
	printf("\n diff_time %f\n", cost);
	printf("\n");
	// sleep(10); 
}

void proc(int* args)//被选中的任务(temp2)执行 线程的入口
{
	printf("in proc %s %d %d\n",tasks[*args].task_id, tasks[*args].ei_left,tasks[*args].di_left );
	while(tasks[*args].ei_left>0&&tasks[*args].di_left>0) //是while，那么意思是不可抢占吗？
	{
		pthread_mutex_lock(&proc_wait[*args]);  //等待被调度（锁上等
		if(idle_num!=0)//给之前的idle做一个总结
		{
			// fprintf(outfile,"idle(%d)",idle_num);
			idle_num=0;
		}
		// fprintf(outfile,"%s%d",tasks[*args].task_id,tasks[*args].call_num);
		fprintf(outfile,"%s,",tasks[*args].task_id);
		tasks[*args].ei_left--;  //执行一个时间单位
		tasks[*args].di_left--;  //执行一个时间单位
		if(tasks[*args].ei_left==0)//走完了全程就用(e)表示
		{
			// printf("(%d)",tasks[*args].ei);
			tasks[*args].flag=0;//执行完了以后flag变为0
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
		fprintf(outfile,"-,");
		// printf("-");  //空耗一个时间单位
		idle_num++;
		pthread_mutex_unlock(&main_wait);  //唤醒主控线程
	}
}

int select_proc(int alg, int step)//调度算法选择线程
{
	int j;
	int temp1,temp2;
	temp1=10000;
	temp2=-1;
	//以下一段表示可抢占
	// if((alg==2)&&(curr_proc!=-1)&&(tasks[curr_proc].flag!=0))//RMS + 已经在上面判断过 + 运行中
	// {
	// 	printf("prompt \n");
	// 	return curr_proc; 
	// }
	for(j=0;j<task_num;j++)//遍历所有的空闲任务
	{
		if(tasks[j].flag==2&&step>=tasks[j].ri)//初始化的时候flag就是2 意思是目前j空闲
		{
			printf("\nloop j=%d step =%d \n", j,step);//值得判断的task[j] 主要看flag是否符合
			switch(alg)
			{
				case 1:    //RMS算法
					if(temp1>tasks[j].pi)//取更小的周期
					{
						temp1=tasks[j].pi;
						temp2=j;//解释了为什么是0开始，temp2为当前的步数吧
					}
				case 2:    //DMS算法,因为每个任务的di是确定的
					if(temp1>tasks[j].di)
					{
						temp1=tasks[j].di;
						temp2=j;
					}
				case 3:    //EDF算法
					if(temp1>tasks[j].pi_left)//取到所有task中最小的执行时间，temp2是最小时间的任务数
					{
						printf("before  %s %d %d\n", tasks[0].task_id, tasks[0].pi_left, tasks[0].flag);
						printf("before  %s %d %d\n", tasks[1].task_id, tasks[1].pi_left, tasks[1].flag);
						printf("before  %s %d %d\n", tasks[2].task_id, tasks[2].pi_left, tasks[2].flag);
						temp1=tasks[j].pi_left;
						temp2=j;
					}
					printf("after select %d\n",temp2);
					// if(temp1>tasks[j].di_left)//取到所有task中最小的执行时间，temp2是最小时间的任务数
					// {
					// 	printf("before  %s %d %d\n", tasks[0].task_id, tasks[0].di_left, tasks[0].flag);
					// 	printf("before  %s %d %d\n", tasks[1].task_id, tasks[1].di_left, tasks[1].flag);
					// 	printf("before  %s %d %d\n", tasks[2].task_id, tasks[2].di_left, tasks[2].flag);
					// 	temp1=tasks[j].di_left;
					// 	temp2=j;
					// 	printf("after select %d\n",temp2);
					// }
			}
		}
	}
	return temp2; //选中的task序号
}
