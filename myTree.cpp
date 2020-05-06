#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
using namespace std;
class node{
	int min;//min number of children possible for non root
	int *keys;
	int n;
	node **children;
	bool isLeaf;
public:
	node(int i, bool l);
	void traverseInOrder();
	void traversePreOrder();
	node *search(int val);
	int getIndex(int val);
	void insertInNode(int val);
	void splitChild(int index, node* c);
	void deleteKey(int val);
	void prepare(int i);
	void add(int i);
	void clockwise(int i);
	void anticlockwise(int i);
	void merge(int i);
friend class tree;
};

class tree{
	node *root;
	int min; //minimum number of children possible
public:
	tree(int m){
		root = NULL;
		min = m;
	}
	void traverseInOrder(){
		if (root != NULL) root->traverseInOrder();
	}
	void traversePreOrder(){
		if (root!=NULL) root->traversePreOrder();
 	}
	node* search(int val){
		if (root == NULL) return NULL;
		if(root->search(val))	cout<<"found element "<<val<<endl;
		else cout<<"didn't find element "<<val<<endl;
		return root->search(val);
	}
	void insert(int val);
	void deleteKey(int val);
};

node::node(int m,bool l){
	min = m;
	isLeaf = l;
	keys = new int[2*min - 1];
	children = new node* [2*min];
	n = 0;
}
int node::getIndex(int val){
	int i;//find least index of key such that keys[i] >= val
	for(i = 0; (i < n && keys[i] < val); i++);
	return i;
}
void node::traverseInOrder(){
	for (int i = 0; i < n; i++)	{
		if(!isLeaf) children[i]->traverseInOrder();
		cout << " " << keys[i];
	}
	if(!isLeaf)
		children[n]->traverseInOrder();
}
void node::traversePreOrder(){
	for (int i = 0; i < n; i++)
		cout << " " << keys[i];
	if(!isLeaf)
	for (int i = 0; i < n+1; i++)
		children[i]->traversePreOrder();
}
node* node::search(int val){
	int i = this->getIndex(val);//keys[i]>=val
	//found key in this node itself
	if(keys[i]==val) {
		return this;
	}
	//else search in a child
	if(isLeaf) return NULL;
	return children[i]->search(val);
}
void node::insertInNode(int val){
	int i = getIndex(val);
	//found right index to place val
	if(isLeaf){
		//start from end, shift keys till we get to point to place val
		for (int j = n; j > i; j--)	keys[j] =  keys[j-1];
		keys[i] = val;
		n++;
		return;
	}//node is an intermediate node
	if(children[i]->n == 2*min - 1){//split child
		splitChild(i,children[i]);
		if(keys[i]<val)	i++;//if new child is made check where val will go
	}//if all were less than full, put val in child[i]
	children[i]->insertInNode(val);
}
void node::splitChild(int i, node* x){
	//x has 2*min-1 nodes.
	//right min-1 go to temp, 1 to new node
	node *temp = new node(min,x->isLeaf);
	temp->n = min-1;
	//temp contains min - 1 nodes as a new child
	//copying keys from x to temp
	for (int j = 0; j < min; j++){
	    if (j < min-1) temp->keys[j] = x->keys[j+min];
	    if(!isLeaf) temp->children[j] = x->children[j+min];
	}
  x->n = min-1;
  //shifting righter children
  for (int j = n; j > i; j--)
  	children[j+1] = children[j];
  children[i+1] = temp;
  //shifting keys greater than new key
  for (int j = n-1; j > i; j--)
  	keys[j+1] = keys[j];
  keys[i] = x->keys[min-1];//update new node.
  n ++;
}
void tree::insert(int val){
	if(!root){//root was empty
		root = new node(min,true);
		root->keys[0] = val;
		root->n = 1;
		return;
	}
	if (root->n == 2*min-1){//root is full. split it and insert in child
		node* newRoot = new node(min,false);
		newRoot->children[0] = root;
		newRoot->splitChild(0,root);
		//tree structure good. insert in child
		if ( val > newRoot->keys[0])	newRoot->children[1]->insertInNode(val);
		else newRoot->children[0]->insertInNode(val);
		root = newRoot;		//make newRoot as new root
	}
	else root->insertInNode(val);
}
void tree::deleteKey(int val){
	if(!root) return;
	root->deleteKey(val);
	//now on deletion, root might become empty.
	if(root->n==0){
		node* temp = root;
		if(root->isLeaf) root = NULL;
		else root = root->children[0];
		delete(temp);
	}
	return;
}
void node::deleteKey(int val){
	//find element in leaf or non leaf node and deleteKey
	int i = getIndex(val);
	if(i<n && keys[i]==val){//element to be deleteKeyd is here in this node
		cout<<"deleted "<<val<<" from tree"<<endl;
		if(isLeaf) {
			//shift all keys back
			for(int j = i; j < n-1; j++) keys[j] = keys[j+1];
			n--;//reduce size of leaf [it still has n keys tho]
		}
		else prepare(i);
		return;
	}
	if(isLeaf) {//element not present in this node and this is leaf.
		cout<<"element "<<val<<" is not in tree"<<endl;
		return;
	}	// we want the node from which we remove to have atleast min nodes.
	// so we add the nodes to make not empty as we go down and they are size >= min
 	if(children[i]->n<min) add(i);
	if(i>n) children[i-1]->deleteKey(val);
	else children[i]->deleteKey(val);
	return;
}
void node::prepare(int i){
	//check in children of both sides and get inorder suc or pred
	if(children[i]->n > min-1){
		cout<<"prepared from left"<<endl;
		node *ret = children[i];//keep going to righmost children till leaf
		while(!ret->isLeaf) ret = ret->children[ret->n];
		keys[i] = ret->keys[ret->n-1];
		children[i]->deleteKey(keys[i]);
	}
	else if(children[i+1]->n > min-1){
		node *ret = children[i+1];//keep going to leftmost children till leaf
		while(!ret->isLeaf) ret = ret->children[0];
		keys[i] = ret->keys[0];
		children[i+1]->deleteKey(keys[i]);
	}//now no child has enough elements. So Merge
	else{
		merge(i);
		children[i]->deleteKey(keys[i]);
	}
	return;
}
void node::add(int i){
	if(i!=0 && children[i-1]->n > min-1) clockwise(i);
	else if(i!=n && children[i+1]->n > min-1) anticlockwise(i);
	else merge(i);
}
void node::clockwise(int i){
	node* giver = children[i-1];//giver gives element for parent
	node* taker = children[i];//parent is taken by taker
	for(int j = taker->n - 1; j > -2; j--){//making space for parent
		if ( j >= 0 ) taker->keys[j+1] = taker->keys[j];
		if ( !taker->isLeaf ) taker->children[j+2] = taker->children[j+1];
	}
	taker->keys[0] = keys[i-1];//rotating keys
	keys[i-1] = giver->keys[giver->n-1];
	if(!taker->isLeaf) taker->children[0] = giver->children[giver->n];//child transfer
	taker->n++;//adjusting number of keys
	giver->n--;
	return;
}
void node::anticlockwise(int i){
	node* giver = children[i+1];
	node* taker = children[i];
	taker->keys[taker->n] = keys[i];
	keys[i] = giver->keys[0];
	if(!giver->isLeaf) taker->children[taker->n+1] = giver->children[0];
	for(int j = 0; j < giver->n-1; j++){
		giver->keys[j] = giver->keys[j+1];
		if(!giver->isLeaf) giver->children[j] = giver-> children[j+1];
	}
	giver->n--;
	taker->n++;
	return;
}
void node::merge(int i){
	node* left = children[i];
	node* right = children[i+1];
	left->keys[min-1] = keys[i];//add key of parent to left
	for( int j = 0;  j < right->n; j++){//add keys and children from right to left
		left->keys[min+j] = right->keys[j];
		if(!left->isLeaf) left->children[min+j] = right->children[j];
	}
	for ( int j = i ; j < n - 1 ; j++){//shift righter elements in parent node
		keys[j] = keys[j+1];
		children[j+1] = children[j+2];
	}
	left->n += right->n + 1;
	n--;
}
int main(){
	int min;
	cout<<"give minimum degree : ";
	cin>>min;
	tree t(min);
	while(1){
		int choice, val;
		cout<<"choose: ";
		cout<<"1. TraverseInOrder, 2.TraversePreOrder"<<endl;
		cout<<"3. Insert, 4. Delete, 5. Search"<<endl;
		cin>>choice;
		if(choice==3 || choice ==4||choice==5){
			cout<<"element = ";
			cin>>val;
		}
		if(choice==1) {t.traverseInOrder();cout<<endl;}
		else if(choice==2) {t.traversePreOrder();cout<<endl;}
		else if(choice==3) t.insert(val);
		else if(choice==4) t.deleteKey(val);
		else if(choice==5) t.search(val);
		else exit(1);
	}
//FOLLOWING IS THE CODE USED TO MEASURE INSERTION TIME
	// int x;
	// ifstream inFile;
	// inFile.open("insert.txt");
	// if (!inFile) {
	//     cout << "Unable to open file to read. Closed file after writing?";
	//     exit(1); // terminate with error
	// }
	// time_t start = time(NULL);
	// for (int i = 0; i<1000000; i++ )	{
	// 	if(i%1000000==0) cout<<i/1000000<<endl;
	// 	t.insert(x);
	// }
	// time_t curr = time(NULL);
	// inFile.close();
	// cout<<"that took "<<curr-start<<" seconds"<<endl;
	// exit(1);
	// return 0;
}
