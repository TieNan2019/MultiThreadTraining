#include <stdio.h>
#include <pthread.h>

#include <unistd.h>



static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;



void *newThread(void *args) {
	/* 给 main 函数足够的时间上锁 */
	sleep(2);

	pthread_mutex_lock(&m);
	printf("This information shows before it should have been!\n");

	return NULL;
}


int main(void)
{
	pthread_cond_t c = PTHREAD_COND_INITIALIZER;

	/* 创建新的线程以验证条件变量的运作机制 */
	pthread_t tid;
	pthread_create(&tid, NULL, newThread, NULL);

	/* 上锁 */
	pthread_mutex_lock(&m);

	/* 条件变量进入等待时, 会进行解锁并陷入等待, 直到条件达成 */
	/* 收到信号后, 此处退出等待并重新持有锁 */
	/* 因此 main 函数虽然率先持有锁, 但是加上这句以后, 锁会被释放 */
	/* 同理, 如果注释掉这一行, 则被阻塞的是 newThread 函数 */
	pthread_cond_wait(&c, &m);

	printf("This information shall not display\n");

	return 0;
}
