#include "semaphore.h"

int Semaphore::P(){
	
	int cond;
	if(cond = pthread_mutex_lock(&m) != 0) 
		return cond;
	while(value<=0)
		if(pthread_cond_wait(&c, &m) != 0) 
			return cond;
	--value;
	if(cond = pthread_mutex_unlock(&m)!=0) 
		return cond;
	return 0;
	
}

int Semaphore::V(){
	
	int cond;
	if (cond = pthread_mutex_lock(&m)!=0)
       return cond;
  		++value;
  	if (cond = pthread_cond_broadcast(&c)!=0)
       return cond;
  	if (cond = pthread_mutex_unlock(&m)!=0)
       return cond;
	 return 0;
	
}
