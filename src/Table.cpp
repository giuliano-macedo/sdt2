#include "Table.hpp"
QueueTask::QueueTask(QueueTaskType t,PhiNode* p){
	type=t;
	node=p;
}
Table::Table(std::vector<arma::fmat>mats){
	matrices=mats;
	s=mats[0].n_rows;
	std::random_shuffle(mats.begin(),mats.end(),shuffler);
	ss=s*s;
	idMatrix=arma::fmat(s,s,arma::fill::eye);
	garfos.push_back(new std::mutex());
	list=nullptr;
}
int shuffler(int i){
	static std::default_random_engine rndGen;
	std::uniform_int_distribution<int> rndDis(0,i);
	return rndDis(rndGen);
}
bool Table::isGarfoReserved(PhiNode* phi){
	queueMutex.lock();
	if(queue.empty()){
		queueMutex.unlock();
		return false;
	}
	std::deque<QueueTask>::iterator it = queue.begin();
  	std::mutex*r,*l;
  	l=phi->lgarfo;
  	r=phi->rgarfo;
  	while(it != queue.end()){
		if(l==(*it).node->lgarfo||r==(*it).node->rgarfo)return true;
		it++;
	}
	queueMutex.unlock();
	return false;
}
void Table::monitor(){
	queueMutex.lock();
	if(queue.empty()){
		queueMutex.unlock();
		return;
	}
	std::deque<QueueTask>::iterator it = queue.begin();
	while(it != queue.end()){
		if((*it).node->lgarfo->try_lock()){
			if((*it).node->rgarfo->try_lock()){
				if((*it).type==EAT){
					eat((*it).node);
				}
				else{
					remove((*it).node);
				}
				queue.erase(it);
			}
			else{
				printf("[WARNING] couldnt pick rgarfo in monitor for %s\n",inet_ntoa((*it).node->addr->sin_addr));
				(*it).node->lgarfo->unlock();
				queueMutex.unlock();
				monitor();
				return;
			}
		}
		else{
			printf("[WARNING] couldnt pick lgarfo in monitor for %s\n",inet_ntoa((*it).node->addr->sin_addr));
			queueMutex.unlock();
			monitor();
			return;
		}
		it++;
	}
	queueMutex.unlock();
}
void Table::eat(PhiNode* p){
	matMutex.lock();
	arma::fmat mat=matrices.back();
	matrices.pop_back();
	matMutex.unlock();
	if(send(p->sock,(void*)&s,sizeof(uint),MSG_MORE)!=(int)sizeof(uint)){
		printf("[Error] failed send eat packet header\n");
		removePhi(p->sock);
	}
	else{
		if(send(p->sock,(void*)mat.memptr(),ss*sizeof(float),0)!=(int)(ss*sizeof(float))){
			printf("[Error] failed send eat packet body\n");
			removePhi(p->sock);
		}
		else{
			float*buff=new float[ss];
			if(recv(p->sock,(void*)buff,ss*sizeof(float),0)!=(int)(ss*sizeof(float))){
				printf("[Error] failed receiving invmatrix\n");
				removePhi(p->sock);
			}
			else{
				arma::fmat invmat(buff, s, s, false, true);
				if(!approx_equal(mat*invmat, idMatrix, "absdiff", 0.001f)){
					printf("%s return wrong invmatrix\n",inet_ntoa(p->addr->sin_addr));
					removePhi(p->sock);
				}
				else{
					printf("%s terminated eating\n",inet_ntoa(p->addr->sin_addr));
				}
			}
		}
	}
	matMutex.lock();
	matrices.push_back(mat);
	matMutex.unlock();
	p->rgarfo->unlock();
	p->lgarfo->unlock();
	monitor();
}
void Table::remove(PhiNode* phi){
	mtx.lock();
	garfos.erase(std::find(garfos.begin(),garfos.end(),phi->rgarfo));
	phi->next->lgarfo=phi->lgarfo;

	delete phi->lgarfo;
	pop(phi);
	phis.erase(phi->sock);
	mtx.unlock();
	phi->rgarfo->unlock();
	monitor();
}
void Table::addPhi(struct sockaddr_in* phiaddr,int sock){
	mtx.lock();
	std::mutex*lmtx,*rmtx;
	
	lmtx=garfos.back();

	if(garfos.size()!=1){
		rmtx=garfos.front();
	}
	else{
		garfos.push_back(new std::mutex());
		rmtx=garfos.back();
	}
	PhiNode* re=push();
	re->lgarfo=lmtx;
	re->rgarfo=rmtx;
	re->addr=phiaddr;
	re->sock=sock;
	phis.insert(std::pair<int,PhiNode*>(sock,re));
	printf("phi %s entered with lgarfo:%p rgarfo:%p\n",
		inet_ntoa(phiaddr->sin_addr),lmtx,rmtx);
	mtx.unlock();
}
void Table::removePhi(int sock){
	PhiNode* phi=phis[sock];
	if(!phi)throw std::runtime_error("null pointer to phi object");
	if(isGarfoReserved(phi)){
		queueMutex.lock();
		queue.push_back(QueueTask(REMOVE,phi));
		queueMutex.unlock();
	}
	else{
		if(phi->lgarfo->try_lock()){
			if(phi->rgarfo->try_lock()){
				remove(phi);
			}
			else{
				phi->lgarfo->unlock();
				queueMutex.lock();
				queue.push_back(QueueTask(REMOVE,phi));
				queueMutex.unlock();
			}
		}
		else{
			queueMutex.lock();
			queue.push_back(QueueTask(REMOVE,phi));
			queueMutex.unlock();
		}
	}

}
void Table::eatPhi(int sock){
	PhiNode* phi=phis[sock];
	if(!phi)throw std::runtime_error("null pointer to phi object");
	if(isGarfoReserved(phi)){
		queueMutex.lock();
		queue.push_back(QueueTask(EAT,phi));
		queueMutex.unlock();
	}
	else{
		if(phi->lgarfo->try_lock()){
			if(phi->rgarfo->try_lock()){
				eat(phi);
			}
			else{
				phi->lgarfo->unlock();
				queueMutex.lock();
				queue.push_back(QueueTask(EAT,phi));
				queueMutex.unlock();
			}
		}
		else{
			queueMutex.lock();
			queue.push_back(QueueTask(EAT,phi));
			queueMutex.unlock();
		}
	}
}
PhiNode* Table::push(){//must test
	PhiNode* ans=new PhiNode();
	if(!list){//size 0
		list=ans;
		ans->prev=ans->next=ans;
	}
	else{
		if(list==list->next&&list==list->prev){//size 1
			list->next=list->prev=ans;
			ans->next=ans->prev=list;
		}
		else{//size n
			ans->prev=list->prev;
			ans->next=list;

			list->prev->next=ans;
			list->prev=ans;
		}
	}
	return ans;
}
void Table::pop(PhiNode* which){
	if(!list)throw std::runtime_error("tried to remove from empty list");
	if(list->next==list&&list->prev==list)list=nullptr;
	else{
		if(which==list){
			list=list->next;
		}
		if(list->prev==list->next){
			list->next=list->prev=list;
		}
		else{
			which->prev->next=which->next;
			which->next->prev=which->prev;
		}
	}
	delete which;
}