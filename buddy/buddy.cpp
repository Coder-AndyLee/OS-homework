#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define MEMORY 2048
int available[100], avaiNum = 0, avaiMemory = MEMORY;
int fragment[100], fragNum = 0;

typedef struct Tree{
	struct Tree *left;
	struct Tree *right;
	struct Tree *father;
	int tab;
	int this_memory;
	int if_used;
}TreeNode, *tree;

int AlloMemory(int toBeAllo, tree root)
{
	if(toBeAllo > avaiMemory)
	{
		return 0;
	}
	else if(toBeAllo > root->this_memory)
	{
		return -1;
	}
	else if(toBeAllo > root->this_memory/2)
	{
		if((root->left == NULL) && (root->tab == 0))
		{
			root->tab = 1;
			root->if_used = toBeAllo;
			return 1;
		}
		else
			return -1;
	}
	else if(root->tab == 1)
	{
		return -1;
	}
	else
	{
		if(root->left == NULL)
		{
			root->left = (tree)malloc(sizeof(TreeNode));
			root->left->tab = 0;
			root->left->this_memory = root->this_memory/2;
			root->left->if_used = 0;
			root->left->left = NULL;
			root->left->right = NULL;
			root->left->father = root;

			root->right = (tree)malloc(sizeof(TreeNode));
			root->right->tab = 0;
			root->right->this_memory = root->this_memory/2;
			root->right->if_used = 0;
			root->right->left = NULL;
			root->right->right = NULL;
			root->right->father = root;
		}
		if(AlloMemory(toBeAllo, root->left) != 1)
			return AlloMemory(toBeAllo, root->right);
		else
			return 1;
	}
}

void ReleaseMemory(int toBeRele, tree root)
{
	tree temp;
	if(root != NULL){
		ReleaseMemory(toBeRele, root->left);
		ReleaseMemory(toBeRele, root->right);
		if(root->left == NULL){
			if((root->tab == 0) && (root->father != NULL)){
				if((root->father->left->tab == 0) && (root->father->right->tab == 0) && (root->father->left->left == NULL) && (root->father->right->left == NULL)){
					temp = root->father;
					temp->left = NULL;
					temp->right = NULL;
				}
			}else if((root->tab == 1) && (root->if_used == toBeRele)){
				root->if_used = 0;
				root->tab = 0;
				if((root->father->left->tab == 0) && (root->father->right->tab == 0) && (root->father->left->left == NULL) && (root->father->right->left == NULL)){
					temp = root->father;
					temp->left = NULL;
					temp->right = NULL;
				}
			}
		}
	}
}

void Delete(int a[], int num)//删除数组中第num个字符
{
	int i = num;
	while(a[i] != '\0'){
		a[i] = a[i+1];
		i++;
	}
}

void ScanMemory(tree root)
{
	if(root->left == NULL){
		if(root->tab == 0)
			available[avaiNum++] = root->this_memory;
		else
			fragment[fragNum++] = root->this_memory - root->if_used;
	}else{
		ScanMemory(root->left);
		ScanMemory(root->right);
	}
}

int main()
{
	int times, i, j, n, releNum;
	int allocated, released;
	int label;  //表明分配后的结果
	int haveAllo[256];
	tree init = NULL;

	printf("The size of memory is:%d.\nPlease input the times you will allocate the memory:",MEMORY);
	scanf("%d", &times);
	n = times;
	releNum = 0;

	init = (tree)malloc(sizeof(TreeNode));
	init->tab = 0;
	init->this_memory = MEMORY;
	init->if_used = 0;
	init->left = NULL;
	init->right = NULL;
	init->father = NULL;

	srand(time(NULL));
	for(i = 0; i < times; i++)
	{
		allocated = rand() % 600;
		printf("The size of memory to be allocated : %d\n", allocated);

		label = AlloMemory(allocated, init); 
		ScanMemory(init);
		avaiMemory = avaiMemory - allocated;
		fragment[fragNum] = '\0';
		available[avaiNum] = '\0';

		if(label == 1){
			haveAllo[releNum++] = allocated;
			printf("Allocation succeed!\n");

			j = 0;
			if(available[0] == '\0')
				printf("There is no available memory.");
			else{
				printf("Now,the memory available are:\n");
				while(available[j] != '\0')
					printf("%d  ",available[j++]);
			}
			avaiNum = 0;
			printf("\n");

			j = 0;
			if(fragment[0] == '\0')
				printf("There is no fragment");
			else{
				printf("The framgment are:\n");
				while(fragment[j] != '\0')
					printf("%d  ",fragment[j++]);
			}
			printf("\n");
			fragNum = 0;
		}else if(label == 0){
			n--;
			printf("Allocation failed!Short of memory!\n");
		}else{
			n--;
			printf("Allocation failed!Too many fragment!\n");
		}
		//Sleep(1000);
	}
	haveAllo[i] = '\0';

	printf("\n\n\nNow,release!!\n");
	times = n;
	for(i = 0; i < times; i++){
		releNum = rand() % n;
		n--;
		released = haveAllo[releNum];
		Delete(haveAllo, releNum);
		ReleaseMemory(released, init);
		ScanMemory(init);
		avaiMemory = avaiMemory + released;
		fragment[fragNum] = '\0';
		available[avaiNum] = '\0';

		printf("The memory to be released is: %d\nReleasion succeed!\n", released);

		j = 0;
		if(available[0] == '\0')
			printf("There is no available memory.");
		else{
			printf("Now,the memory available are:\n");
			while(available[j] != '\0')
				printf("%d  ",available[j++]);
		}
		avaiNum = 0;
		printf("\n");

		j = 0;
		if(fragment[0] == '\0')
			printf("There is no fragment");
		else{
			printf("The framgment are:\n");
			while(fragment[j] != '\0')
				printf("%d  ",fragment[j++]);
		}
		printf("\n");
		fragNum = 0;
	}
	system("pause");
}
