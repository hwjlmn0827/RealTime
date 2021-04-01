# Input

#### example followd
3 //algorithm
20 //bound
3 // task num
T1 1,2,3,6  //Task Model
T2 1,3,12,3
T3 2,4,4,1


switch algorithm 
case 1:    //RMS算法
case 2:    //DMS算法
case 3:    //EDF算法
                   
#### execution
sh start.sh


# RM
任务的周期越短，优先级越高


# RM & EDF
https://medlen.blog.csdn.net/arpicle/details/80772201?utm_medium=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromBaidu-1.control&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromBaidu-1.control

#### EDF为可抢先式调度算法，其调度条件为sum(ei/pi) <= 1；
RMS算法为不可抢先调度算法，其调度条件为sum(ei/pi) <= n(exp(ln(2)/n)-1)。




# LLF
https://blog.csdn.net/weixin_30468137/article/details/95038980?utm_medium=distribute.pc_relevant.none-task-blog-searchFromBaidu-3.control&dist_request_id=&depth_1-utm_source=distribute.pc_relevant.none-task-blog-searchFromBaidu-3.control


# 阅读理解代码

#### 三个初始化互斥锁，立马锁住
pthread_mutex_init(&main_wait,NULL); //初始化互斥锁
    pthread_mutex_init(&idle_wait,NULL);
    pthread_mutex_init(&proc_wait[i],NULL);

#### main_wait 主线程 vs idle_/proc_  只能有一个是醒着的

#### pthread_create

#### pthread_create(pid,NULL,func,arg);
* 第一个参数是pthread_t型的指针，用于保存线程id；
* 第二个参数是pthread_attr_t的指针，用于说明要创建的线程的属性，NULL表示使用缺省属性；
* 第三个参数指明了线程的入口，是一个只有一个(void * )
* 参数的函数；第四个参数是传给线程入口函数的参数
为模拟调度算法，给每个线程设置一个等待锁，暂不运行的任务等待在相应的锁变量上。
* 主线程按调度算法唤醒一个子线程，被选中线程执行一个时间单位，然后将控制权交给主线程判断是否需要重新调度。

#### task线程和idle线程抢占 主线程

#### step

* 初始化的时候，所有的任务的线程都被初始化后，设置为lock在等待
* unlock selected的任务 
* func proc() 中 unlock task 往下执行



3
20
3
T1,1,3,3,0
T2,1,4,4,0
T3,2,5,5,0





# 分析bug
## ps:已经解决，换成pi_left(but why?)
* because > pi 每走一步都会-- (?)

--------------------------------------
3
20
3
T1,1,3,3,0
T2,1,4,4,0
T3,2,5,5,0

### 以下是di_left test data
--------------------------------------
loop j= 0
before  T1 3 2
before  T2 4 2
before  T3 5 2
after select 0

loop j= 1

loop j= 2

⬆️step1
--------------------------------------

loop j= 1
before  T1 2 0
before  T2 4 2
before  T3 5 2
after select 1

loop j= 2

⬆️step2
--------------------------------------

loop j= 2
before  T1 2 0
before  T2 3 0
before  T3 5 2
after select 2

⬆️step3
--------------------------------------

loop j= 0
before  T1 3 2
before  T2 3 0
before  T3 4 2
after select 0

loop j= 2

⬆️step4，问题出在：此时抉择的标准不同:task2的4是基于0，task0的3是基于此时的step
--------------------------------------
......






























