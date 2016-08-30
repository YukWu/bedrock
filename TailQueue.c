#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

struct element{
	int num;
	TAILQ_ENTRY(element) elements;
};

int main(){
	TAILQ_HEAD(tail,element) head;
	TAILQ_INIT(&head);
	struct element* n1 = (struct element*)malloc(sizeof(struct element));
	n1->num = 3;
	struct element* n2 = (struct element*)malloc(sizeof(struct element));
	n2->num = 4;
	struct element* n3 = (struct element*)malloc(sizeof(struct element));
	n3->num = 5;
	struct element* n4 = (struct element*)malloc(sizeof(struct element));
	n4->num = 6;
	TAILQ_INSERT_HEAD(&head,n1,elements);
	TAILQ_INSERT_TAIL(&head,n2,elements);
	/*注意这里两个函数的参数个数不一样*/
	TAILQ_INSERT_AFTER(&head,n1,n3,elements);
	TAILQ_INSERT_BEFORE(n1,n4,elements);
	//遍历
	for(struct element* tp = TAILQ_FIRST(&head);tp!=NULL;tp=TAILQ_NEXT(tp,elements)){
		printf("%d ",tp->num);
	}
	//遍历方法2
	printf("\n");
	struct element* tmp;
	TAILQ_FOREACH(tmp,&head,elements){
		printf("%d ",tmp->num);
	}
	printf("\n");
	//delete
	while(TAILQ_FIRST(&head)){
		TAILQ_REMOVE(&head,TAILQ_FIRST(&head),elements);
	}
	printf("\n");
	return 0;
}
