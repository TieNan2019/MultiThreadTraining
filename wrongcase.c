#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


#define LOOPS	5


static pthread_cond_t c = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

/* 临界资源 */
static int val = 0;



void *newThread(void *args)
{
	/* newThread 为生产者 */


	for (int i = 0; i < LOOPS; i++) {
		val++;
		/* 此处设计存在缺陷, 
	 	* 如果不存在上一句的等待, 在 main 函数进入等待之前就
	 	* 发送信号, 则 main 线程会陷入永久等待 */
		pthread_cond_signal(&c);

		printf("产生临界资源\n");
	}

	return NULL;
}




int main(void)
{
	/* main 函数为消费者 */
	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, newThread, NULL);
	pthread_create(&tid2, NULL, newThread, NULL);



	for (int i = 0; i < LOOPS; i++) {
		/* main 函数持有锁并检查临界资源 */
		pthread_mutex_lock(&m);


		while (!val) {
			/* 将临界条件和等待的粒度放大 */
			/* 使得 wait 可以成功错过唤醒信号 */
			sleep(2);

			/* 到达此处时, newThread 的唤醒信号均已经被错过 */

			/* 注意概念 : 执行到此处 main 函数已经交出锁 */
			/* 阻塞仅仅是因为进入等待队列, 等待信号唤醒 */
			/* 没有后续的唤醒信号的话, 将会永远在此处等待 */
			/* 虽然实际上可以通过后续信号唤醒, 但是已经减小了并发量 */
			pthread_cond_wait(&c, &m);
		}

		/* 执行到此处, 一个设计健康的并行程序中, main 函数应该再次持有锁 */
		val--;
		printf("利用临界资源\n");

		/* 释放锁 */
		pthread_mutex_unlock(&m);
	}

	

	return 0;
}
