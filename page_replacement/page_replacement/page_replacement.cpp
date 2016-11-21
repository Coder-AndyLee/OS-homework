#include<stdio.h> 
#include<stdlib.h> 
#include<time.h>
#define INVALID -1 
#define VALID -2
#define total_instruction 2048   //ָ������
#define total_vp 32              //���ҳ��
#define MAX  3000

int page_fault;                  //ҳ��ʧЧ����
int a[total_instruction];        //ָ������
int page[total_instruction];    //ҳ��ַ��
int full_;                      //full_=-1ʱ����ʾ�ڴ�����

typedef struct frame_struct   //�ڴ�ṹ
{      
	int pn;         //��Ӧ����ҳ��
	int count;      //���ʴ���
	int time;       //�����ڴ�ʱ��
	int exe_time;   //��һ��ִ��ʱ��
}frame_type; 

typedef struct      //���ṹ
{         
	int sig;        //�Ƿ����ڴ�ı�־
	int pfn;       //������ڴ��У������ڴ�ҳ��ҳ��

}p_type; 

p_type page_v[total_vp];  //���ҳ�ṹ����
frame_type page_m[total_vp];    //�ڴ�ҳ�ṹ����

void initialize(int total_pf)   //�ڴ�,���ҳ�ĳ�ʼ��
{ 
	int i; 
	page_fault=0; 
	for(i=0;i<total_vp;i++) 
	{ 
		page_v[i].sig=INVALID;   
		page_v[i].pfn=-1;
	}
	for(i=0;i<total_pf;i++) 
	{
		page_m[i].pn=-1;
		page_m[i].count=0;
		page_m[i].time=-1;
		page_m[i].exe_time=MAX;
	} 
}

int full(int total_pf)           //�ж��ڴ��Ƿ���װ��������Ƿ���-1�����򷵻ؿ�ҳ��ҳ��
{
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].pn==-1)    
			return i;
	}
	return -1;
}

int find_old(int total_pf)       //������������ڴ��ҳ������ҳ��
{
	int old = MAX;
	int k = 0;
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].time<old && page_m[i].time!=-1)
		{
			old=page_m[i].time;
			k=i;
		}
	}
	return k;
}

int find_exetime(int k)    //���Ҹ��ڴ�ҳָ����һ�α�ִ�е�ʱ��
{
	for(int i=k+1;i<total_instruction;i++)
	{
		if(page[i] == page[k])
			return i;
	}
	return MAX;   
}		

int find_most(int total_pf)   //�����ڴ����������ʹ�õ�ҳ������ҳ��
{
	int max_=0;
	int k=0;
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].count>max_)
		{
			max_=page_m[i].count;
			k=i;
		}		
	}
	return k;
}

int find_max(int total_pf)   //���������ڴ�ҳ�о���һ��ִ��ʱ����Զ��ҳ
{
	int max=0;
	int k=0;
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].exe_time>max)
		{
			max=page_m[i].exe_time;
			k=i;
		}
	}
	return k;
}

int find_least(int total_pf)   //�����ڴ��з��ʴ������ٵ�ҳ������ҳ��
{
	int min_=MAX;
	int k=0;
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].count<min_&&page_m[i].count!=0)
		{
			min_=page_m[i].count;
			k=i;
		}
	}
	return k;
}

void FIFO(int total_pf) 
{ 
	int i,j=0; 
	int old,m;

	initialize(total_pf);
	for(i=0;i<total_instruction;i++) 
	{ 
		if(page_v[page[i]].sig==INVALID) //�ڴ�ҳʧЧʱ
		{ 
			page_fault++;   
			full_=full(total_pf);
			if(full_!=-1)                //�ڴ��п�ҳʱ�������ڴ���
			{
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=full_;
				page_m[full_].pn=page[i];//��¼�����ڴ�ʱ��
				page_m[full_].time=j;
				j++;
			}
			else
			{
				old=find_old(total_pf);      //�ڴ�������Ѱ�����ȵ��ڴ��ҳ���û�
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=old;
				page_v[page_m[old].pn].sig=INVALID;
				page_v[page_m[old].pn].pfn=-1;
				page_m[old].pn=page[i];
				page_m[old].time=j;
				j++;
			}
		}
	}
	printf("\t\t%.2f",1-(float)page_fault/total_instruction);  //����������
}

void LRU (int total_pf) 
{ 
	int max,i; 
	initialize(total_pf); 

	for(i=0;i<total_instruction;i++) 
	{ 
		if(page_v[page[i]].sig==INVALID)   //�ڴ�ҳʧЧ
		{ 
			page_fault++; 
			full_=full(total_pf);
			if(full_!=-1)                //�ڴ�δ��ʱ
			{
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=full_;
				page_m[full_].pn=page[i];//�����ڴ�ҳ�У�count��0
				page_m[full_].count=0;   
			}
			else
			{
				max=find_most(total_pf);  //�ڴ�װ��ʱ�����ҵ�����countֵ���û�
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=max;
				page_v[page_m[max].pn].sig=INVALID;
				page_v[page_m[max].pn].pfn=-1;
				page_m[max].pn=page[i];
				page_m[max].count=0;          //�û����ڴ�ҳcount��0

				for(int j=0;j<total_pf;j++)   //����δ�����ʵ��ڴ�ҳcount+1
				{
					if(page_m[j].pn!=page[i])
						page_m[j].count++;
				}
			}
		}
		else   //ҳ�����ڴ���
		{
			for(int j=0;j<total_pf;j++)     //����δ�����ʵ��ڴ�ҳcount+1
			{
				if(page_m[j].pn!=page[i])
					page_m[j].count++;
			}
			page_m[page_v[page[i]].pfn].count=0;  //�����ʵ��ڴ�ҳ��0
		}
	}
	printf("\t\t%.2f",1-(float)page_fault/total_instruction); 
}

void OPT(int total_pf) 
{ 
	int i,j,max;  
	int exe_t;
	initialize(total_pf); 
	for(i=0;i<total_instruction;i++) 
	{ 
		if(page_v[page[i]].sig==INVALID) //���ҳ��ʧЧ
		{ 
			page_fault++; 
			full_=full(total_pf);
			if(full_!=-1)    //����ڴ治��
			{
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=full_;
				//����ҳ�����ڴ��в���¼��һ��ִ��ʱ��
				exe_t=find_exetime(i);
				page_m[full_].exe_time=exe_t; 
				page_m[full_].pn=page[i];
			}
			else
			{
				max=find_max(total_pf);  //�ҵ���һ��ִ��ʱ�������ҳ���û�
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=max;
				page_v[page_m[max].pn].sig=INVALID;
				page_v[page_m[max].pn].pfn=-1;
				page_m[max].pn=page[i];    //�µ�ҳ������ڴ�
				exe_t=find_exetime(i);     //������һ��ִ��ʱ��
				page_m[max].exe_time=exe_t;
			}
		}
		else  //�ڴ�ҳδʧЧ��������һ��ִ��ʱ��
		{
			exe_t=find_exetime(i);        
			page_m[page_v[page[i]].pfn].exe_time=exe_t;	
		}
	}
	printf("\t\t%.2f",1-(float)page_fault/total_instruction); 
}

void LFU(int total_pf) 
{ 
	int i,j,min,minpage,m; 
	int full_;

	initialize(total_pf); 
	for(i=0;i<total_instruction;i++)  
	{ 
		if(page_v[page[i]].sig==INVALID)  //ҳ��ʧЧ
		{
			page_fault++;
			full_=full(total_pf);
			if(full_!=-1)                 //�ڴ治��ʱ����ҳ���룬count��1
			{
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=full_;
				page_m[full_].pn=page[i];
				page_m[full_].count=1;
			}
			else                       //�ڴ�װ��ʱ���û����ʴ������ٵ�ҳ����countֵ��С��ҳ
			{
				min=find_least(total_pf);
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=min;
				page_v[page_m[min].pn].sig=INVALID;
				page_v[page_m[min].pn].pfn=-1;
				page_m[min].pn=page[i];
				page_m[min].count=1;

			}
		}
		else            
			page_m[page_v[page[i]].pfn].count++;    //���У��ڴ�ҳcount++
	} 
	printf("\t\t%.2f",1-(float)page_fault/total_instruction); 
} 

int main(int) 
{   
	int i,m,m1,m2; 
	srand(time(NULL)); 
	for(i=0;i<total_instruction;i++)     //����ָ������
	{
		m=rand()%(total_instruction/2); 
		a[i++]=m;        
		a[i++]=m+1; 

		m1=rand()%(m+2);  
		a[i++]=m1;
		a[i++]=m1+1;
		m2 = (rand() % (total_instruction - (m1 + 2))) + (m1 + 2);
		a[i++]=m2;
	}
	for(i=0;i<total_instruction;i++)    //����ҳ��ַ��
	{
		page[i]=a[i]/64; 
	} 
	printf("\t\t\t�Բ�ͬ֡���ĸ����㷨�����ʱȽ�\n"); 
	printf("\n"); 
	printf("Frame\t\tFIFO\t\tLRU\t\tOPT\t\tLFU\n"); 
	for(i=4;i<=32;i++)
	{
		printf("%d",i);//�ڴ�ҳ�� 
		FIFO(i);
		LRU(i); 
		OPT(i); 
		LFU(i); 

		printf("\n");
	} 
	system("pause"); 
	return 0; 
} 
