# MP4
This program now creates multiple worker threads to handle requests sent to the dataserver. We use the Fork() command to run the dataserver. We then create multiple process threads for the worker threads and stat threads. We use semaphores to protect against race conditions involved with multiple processes acessing the request buffer. This program now keeps track of the responses received from the dataserver (ranging from 0-99)

Execute client with:

./Client -n [request] -b [buffer size] -w [worker threads]
