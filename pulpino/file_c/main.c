#include "../../rtthread/include/rtthread.h"

/**
 * \brief This is main function.
 * \param void
 * \return 0
*/
int main(void)
{
	printf("This is my main function\n");
	printf("thread name is %s\n",rt_thread_self()->name);
	printf("thread init priority is %d\n",rt_thread_self()->init_priority);
	printf("thread current priority is %d\n",rt_thread_self()->current_priority);

	start_timer();

	while(1)
	{
		sleep();
	}
	return 0;
}