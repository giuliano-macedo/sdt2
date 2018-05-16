#ifndef TABLE

#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <map>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <deque>
#include <algorithm>

#include <armadillo>

typedef struct PhiNode{
	std::mutex* lgarfo;
	std::mutex* rgarfo;

	int sock;
	sockaddr_in* addr;

	PhiNode*next;
	PhiNode*prev;
}PhiNode;

typedef enum QueueTaskType{
	EAT,
	REMOVE
}QueueTaskType;

typedef struct QueueTask{
	QueueTask(QueueTaskType t,PhiNode* p);
	QueueTaskType type;
	PhiNode* node;
}QueueTask;

class Table{
public:
	Table(std::vector<arma::fmat>mats);
	void addPhi(struct sockaddr_in* phiaddr,int sock);
	void removePhi(int sock);
	void eatPhi(int sock);
	void eat(PhiNode* p);
	void remove(PhiNode* p);


	std::vector<arma::fmat>matrices;
	std::mutex matMutex;
	arma::fmat idMatrix;
	uint s,ss;
	std::map<int,PhiNode*> phis;
	std::vector<std::mutex*> garfos;

	
	PhiNode* list;
	std::mutex mtx;
	std::deque<QueueTask> queue;
	std::mutex queueMutex;
private:
	bool isGarfoReserved(PhiNode* phi);
	PhiNode* push();
	void pop(PhiNode*r);
	void monitor();
};
int shuffler(int i);
#endif
#define TABLE