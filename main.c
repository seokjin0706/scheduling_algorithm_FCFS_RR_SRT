#include<stdio.h>
#include<stdlib.h>

#define TRUE 1
#define FALSE 0
#define MAX_SIZE 10000 //프로세스 최대 크기
#define SET_ARRIVAL_TIME 2
#define FILE_NAME "input.txt"

typedef struct _process {
	int Process_id; // 프로세스 id
	int priority; // 우선 순위
	int computing_time; // 서비스 시간
	int remain_time; //남아있는 서비스 시간
	int arrival_time; //도착 시간 입력 당 2초라고 가정
	int finish_time; // 종료 시간
	int turn_around_time; //반환 시간
	int time_quantum; // 시간 할당량
	struct process* next;
	struct process* prev;
}process;

/*처리돼야 할 프로세스 큐*/
typedef struct _processQueue {
	process* head;
	process* tail;
	int size;
}processQueue;

typedef struct _priorityQueue {
	process heap[MAX_SIZE];
	int count;
}priorityQueue;

void FCFS();
void RR(int init_time_quantum);
void process_push_back(processQueue* Q, int Process_id, int priority, int computing_time, int remain_time, int arrival_time, int finish_time, int turn_around_time, int time_quantum);
process* process_pop_front(processQueue* Q);
void SRT();
process pop(priorityQueue* pq);
void push(priorityQueue* pq, int Process_id, int priority, int computing_time, int remain_time, int arrival_time, int finish_time, int turn_around_time, int time_quantum);
void swap(process* a, process* b);


int main(void) {
	int flag = 0;
	while (!flag) {
		int init_time_quantum;
		printf("===========================================FCFS, RR scheduling algorithm===========================================\n\n");

		printf("input Time Quantum for Round-Robin : ");

		scanf("%d", &init_time_quantum);
		if (init_time_quantum <= 0) init_time_quantum = 1;
		printf("\n");
		printf("==================================================================================================================\n\n");
		FCFS();
		printf("==================================================================================================================\n\n");
		RR(init_time_quantum);
		printf("==================================================================================================================\n\n");
		SRT();
		printf("==================================================================================================================\n\n");
		printf("type 0 -> reset\n");
		printf("type anything ->  exit\n");
		printf("input: ");
		scanf("%d", &flag);
		
		system("cls");
	}

}

/*큐에 프로세스를 삽입*/
void process_push_back(processQueue* Q, int Process_id, int priority, int computing_time, int remain_time, int arrival_time, int finish_time, int turn_around_time, int time_quantum) {
	process* newProcess = (process*)malloc(sizeof(process));
	if (newProcess == NULL) return;
	newProcess->Process_id = Process_id;
	newProcess->priority = priority;
	newProcess->computing_time = computing_time;
	newProcess->remain_time = remain_time;
	newProcess->arrival_time = arrival_time;
	newProcess->finish_time = finish_time;
	newProcess->turn_around_time = turn_around_time;
	newProcess->time_quantum = time_quantum;
	newProcess->next = NULL;
	newProcess->prev = NULL;
	Q->size++;
	if (Q->head == NULL) {
		Q->head = newProcess;
		Q->tail = newProcess;
	}
	else {
		Q->tail->next = newProcess;
		newProcess->prev = Q->tail;
		Q->tail = newProcess;
	}
}


/*프로세스 하나를 큐에서 제거*/
process* process_pop_front(processQueue* Q) {
	Q->size--;
	if (Q->head == NULL) return NULL;
	process* target = Q->head;
	Q->head = Q->head->next;
	return target;
}

void RR(int init_time_quantum) {
	int type, Process_id, priority, computing_time, arrival_time;
	processQueue temp;
	processQueue Q; //처리 되어야 할 프로세스 큐
	processQueue finished; // 서비스가 끝난 프로세스 큐
	temp.head = NULL; temp.tail = NULL; temp.size = 0;
	Q.head = NULL; Q.tail = NULL; Q.size = 0;
	finished.head = NULL; finished.tail = NULL, finished.size = 0;
	arrival_time = 0; //도착 시간 설정
	FILE* fp = fopen(FILE_NAME, "r");
	while (TRUE) {
		fscanf(fp, "%d", &type);
		if (type == -1) break;
		fscanf(fp, "%d %d %d", &Process_id, &priority, &computing_time);
		if (type == 0) process_push_back(&temp, Process_id, priority, computing_time, computing_time, arrival_time, 0, 0, init_time_quantum);
		arrival_time += SET_ARRIVAL_TIME;
	}
	fclose(fp);
	double average_turn_around_time = 0;
	double normalized_turn_around_time = 0;
	int finish_time = 0;
	process* cur;
	int process_cnt = temp.size;
	process_push_back(&Q, temp.head->Process_id, temp.head->priority, temp.head->computing_time, temp.head->computing_time, temp.head->arrival_time, 0, 0, init_time_quantum);
	process_pop_front(&temp);
	while (Q.size) {

		cur = Q.head;
		int time_quantum = cur->time_quantum;
		process* it = temp.head;
		/*처리*/
		while (cur->remain_time && time_quantum) { //서비스 시간이 남아있으면서 시간할당량이 남아있으면
			cur->remain_time--;
			time_quantum--;
			finish_time++;
			cur->finish_time = finish_time;
			/*프로세스가 들어오면 큐에 넣는다.*/
			
			while (temp.size) {
				if (it->arrival_time <= finish_time) {
					process_push_back(&Q, it->Process_id, it->priority, it->computing_time, it->computing_time, it->arrival_time, 0, 0, init_time_quantum);
					process_pop_front(&temp);
					it = it->next;
				}
				else {
					break;
				}
			}

		}
		if (cur->remain_time == 0) {
			/*서비스 종료*/
			cur->finish_time = finish_time;
			cur->turn_around_time = cur->finish_time - cur->arrival_time;
			average_turn_around_time += cur->turn_around_time / (double)process_cnt;
			normalized_turn_around_time += cur->turn_around_time / (double)cur->computing_time / (double)process_cnt;
			process_push_back(&finished, cur->Process_id, cur->priority, cur->computing_time, cur->remain_time, cur->arrival_time, cur->finish_time, cur->turn_around_time, init_time_quantum);
			process_pop_front(&Q);
		}
		else {//시간 만료 큐에 다시 삽입
			process_push_back(&Q, cur->Process_id, cur->priority, cur->computing_time, cur->remain_time, cur->arrival_time, cur->finish_time, cur->turn_around_time, init_time_quantum);
			process_pop_front(&Q);

		}
		if (Q.size == 0 && temp.size) { //남아있는 도착시간을 기다림
			process_push_back(&Q, it->Process_id, it->priority, it->computing_time, it->computing_time, it->arrival_time, 0, 0, init_time_quantum);
			finish_time = temp.head->arrival_time;
			process_pop_front(&temp);

		}
	}
	cur = finished.head;

	printf("[RR] <Time Quantum: %d>\n\n", init_time_quantum);
	printf("Process_id\tpriority\tcomputing_time\tfinish_time\tturn_around_time\tarrival_time\n");
	while (cur != NULL) {
		printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%d\t\t\n", cur->Process_id, cur->priority, cur->computing_time, cur->finish_time, cur->turn_around_time, cur->arrival_time);
		cur = cur->next;
	}
	printf("average_turn_around_time\tnormalized_turn_around_time\n");
	printf("%.2lf\t\t\t\t%.2lf\n\n", average_turn_around_time, normalized_turn_around_time);
}

void FCFS() {
	int type, Process_id, priority, computing_time, arrival_time;
	processQueue temp;
	processQueue Q; //처리 되어야 할 프로세스 큐
	processQueue finished; // 서비스가 끝난 프로세스 큐
	temp.head = NULL; temp.tail = NULL; temp.size = 0;
	Q.head = NULL; Q.tail = NULL; Q.size = 0;
	finished.head = NULL; finished.tail = NULL, finished.size = 0;
	arrival_time = 0; //도착 시간 설정

	FILE* fp = fopen(FILE_NAME, "r");
	while (!feof(fp)) {
		fscanf(fp, "%d", &type);
		if (type == -1) break;
		fscanf(fp, "%d %d %d", &Process_id, &priority, &computing_time);
		if (type == 0) process_push_back(&temp, Process_id, priority, computing_time, computing_time, arrival_time, 0, 0, 0);
		arrival_time += SET_ARRIVAL_TIME;
	}
	fclose(fp);
	double average_turn_around_time = 0;
	double normalized_turn_around_time = 0;
	int finish_time = 0;
	process* cur;
	int process_cnt = temp.size;
	process_push_back(&Q, temp.head->Process_id, temp.head->priority, temp.head->computing_time, temp.head->computing_time, temp.head->arrival_time, 0, 0, 0);
	process_pop_front(&temp);
	while (Q.size) {
		cur = Q.head;
		while (cur->remain_time) { //서비스 시간이 남아있으면
			cur->remain_time--;
			finish_time++;
		}
		/*프로세스가 들어오면 큐에 넣는다.*/
		process* it = temp.head;
		while (temp.size) {
			if (it->arrival_time <= finish_time) {
				process_push_back(&Q, it->Process_id, it->priority, it->computing_time, it->computing_time, it->arrival_time, 0, 0, 0);
				process_pop_front(&temp);
				it = it->next;
			}
			else {
				break;
			}
		}

		/*서비스 종료*/
		cur->finish_time = finish_time;
		cur->turn_around_time = cur->finish_time - cur->arrival_time;
		average_turn_around_time += cur->turn_around_time / (double)process_cnt;
		normalized_turn_around_time += cur->turn_around_time / (double)cur->computing_time / (double)process_cnt;
		process_push_back(&finished, cur->Process_id, cur->priority, cur->computing_time, cur->remain_time, cur->arrival_time, cur->finish_time, cur->turn_around_time, 0);
		process_pop_front(&Q);
		if (Q.size == 0 && temp.size) { //남아있는 도착시간을 기다림
			process_push_back(&Q, it->Process_id, it->priority, it->computing_time, it->computing_time, it->arrival_time, 0, 0, 0);
			finish_time = temp.head->arrival_time;
			process_pop_front(&temp);

		}
	}
	cur = finished.head;
	printf("[FCFS]\n\n");
	printf("Process_id\tpriority\tcomputing_time\tfinish_time\tturn_around_time\tarrival_time\n");
	while (cur != NULL) {
		printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%d\t\t\n", cur->Process_id, cur->priority, cur->computing_time, cur->finish_time, cur->turn_around_time, cur->arrival_time);
		cur = cur->next;
	}
	printf("average_turn_around_time\tnormalized_turn_around_time\n");
	printf("%.2lf\t\t\t\t%.2lf\n\n", average_turn_around_time, normalized_turn_around_time);

}



void push(priorityQueue* pq, int Process_id, int priority, int computing_time, int remain_time, int arrival_time, int finish_time, int turn_around_time, int time_quantum) {
	if (pq->count >= MAX_SIZE) return;
	process* newProcess = (process*)malloc(sizeof(process));
	if (newProcess == NULL) return;
	newProcess->Process_id = Process_id;
	newProcess->priority = priority;
	newProcess->computing_time = computing_time;
	newProcess->remain_time = remain_time;
	newProcess->arrival_time = arrival_time;
	newProcess->finish_time = finish_time;
	newProcess->turn_around_time = turn_around_time;
	newProcess->time_quantum = time_quantum;
	newProcess->next = NULL;
	newProcess->prev = NULL;
	pq->heap[pq->count] = *newProcess;
	int now = pq->count; //삽입된 데이터에 해당하는 노드의 인덱스
	int parent = (pq->count - 1) / 2; //삽입된 노드의 부모노드
	while (now > 0 && pq->heap[now].remain_time < pq->heap[parent].remain_time) {
		swap(&pq->heap[now], &pq->heap[parent]); //부모노드와 삽입된 노드 교체
		now = parent;
		parent = (parent - 1) / 2;
	}
	pq->count++;
}
process pop(priorityQueue* pq) {
	if (pq->count <= 0) {
		process gar; gar.Process_id = -1;
		return gar;
	}
	process res = pq->heap[0];
	pq->count--;
	pq->heap[0] = pq->heap[pq->count];
	int now = 0, leftChild = 1, rightChild = 2;
	int target = now;

	//원소가 존재할 때 까지만 반복
	while (leftChild < pq->count) {
		if (pq->heap[target].remain_time > pq->heap[leftChild].remain_time) target = leftChild;
		if (pq->heap[target].remain_time > pq->heap[rightChild].remain_time && rightChild < pq->count) target = rightChild;
		if (target == now) break;
		else {
			swap(&pq->heap[now], &pq->heap[target]);
			now = target;
			leftChild = now * 2 + 1;
			rightChild = now * 2 + 2;
		}
	}
	return res; //res
}

void SRT() {
	int type, Process_id, priority, computing_time, arrival_time;
	processQueue temp;
	priorityQueue PQ; //처리 되어야 할 프로세스 큐
	processQueue finished; // 서비스가 끝난 프로세스 큐
	temp.head = NULL; temp.tail = NULL; temp.size = 0;
	PQ.count = 0;
	finished.head = NULL; finished.tail = NULL, finished.size = 0;
	arrival_time = 0; //도착 시간 설정
	FILE* fp = fopen(FILE_NAME, "r");
	while (TRUE) {
		fscanf(fp, "%d", &type);
		if (type == -1) break;
		fscanf(fp, "%d %d %d", &Process_id, &priority, &computing_time);
		if (type == 0) process_push_back(&temp, Process_id, priority, computing_time, computing_time, arrival_time, 0, 0, 0);
		arrival_time += SET_ARRIVAL_TIME;
	}
	fclose(fp);
	double average_turn_around_time = 0;
	double normalized_turn_around_time = 0;
	int finish_time = 0;
	process cur;
	int process_cnt = temp.size;
	push(&PQ, temp.head->Process_id, temp.head->priority, temp.head->computing_time, temp.head->computing_time, temp.head->arrival_time, 0, 0, 0);
	process_pop_front(&temp);
	while (PQ.count) {
		cur = pop(&PQ);
		int flag = TRUE;
		/*처리*/
		process* it = temp.head;
		while (cur.remain_time && flag) { //서비스 시간이 남아있으면서 시간할당량이 남아있으면
			cur.remain_time--;
			finish_time++;
			cur.finish_time = finish_time;

			/*프로세스가 들어오면 큐에 넣는다.*/
		
			while (temp.size) {
				if (it->arrival_time == finish_time) {//프로세스가 도착할 때마다 남은 시간이 가장 적은 프로세스를 선택하기 위한 부분
					push(&PQ, it->Process_id, it->priority, it->computing_time, it->remain_time, it->arrival_time, 0, 0, 0);
					process_pop_front(&temp);
					if (cur.remain_time)push(&PQ, cur.Process_id, cur.priority, cur.computing_time, cur.remain_time, cur.arrival_time, cur.finish_time, cur.turn_around_time, cur.time_quantum);

					it = it->next;
					flag = FALSE;
				}
				else {
					break;
				}
			}

		}
		if (cur.remain_time == 0) {
			/*서비스 종료*/
			cur.finish_time = finish_time;
			cur.turn_around_time = cur.finish_time - cur.arrival_time;
			average_turn_around_time += cur.turn_around_time / (double)process_cnt;
			normalized_turn_around_time += cur.turn_around_time / (double)cur.computing_time / (double)process_cnt;
			process_push_back(&finished, cur.Process_id, cur.priority, cur.computing_time, cur.remain_time, cur.arrival_time, cur.finish_time, cur.turn_around_time, 0);
		}
		if (PQ.count == 0 && temp.size) { //남아있는 도착시간을 기다림
			push(&PQ, it->Process_id, it->priority, it->computing_time, it->remain_time, it->arrival_time, 0, 0, 0);
			finish_time = temp.head->arrival_time;
			process_pop_front(&temp);

		}
	}
	process* iter = finished.head;


	printf("[SRT]\n");
	printf("Process_id\tpriority\tcomputing_time\tfinish_time\tturn_around_time\tarrival_time\n");
	while (iter != NULL) {
		printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%d\t\t\n", iter->Process_id, iter->priority, iter->computing_time, iter->finish_time, iter->turn_around_time, iter->arrival_time);
		iter = iter->next;
	}
	printf("average_turn_around_time\tnormalized_turn_around_time\n");
	printf("%.2lf\t\t\t\t%.2lf\n\n", average_turn_around_time, normalized_turn_around_time);

}


void swap(process* a, process* b) {
	process temp = *a;
	*a = *b;
	*b = temp;

}
