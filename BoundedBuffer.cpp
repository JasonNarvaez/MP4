#include "BoundedBuffer.h"

void BoundedBuffer::add(string str){//should be able to push something only if there is an empty space on the vector
	//cout<<"adding"<<endl;
	empty.P();//if it is empty, there is no waiting here, since an empty vector should have b slots, so can do b decrements before lock
	//cout<<"mutex is next"<<endl;
	mutex.P();
	//cout<<endl<<"pushing: "<<str<<endl;
	//data.push_back(str);
	data.push(str);
	mutex.V();
	full.V();//since we just put an item on data, we have to increment full by one
	//cout<<"pushed: "<<str<<endl;
}

string BoundedBuffer::remove(){//remove from the data vector
	cout<<"removing"<<endl;
	full.P();//checks if there is something that can be removed, "go" means there is data to be popped
	//cout<<"FREEDOM"<<endl;
	mutex.P();
	//cout<<"free again!"<<endl;
	string s = data.front();
	data.pop();
	//string s = data.back();
	//data.pop_back();
	//cout<<"popped: "<<s<<endl;
	mutex.V();
	empty.V();//since we just took an item off of data, we have to increment empty by one

	return s;

}
