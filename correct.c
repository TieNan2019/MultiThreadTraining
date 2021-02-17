#include <pthread.h>

#include <stdio.h>
#include <unistd.h>


#define LOOPS		5



static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t c = PTHREAD_COND_INITIALIZER;


/* 临界资源 */
static int val = 0;


void *newThread(void *args)
{
	/* new thread 作为生产者 */

	/* 等待 main 函数先进入临界区 */
	sleep(2);

	for (int i = 0; i < LOOPS; i++) {
		/* 虽然目前还没有成功用实验验证 */
		/* 为了避免信号丢失, 先保证 生产过程+发送信号 这个组合行为的原子性 */
		/* 生成的过程中不允许 */
		pthread_mutex_lock(&m);

		/* 生成临界资源 */
		val++;
		printf("生成临界资源\n");

		/* 释放锁 */
		pthread_mutex_unlock(&m);
		/* 令 main 程序退出休眠, 并把锁交给 main */
		pthread_cond_signal(&c);
	}


	return NULL;
}



int main(void)
{
	pthread_t tid;
	pthread_create(&tid, NULL, newThread, NULL);


	for (int i = 0; i < LOOPS; i++) {
		/* 当读取数据时, 不应该有其他线程也在处理该临界区 */
		pthread_mutex_lock(&m);
		while (!val)
			/* 等待信号 : 此时 main 函数已经释放锁 */
			pthread_cond_wait(&c, &m);

		/* 进入临界区, 此时 main 退出休眠, 再次持有锁 */
		val--;
		printf("消耗临界资源\n");
		sleep(2);

		/* 释放锁, 供其他线程操作临界区 */
		pthread_mutex_unlock(&m);
	}


	return 0;
}
