/*
 sys/queue.h中各种链表的使用例子
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

//双向链表例子

/*定义链表节点*/
struct element{
	int num;
	LIST_ENTRY(element) elements;
	/*LIST_ENTRY 创建一个以element为名的结构体
	 定义这个结构体含有向后指针le_next和向前指针le_prev
	 并声明一个类型为element的变量elements
	 显然，这个sys/queue内的链表结构体是入侵式的
	 */
};

int main(){
	/*
	 LIST_HEAD 会自动构建一个名为listh的结构体，但是这个结构体是head的类型。
	 但是一般不会用到
	 里面含有一个element指针 *lh_first
	 所以，必须先调用LIST_HEAD
	 */
	LIST_HEAD(listh,element) head;
	struct element *n1,*np,*n2,*n3;
	LIST_INIT(&head);
	n1 = (struct element*)malloc(sizeof(struct element));
	n1->num = 3;
	n2 = (struct element*)malloc(sizeof(struct element));
	n2->num = 4;
	n3 = (struct element*)malloc(sizeof(struct element));
	n3->num = 5;
	/*
	 对于LIST_INSERT_HEAD为何需要指针域作为参数？
	 因为指针域的名字是用户自己定义的，函数并不知晓
	 所以需要作为参数传入。
	 */
	LIST_INSERT_HEAD(&head,n1,elements); //在头部插入
	LIST_INSERT_AFTER(n1,n2,elements);  //插入在n1后面
	LIST_INSERT_BEFORE(n1,n3,elements); //插入在n1前面
	LIST_REMOVE(n1,elements); //删除节点


	for(np = LIST_FIRST(&head);np!=NULL;np=LIST_NEXT(np,elements)){
		printf("%d ",np->num);
	}
	printf("\n");
	while(head.lh_first!=NULL){
		struct element* tmp = head.lh_first;
		LIST_REMOVE(head.lh_first,elements);
		free(tmp);
	}
	return 0;


}
