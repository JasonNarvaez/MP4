/* 
    File: simpleclient.C
    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/01/31
    Simple client main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iomanip> 
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "reqchannel.h"
#include "BoundedBuffer.h"
#include "semaphore.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/
//a long sentence using only c++ keywords:
//if new true friend not protected for explicit private union, break case and try using this

struct RequestThread{
	RequestThread(int n): reqno(n) { }
	int reqno;
};

int n;//amount of requests per person
int b;//size of bounded buffer
int w;//amount of worker threads
BoundedBuffer RequestBuffer;
BoundedBuffer joeBuffer;	//Response buffers
BoundedBuffer janeBuffer;
BoundedBuffer johnBuffer;
vector<int>joeGraph(100);	//Histograms
vector<int>janeGraph(100);	//Response int's 0-99
vector<int>johnGraph(100);

void display(vector<int>v, string name)
{
	cout << endl;
	cout << name << "'s histogram: " << endl;
	for (int i = 0; i < 10; i++)
	{
		int min = i*10;
		int max = min + 9;
		int total = 0;
		for (int j = min; j < max; j++)
			total += v[j];
		
		cout << "[" << min+1 << " - " << max+1 << "]: " << setw(15) << left << total << endl;
	}
	cout << endl;
	cout <<  "-----------------------------------------------------" << endl;
}

void * statThread(void* arg)
{
	int id = *(int*) arg;
	if(id == 0)
			for(int i = 0; i<n;i++)
			{
				string resp = joeBuffer.remove();
				//cout << "JOE RESP: " << resp << endl;
				joeGraph[atoi(resp.c_str())]+=1;
			}
	if(id == 1)
			for(int i = 0; i<n;i++)
			{
				string resp = janeBuffer.remove();
				//cout << "JANE RESP: " << resp << endl;
				janeGraph[atoi(resp.c_str())]+=1;
			}
	if(id == 2)
			for(int i = 0; i<n;i++)
			{
				string resp = johnBuffer.remove();
				//cout << "JOHN RESP: " << resp << endl;
				johnGraph[atoi(resp.c_str())]+=1;	
			}
}

void* RTF(void* arg){
	int ind = *(int*) arg;
	if(ind == 0)
			for(int i = 0; i<n;i++)
			{	//cout<<i<<endl;
				//cout<<endl<<"pushing joe smith"<<endl;
				RequestBuffer.add("data Joe Smith");
			}
	if(ind == 1)
			for(int i = 0; i<n;i++)
			{//cout<<i<<endl;
				//cout<<endl<<"pushing jane smith "<<i<<endl;
				RequestBuffer.add("data Jane Smith");
			}
	if(ind == 2)
			for(int i = 0; i<n;i++)
			{//cout<<i<<endl;
				//cout<<endl<<"pushing john doe "<<i<<endl;
				RequestBuffer.add("data John Doe");
			}
	
}

int identify(string response){
	string joe_smith = "Joe Smith";
	string jane_smith = "Jane Smith";
	string john_doe = "John Doe";
	int found;// = response.find(jane_smith);
	
	if(response.find(joe_smith) != -1)
		found = 0;
	else if(response.find(jane_smith) != -1)
		found = 1;
	else if(response.find(john_doe) != -1)
		found = 2;
	else//not found
		found = -1;
	return found;
	//cout<<found<<endl;
	//if()
	
}

void* WTF(void* arg){
	//int ind = *(int*) arg;
	int ind;//identifies the person. 0 = joe smith, 1 = jane smith, 2 = john doe
	
	
	RequestChannel* rChan = (RequestChannel*) arg;//use the channel from main
	//string ch = rChan.send_request("newthread");
	//cout<<endl<<ch<<endl;
	//RequestChannel* rc = new RequestChannel(ch,RequestChannel::CLIENT_SIDE);
	// cout<<"newthread name: "<<ch<<endl;
	while(true)
	{
		
		string request = RequestBuffer.remove();
		if(request == "Stop"){break;}
		string response = rChan->send_request(request);
		ind = identify(request);
		if(ind == 0)
			joeBuffer.add(response);
		else if(ind == 1)
			janeBuffer.add(response);
		else if(ind == 2)
			johnBuffer.add(response);
	}
	rChan->send_request("quit");
	//RequestChannel* rc = new ReqChan;
	/*
	while(true){
		msg = retrieve from Req_BB
		resp = rc->send(msg)
		//puts resp in the right resp_buffer
	}
	*/
}


int main(int argc, char * argv[]) {

	int opt;
	int get_n, get_b, get_w;//read in values for n,b,w
	while((opt = getopt(argc, argv, "n::b::w::")) != -1){
		
		switch (opt){
			case 'n':{
				if(argv[2]) get_n = atoi(argv[2]);
				else get_n=100000;
				//printf("b: %s \n",argv[2]);
				break;
			}
			case 'b':{
				if(argv[4]) get_b = atoi(argv[4]);
				else get_b=100;
				//printf("b: %s \n",argv[2]);
				break;
			}
			case 'w':{
				if(argv[6]) get_w = atoi(argv[6]);
				else get_w=40;
				//printf("b: %s \n",argv[2]);
				break;
			}
			default:{
				fprintf(stderr, "Usage: %s [-n get_n] [-b get_b] [-w get_w]\n",argv[0]);
				exit(EXIT_FAILURE);
			}
		}	
	}
	printf("using values: n = %d \n b = %d \n w = %d", get_n, get_b, get_w);
	
	pid_t child = fork();
	if (child == 0)
	{	
		execl("./dataserver", "", NULL);
	}  

	else {
		cout << "CLIENT STARTED:" << endl;

		cout << "Establishing control channel... " << flush;
		RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
		cout << "done." << endl;;

	  /* -- Start sending a sequence of requests */
		// pthread_t tid;
		int index[] = {0, 1, 2};
		
		/* -- assigning passed in values -- */
		b = get_b;
		n = get_n;
		w = get_w;
		// w = 40;//number of worker threads
		// n = 10000;//number of requests per person
		// b = 100;//size of bounded buffer
		identify("data Jane Smith");
		
		// -- request threads --
		pthread_t tid1;
		pthread_t tid2;
		pthread_t tid3;
		
		// -- worker threads --
		pthread_t wtid[w];
		
		// -- stat threads --
		pthread_t stid1;
		pthread_t stid2;
		pthread_t stid3;
		
		RequestBuffer = BoundedBuffer(b);//instance of bounded buffer of size b
		
		// -- creating worker threads --
		pthread_create(&tid1,NULL,RTF,&index[0]);
		pthread_create(&tid2,NULL,RTF,&index[1]);
		pthread_create(&tid3,NULL,RTF,&index[2]);

		for(int i=0;i<w;i++){
			string ch = chan.send_request("newthread");
			RequestChannel* newchan = new RequestChannel(ch,RequestChannel::CLIENT_SIDE);
			pthread_create(&wtid[i],NULL,WTF,newchan);
		}
		
		//usleep(2000000);
		pthread_create(&stid1,NULL,statThread,&index[0]);
		pthread_create(&stid2,NULL,statThread,&index[1]);
		pthread_create(&stid3,NULL,statThread,&index[2]);
		
		
		pthread_join(tid1,NULL);
		pthread_join(tid2,NULL);
		pthread_join(tid3,NULL);
		
		for(int i=0;i<w;i++){
			RequestBuffer.add("Stop");
		}
		for(int i=0;i<w;i++){
			pthread_join(wtid[i],NULL);
		}
		
		pthread_join(stid1,NULL);
		pthread_join(stid2,NULL);
		pthread_join(stid3,NULL);
		
		chan.send_request("quit");
		sleep(1);
		display(joeGraph,"joe smith");
		display(janeGraph,"jane smith");
		display(johnGraph,"john doe");
		// for (int i = 0; i<3;i++){
			// pthread_create(&tid[i],NULL,RTF,&index[i]);
		// }
		//pthread_create(&tid, NULL, , );
		
	  /* string reply1 = chan.send_request("hello");
	  cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;
	  string reply2 = chan.send_request("data Joe Smith");
	  cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;
	  string reply3 = chan.send_request("data Jane Smith");
	  cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;
	  string reply5 = chan.send_request("newthread");
	  cout << "Reply to request 'newthread' is " << reply5 << "'" << endl;
	  RequestChannel chan2(reply5, RequestChannel::CLIENT_SIDE);
	  string reply6 = chan2.send_request("data John Doe");
	  cout << "Reply to request 'data John Doe' is '" << reply6 << "'" << endl;
	  string reply7 = chan2.send_request("quit");
	  cout << "Reply to request 'quit' is '" << reply7 << "'" << endl;
	  string reply4 = chan.send_request("quit");
	  cout << "Reply to request 'quit' is '" << reply4 << "'" << endl;
	  */
	  //usleep(1000000); 
	}
}
