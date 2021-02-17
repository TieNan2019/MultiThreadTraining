#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


static pthread_cond_t c = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

/* 临界资源 */
static int val = 0;



void *newThread(void *args)
{
	/* newThread 为生产者 */

	/* 休眠足够长时间, 让 main 函数先接触临界资源 */
	// sleep(5);

	val++;
	/* 此处设计存在缺陷, 
	 * 如果不存在上一句的等待, 在 main 函数进入等待之前就
	 * 发送信号, 则 main 线程会陷入永久等待 */
	pthread_cond_signal(&c);


	printf("产生临界资源\n");

	return NULL;
}




int main(void)
{
	/* main 函数为消费者 */
	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, newThread, NULL);
	pthread_create(&tid2, NULL, newThread, NULL);

	/* 用于验证错误情况 */
	// sleep(5);		/* 但是实际上不管用…… */

	/* main 函数率先持有锁并检查临界资源 */
	pthread_mutex_lock(&m);


	while (!val)
		/* 注意概念 : 执行到此处 main 函数已经交出锁 */
		/* 阻塞仅仅是因为进入等待队列, 等待信号唤醒 */
		pthread_cond_wait(&c, &m);

	/* 执行到此处, 一个设计健康的并行程序中, main 函数应该再次持有锁 */
	val--;
	printf("利用临界资源\n");

	/* 释放锁 */
	pthread_mutex_unlock(&m);

	

	return 0;
}
