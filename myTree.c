#include <stdlib.h>
#include <stdio.h>

#define max 4
#define min max/2

struct node* newNode(int val, struct node* c);
void search(int val, struct node *n);
void printAll(struct node* n, int order);
void split(int val, int *pval, int i, struct node *n, struct node *child, struct node **newNode);
int chooseNode(int val, int *pval, struct node *n, struct node **c);	
void addValue(int val, int i, struct node *n, struct node *child);
void insert(int val);

struct node{
	int numValues;
	int values[max+1];//values start from index 1 to numValues
	struct node* children[max+1]; //children[0] is parent
};

struct node* root;

struct node* newNode(int val, struct node* c){
	struct node* n;
	n = (struct node* )malloc(sizeof(struct node));
	n->values[1] = val;
	n->children[0] = root;
	n->children[1] = c;
	n->numValues = 1;
	return n;
}

void search(int val, struct node *n){
	int size = n->numValues;//size is number of children-1
	int i;
	if (!n) {
		return;//if node is empty, return 
	}
	if (val < n->values[1])//value is less than leftmost element
		i = 0;//got to parent and search
	else {//find the index i where value is present
		i = size;
		while(val < n->values[i] && i) i--;
		//i is index next to which we have to go down
		if (val == n->values[i])
		{//element with same value found. 
			printf("Element with value %d is present in tree\n",val);
			return;
		}
		
	}//now i is the index of children list in which value can be found	
	search(val, n->children[i]);
	return;
}

void printAll(struct node* n, int order) {
	switch (order){
		case 1:
   			if (n) {
   				int i;
   		    	for (i = 0; i < n->numValues; i++) {
            		printAll(n->children[i],order);
            		printf("%d ", n->values[i+1]); 
            		//first element is values[1]
        		}
        		printAll( n->children[i] ,order);
    		}
    		break;
    	case 2:
    		if (n) {
   				int i;
   		    	for (i = 0; i < n->numValues; i++) {
            		printf("%d ", n->values[i+1]); 
            		//first element is values[1]
        		}
        		for (int j = 0; j < i + 1; j++) {
            		printAll(n->children[j],order); 
            		//first element is values[1]
        		}
        	}
    		break;
    	default:
    		printf("Unknown Choice\n");
	}
	
}

void split(int val, int *pval, int i, struct node *n,
	struct node *child, struct node **newNode){
	int mid, temp;

	if ( i > mid ) mid = min + 1;//mid element to be pushed up
	else mid = min;

	*newNode = (struct node *)malloc(sizeof(struct node));
	temp = mid + 1;
	while(temp <= max){
		(*newNode)->values[temp-mid] = n->values[temp];
		(*newNode)->children[temp-mid] = n->children[temp];
		temp++;
	}
	n->numValues = mid;
	(*newNode)->numValues = max - mid;

	if (i <= min) addValue(val,i,n,child);
	else addValue(val,i-mid,*newNode,child);

	*pval = n->values[n->numValues];
	(*newNode)->children[0] = n->children[n->numValues];
	n->numValues --;
}

void addValue(int val, int i, struct node *n, struct node *c){
	int temp = n->numValues;
	while(temp > i){
		n->values[temp+1] = n->values[temp];
		n->children[temp+1] = n->children[temp];
		temp--;
	}
	n->values[temp+1] = val;
	n->children[temp+1] = c;
	n->numValues ++;
}

int chooseNode(int val, int *pval, struct node *n, struct node **c){
	int i;
	if(!n){//node is empty so make a new root
		*pval = val;
		*c = NULL;
		return 1;
	}

	if (val < n->values[1]) {
		i = 0;//insert in leftmost child
	} 
	else {//find the child where it should be inserted
		for (i = n->numValues; (val < n->values[i] && i > 1); i--){
			if (val == n->values[i]){//if already present
				printf("element already present\n");
				return 0;
			}
		}
	}
	printf("%d\n", i);
	if (chooseNode(val,pval,n->children[i],c)){
		if (n->numValues < max) addValue(*pval,i,n,*c);//no problem
		else {//overflow detected split node
			split(*pval,pval,i,n,*c,c);
			return 1;
		}
	}

	return 0;
}

void insert(int val){
	int changeRoot, i;
	struct node* child;

	changeRoot = chooseNode(val,&i,root,&child);
	if (changeRoot){
		root = newNode(i,child);
	}
}

void getNextInOrder(struct node* n, int i){
	struct node* temp;
	temp = n->children[i];
	while(temp->children[0] != NULL) temp = temp->children[0];
	n->values[i] = temp->values[1];
}

void borrowFromRight(struct node* n, int i){
	struct node *temp= n->children[i];
	int j = temp->numValues;

	while (j > 0) {
		temp->values[j + 1] = temp->values[j];
		temp->children[j + 1] = temp->children[j];
	}
	temp->values[1] = n->values[i];
	temp->children[1] = temp->children[0];
	temp->numValues++;

	temp = n->children[i - 1];
	n->values[i] = temp->values[temp->numValues];
	n->children[i] = temp->children[temp->numValues];
	temp->numValues--;
	return;
}

void borrowFromLeft(struct node* n, int i){
	struct node *temp = n->children[i - 1];
	int j = 1;

	temp->numValues++;
	temp->values[temp->numValues] = n->values[i];
	temp->children[temp->numValues] = n->children[i]->children[0];

	temp = n->children[i];
	n->values[i] = temp->values[1];
	temp->children[0] = temp->children[1];
	temp->numValues--;

	while (j <= temp->numValues) {
		temp->values[j] = temp->values[j + 1];
		temp->children[j] = temp->children[j + 1];
		j++;
	}
	return;
}

void removeFromNode(struct node* n, int i){
	for(int temp = i+1; temp <= n->numValues ; temp++){
		n->values[temp-1] = n->values[temp];
		n->children[temp-1] = n->children[temp];
	}
	n->numValues--;
}

void merge(struct node* n, int i){
	int temp = 1;
	struct node* r = n->children[i];//right child
	struct node* l = n-> children[i-1];//left child

	l->numValues ++;
	l->values[l->numValues] = n->values[i];
	l->children[l->numValues] = n->children[0];

	while(temp <= r->numValues){
		l->numValues ++;
		l->values[l->numValues] = r->values[temp];
		l->children[l->numValues] = r->children[temp];
		temp++;	
	}
	temp = i;
	while(temp < n->numValues){
		n->values[temp] = n->values[temp+1];
		n-> children[temp] = n-> children[temp+1];
		temp++;
	}
	n->numValues--;
	free(r);
}

void borrowOrMerge(struct node *n, int i){
	if(!i){//leftmost child is deleted from
		if(n->children[1]->numValues > min) borrowFromLeft(n,1);
		else merge(n,1); 
	}
	else if (n->numValues==i){//rightmost child is deleted from
		if(n->children[i-1]->numValues > min) borrowFromRight(n,i);
		else merge(n,i);
	}
	//intermediate child is deleted from
	else {//check is a sibling can lend an element
		if(n->children[i-1]->numValues > min) borrowFromRight(n,i);
		else if(n->children[i+1]->numValues > min) borrowFromLeft(n,i+1);
		else merge(n,i);//no sibling can lend
	}
}

int elementFound(int val, struct node* n){
	int i;//index i of element equal to val or of child to search in
	if(!n) {
		printf("node empty line 266");
		return 0;
	}
	int found=0;
	if (val<n->values[1]){
		i=0;//search in leftmost child
	}
	else {//search for child index to search in
		i=n->numValues;
		while( val<n->values[i] && i>1 ) i--;
		// i is index to go down or check if element=val
		if(val==n->values[i]) found = 1;
	}

	if(found){//search and delete
		if(n->children[i-1]){
			getNextInOrder(n,i);
			found = elementFound(n->values[i],n->children[i]);
			if(!found) printf("Element Not Found\n"); 
		}
		else removeFromNode(n,i);
	}

	if(n->children[i]){//adust after deletion
		if(n->children[i]->numValues < min) borrowOrMerge(n,i);
	}

	printf("found = %d\n", found);	
	return found;
}

void delete(int val, struct node* n){
	struct node* temp;
	if (!elementFound(val,n)){
		printf("Not found line 298\n");
		printf("Element not found\n");
		return;		
	}
	else if (n->numValues==0){
		temp = n;
		n = n-> children[0];
		//pointer to node n now points to its parent
		free(temp);
	}
	root = n;
	return;
}
int main(){	
	int val, choice;
	while(1){
		printf("printAll : 1\n");
		printf("Insert : 2\n");
		printf("Delete : 3\n");
		printf("Search : 4\n");
		printf("Enter Your Choice : ");
		scanf("%d",&choice);
		switch(choice){
			case 1:
				printf("InOrder: 1\nPreOrder:2\n");
				int order;
				scanf("%d",&order);
				printAll(root,order);
				printf("\n");
				break;
			case 2: 
				printf("Enter Value To Insert : ");
				scanf("%d",&val);
				insert(val);
				break;
			case 3:
	            printf("Enter the value to delete : ");
	            scanf("%d", &val);
	            delete(val, root);
	            break;
	        case 4:
	            printf("Enter the value to search : ");
	            scanf("%d", &val);
	            search(val, root);
	            break;
	        default:
	        	printf("unknown choice");
	    }
	}
	return 0;
}