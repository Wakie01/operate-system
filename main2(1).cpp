#include <stdio.h>
#include<memory.h>
#include<stdlib.h>
#include<iostream>
#include<string.h>
#include<unistd.h>

using namespace std;

typedef struct source{
	int a;
	int b;
	int c;
}Source;

//pcb��
typedef struct pcb{
    char name[20];
	float executeTime;
    int blockTime;
    int isHangup;
    float priority;
	char content[20];
	int waitTime;       //�ȴ�ʱ�� 
	Source Nsource;     //��Ҫ����Դ 
	Source Asource;     //�������Դ 
}Pcb;


 
//����
typedef struct pcb_ready{
    int num;  //pcb������
    Pcb *pcb;
	int size;
}Pcb_ready;


//����
typedef struct pcb_block{
    int num;  //pcb��������
	Pcb *pcb;
}Pcb_block;


//ִ��״̬
typedef struct pcb_execute{
    int isLock;   //0��1
	Pcb pcb;
	int eachTime;   //������ʱ��Ƭ
	int currentTime;    //��ǰ�ڼ���ʱ��Ƭ
}Pcb_execute;

//����
typedef struct pcb_hangup{
	int num;   //��������
	Pcb *pcb;
}Pcb_hangup;



//�Ծ������а����ȼ�����
void sort(Pcb_ready *pr){
    int num=(*pr).num;
	int i,n=0;
	for(i=0;i<(*pr).size;i++){
		if((*pr).pcb[i].priority>0){
			(*pr).pcb[n]=(*pr).pcb[i];
			n++;
		}
		if(n==num){
			break;
		}
	}
    for(i=0;i<num-1;i++){
        for(int j=i+1;j<num;j++){
            //ָ���������ת��
            if((*pr).pcb[i].priority<(*pr).pcb[j].priority){
                Pcb pcb=(*pr).pcb[i];
                (*pr).pcb[i]=(*pr).pcb[j];
                (*pr).pcb[j]=pcb;
            }
        }
    }
}


//��������
Pcb createPCB(Source *ts){
	Pcb pcb;
	printf("�������������");
	scanf("%s",&pcb.name);
//	printf("�����ý������ȼ� 1-10 ��");
//	scanf("%d",&pcb.priority);
    pcb.waitTime=0;
	printf("������ִ��ʱ�� >0 ��");
	scanf("%f",&pcb.executeTime);
	pcb.priority=(pcb.waitTime+pcb.executeTime)/pcb.executeTime;
	printf("�ֱ���Ҫ��ԴA,B,C������Ϊ");
	scanf("%d,%d,%d",&pcb.Nsource.a,&pcb.Nsource.b,&pcb.Nsource.c); 
	if((*ts).a<1||(*ts).b<1||(*ts).c<1){
		printf("��Դ��������������ʧ��\n");
		pcb.Nsource.a=-1;
		return pcb;
	}
	(*ts).a--;
	(*ts).b--;
	(*ts).c--;
	pcb.Nsource.a--;
	pcb.Nsource.b--;
	pcb.Nsource.c--;
	pcb.Asource.a=1;
	pcb.Asource.b=1;
	pcb.Asource.c=1;
	printf("��ʼ�������ԴA��B��C��������Ϊ1\n"); 
	return pcb;

}

//�����̲����������   kind=1���û������Ľ��̣���Ҫ����     kind=0����CPUת���������У���������
void putReadyQueue(Pcb_ready *pr,Pcb_hangup *ph,Pcb pcb,int kind,Source *ts){
	int putHangupQueue(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Source *ts);
    void queryPcb_ready(Pcb_ready *pr);

	if(pcb.Nsource.a<0){
		printf("���̲����������ʧ��\n");
		return;
	}
    int num=(*pr).num;
    int size=(*pr).size;

    if(size>num){
        (*pr).pcb[num]=pcb;            // *((*pr).pcb+num)=pcb;   ���ã��������׳���ָ���������
        (*pr).num++;
        if(kind>0)
            sort(pr);
        printf("%s����������гɹ�\n",pcb.name);
    }
    else{
        printf("���ھ����������������ȹ�����������е�һ������\n");
        int ok=putHangupQueue(pr,NULL,ph,ts);
		if(ok>0){
            putReadyQueue(pr,ph,pcb,1,ts);        
        }
        else{
            printf("���̲����������ʧ��\n");
        }       
    }
}

//�����̹���     ����������Դ 
int putHangupQueue(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Source *ts){	
	int isHangup;
	printf("ȷ�����ù������ 1 or 0 ��\n");
	scanf("%d",&isHangup);
	while(isHangup!=1&&isHangup!=0){
		printf("��������밴Ҫ����������ָ��\n");
		scanf("%d",&isHangup);
	}
	if(isHangup==1){
		printf("��ѡ�����λ��  �������У�0     ִ���ң�1    \n");
		int l; 
    	scanf("%d",&l);
    	while(l!=1&&l!=0){
			printf("��������밴Ҫ����������ָ��\n");
			scanf("%d",&l);
		}
		if(l==0){
			printf("�������������\n");
			char name[20];
			scanf("%s",&name);
			for(int i=0;i<(*pr).num;i++){
				if(strcmp((*pr).pcb[i].name,name)==0){			
					int num=(*ph).num;
					Pcb pcb=(*pr).pcb[i];
					
					(*ts).a+=pcb.Asource.a;
					(*ts).b+=pcb.Asource.b;
					(*ts).c+=pcb.Asource.c;
					pcb.Nsource.a+=pcb.Asource.a;	
					pcb.Nsource.b+=pcb.Asource.b;	
					pcb.Nsource.c+=pcb.Asource.c;
					pcb.Asource.a=0;
					pcb.Asource.b=0;
					pcb.Asource.c=0;						
					
					pcb.isHangup=1;		
					(*ph).pcb[num]=pcb;
					(*ph).num++;
					if(i==0){
						(*pr).pcb[0]=(*pr).pcb[1];
						(*pr).pcb[1]=pcb;
						
						memset(&(*pr).pcb[1],0,sizeof(Pcb));
						(*pr).num--;
					}else{
						memset(&(*pr).pcb[i],0,sizeof(Pcb));
						(*pr).num--;
					}
					sort(pr);
					printf("���� %s ����ɹ�\n",pcb.name);					
					return 0;	
				}			
			}
			printf("�Ҳ�����������Ľ���\n");
			return 0; 
		}
		else if(l==1){
			int num=(*ph).num;
			Pcb pcb=(*pe).pcb;
			
			(*ts).a+=pcb.Asource.a;
			(*ts).b+=pcb.Asource.b;
			(*ts).c+=pcb.Asource.c;
			pcb.Nsource.a+=pcb.Asource.a;	
			pcb.Nsource.b+=pcb.Asource.b;	
			pcb.Nsource.c+=pcb.Asource.c;
			pcb.Asource.a=0;
			pcb.Asource.b=0;
			pcb.Asource.c=0;
					
			(*ph).pcb[num]=pcb;
			(*ph).pcb[num].isHangup=1;
			(*ph).num++;
			printf("���� %s ����ɹ�\n",(*pe).pcb.name);
			memset(&(*pe).pcb,0,sizeof(Pcb));
			(*pe).isLock--;
			return 0;			
		}
	}else{
		printf("ȡ��������̳ɹ�\n");
		return -1;
	}

}

//�г������������н���
void queryPcb_ready(Pcb_ready *pr){
    int num=(*pr).num;
    if(num<=0){
    	printf("��������û�н���\n");
    	return;
	}
    printf("\n��������\n\t������\t���ȼ�\t�ȴ�ʱ��\tִ��ʱ��\t����a\t����b\t����c\t����a\t����b\t����c\n");
    for(int i=0;i<num;i++){
        Pcb pcb=(*pr).pcb[i];
        printf("\t%s\t%0.2f\t%d\t\t%0.2f\t\t%d\t%d\t%d\t%d\t%d\t%d\n",pcb.name,pcb.priority,pcb.waitTime,pcb.executeTime,pcb.Asource.a,pcb.Asource.b,pcb.Asource.c,pcb.Nsource.a,pcb.Nsource.b,pcb.Nsource.c);
    }
	printf("\n");

}

//���������е�ͷpcb����ִ����
int readyToExcute(Pcb_ready *pr,Pcb_hangup *ph,Pcb_execute *pe,Pcb_block *pb){
	
	void blockToReady(Pcb_ready *pr,Pcb_block *pb,Pcb_hangup *ph);

    if((*pr).num<=0){
        printf("����������û�н���\t\t");
		if((*pb).num>0){
			printf("�������������� %d ������, So�������IO����\n",(*pb).num);
//			system("pause");
			return 0;
		}
		else{
			printf("����������Ҳû�н���\n");
			return -1;
		}
		
    }
    else{
        (*pe).pcb=(*pr).pcb[0];
        (*pr).pcb[0]=(*pr).pcb[1];
        memset(&(*pr).pcb[1],0,sizeof(Pcb));    //memset�׵�ַʱ���Զ��������������
        (*pr).num--;
        sort(pr);
        (*pe).isLock++;
        printf("���������е� %s ���̳ɹ�ռ��CPU��Դ\n\n",(*pe).pcb.name);
		return 1;

    }
}

//cpuִ��      ִ��һ��ʱ��Ƭ 
int execute(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Pcb_block *pb,Source *ts){
	void executeToReady(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Source *ts);
	//ִ����Ϊ��ʱ��ȡ���� 
	if((*pe).isLock==0){  
		int ok=readyToExcute(pr,ph,pe,pb);
		if(ok<0){
			printf("���� %s �Ӿ�����������ִ����ʧ��\n",(*pr).pcb[0].name);			
			return -1;
		}
		if(ok==0){      //��Ҫ����IO���� 
			return -1;
		}      				
	}
	if((*ts).a>=(*pe).pcb.Nsource.a&&(*ts).b>=(*pe).pcb.Nsource.b&&(*ts).c>=(*pe).pcb.Nsource.c){
		(*pe).pcb.Asource.a+=(*pe).pcb.Nsource.a;
		(*pe).pcb.Asource.b+=(*pe).pcb.Nsource.b;
		(*pe).pcb.Asource.c+=(*pe).pcb.Nsource.c;
		(*ts).a-=(*pe).pcb.Nsource.a;
		(*ts).b-=(*pe).pcb.Nsource.b;
		(*ts).c-=(*pe).pcb.Nsource.c;
		(*pe).pcb.Nsource.a=0;
		(*pe).pcb.Nsource.b=0;
		(*pe).pcb.Nsource.c=0;
	}
	else{
		printf("����������Դ���㣬������\n");
		(*pe).pcb.waitTime=0;
		for(int i=0;i<(*pr).num;i++){
			(*pr).pcb[i].waitTime++;
			(*pr).pcb[i].priority=((*pr).pcb[i].waitTime+(*pr).pcb[i].executeTime)/(*pr).pcb[i].executeTime;
		}
		executeToReady(pr,pe,ph,ts);	
		return -1;
	} 
	(*pe).currentTime++;
	(*pe).pcb.executeTime--;
	(*pe).pcb.waitTime=0;	
	for(int i=0;i<(*pr).num;i++){
		(*pr).pcb[i].waitTime++;
		(*pr).pcb[i].priority=((*pr).pcb[i].waitTime+(*pr).pcb[i].executeTime)/(*pr).pcb[i].executeTime;
	}
	sort(pr);
	printf("%sִ����һ��ʱ��Ƭ...\n",(*pe).pcb.name);
	
	
	if((*pe).pcb.executeTime<=0){
		printf("\n���� %s ����\n",(*pe).pcb.name);
        (*ts).a+=(*pe).pcb.Asource.a;
        (*ts).b+=(*pe).pcb.Asource.b;
        (*ts).c+=(*pe).pcb.Asource.c;
		memset(&(*pe).pcb,0,sizeof(Pcb));     
        (*pe).isLock--;
	}
	if((*pe).currentTime%(*pe).eachTime==0){
		printf("CPU��һ��ִ�����ڽ���\n");
		(*pe).currentTime=0; 
		if((*pe).isLock!=0){
			executeToReady(pr,pe,ph,ts);
		}		
	}	
}


//ִ�е�����
void executeToReady(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Source *ts){
    putReadyQueue(pr,ph,(*pe).pcb,1,ts);
    memset(&(*pe).pcb,0,sizeof(Pcb));
    (*pe).isLock--;
}


//�����Ƿ�����
void setBlock(Pcb_ready *pr,Pcb_hangup *ph,Pcb_execute *pe,Pcb_block *pb){	
	int isBlock;
	printf("ȷ�������������� 1 or 0 ��\n");
	scanf("%d",&isBlock);
	while(isBlock!=1&&isBlock!=0){
		printf("��������밴Ҫ����������ָ��\n");
		scanf("%d",&isBlock);
	}
	if(isBlock==1){
		int num=(*pb).num;
		int time;
		printf("��������Ҫ������ʱ�䣺\n");
		scanf("%d",&time);
		(*pb).pcb[num]=(*pe).pcb;
		(*pb).pcb[num].blockTime=time;
		(*pb).num++;
		
		printf("���� %s �����������ѷ�����������\n",(*pe).pcb.name);
		memset(&(*pe).pcb,0,sizeof(Pcb));
		(*pe).isLock--;
		return;		
	}
	else{
		return;
	}
}


//��������������
void sortBlockPcb(Pcb_block *pb){
	int num=(*pb).num;
	int n=0;
	int i;
	for(i=0;i<100;i++){
		if((*pb).pcb[i].blockTime>0){
			(*pb).pcb[n]=(*pb).pcb[i];
			n++;
		}
		if(n==num){
			break;
		}
	}
	for(i=0;i<num-1;i++){
		for(int j=i+1;j<num;j++){
			if((*pb).pcb[i].blockTime<(*pb).pcb[j].blockTime){
				Pcb pcb=(*pb).pcb[i];
				(*pb).pcb[i]=(*pb).pcb[j];
				(*pb).pcb[j]=pcb;
			}
		}
	}
}


//�鿴��������
void queryBlockQueue(Pcb_block *pb){
	int num=(*pb).num;
	if(num==0){
		printf("\n��������û�н���\n");
		return;
	}
    printf("\n��������\n\t\t������\t\t����ʱ��\tִ��ʱ��\t����a\t����b\t����c\t����a\t����b\t����c\n");
    for(int i=0;i<num;i++){
        Pcb pcb=(*pb).pcb[i];
        printf("\t\t%s\t\t%d\t\t%0.2f\t\t%d\t%d\t%d\t%d\t%d\t%d\n",pcb.name,pcb.blockTime,pcb.executeTime,pcb.Asource.a,pcb.Asource.b,pcb.Asource.c,pcb.Nsource.a,pcb.Nsource.b,pcb.Nsource.c);
    }
	printf("\n");
}


void queryExecute(Pcb_execute *pe){
    if((*pe).isLock!=0){
		printf("\nִ����\n\t\t������\t\tִ��ʱ��\tӵ��a\tӵ��b\tӵ��c\n");
		Pcb pcb=(*pe).pcb;
		printf("\t\t%s\t\t%0.2f\t\t%d\t%d\t%d\n",pcb.name,pcb.executeTime,pcb.Asource.a,pcb.Asource.b,pcb.Asource.c); 
		printf("\n");
	}
	else{
		printf("\nִ����û�н���\n");
	}
	
}

void queryHangupQueue(Pcb_hangup *ph){
	int num=(*ph).num;
	if(num==0){
		printf("\n�������û�н���\n");
		return;
	}
	printf("\n�������\n\t\t������\t\t���ȼ�\t\tִ��ʱ��\t����a\t����b\t����c\n");
    for(int i=0;i<num;i++){
        Pcb pcb=(*ph).pcb[i];
        printf("\t\t%s\t\t%d\t\t%0.2f\t\t%d\t%d\t%d\n",pcb.name,pcb.priority,pcb.executeTime,pcb.Nsource.a,pcb.Nsource.b,pcb.Nsource.c);
    }
	printf("\n");
}

void querySource(Source *ts){
	printf("��Դa\t��Դb\t��Դc\n");
	printf("%d\t%d\t%d\n",(*ts).a,(*ts).b,(*ts).c);
}

int activePcb(Pcb_hangup *ph,Pcb_ready *pr,Source *ts){
	void sortHangupPcb(Pcb_hangup *ph);
	int n=(*ph).num;
	if(n==0){
		printf("�������û�н���\n");
		return -1;
	}
	if((*ts).a<1||(*ts).b<1||(*ts).c<1){
		printf("��Դ�������������ʧ��\n");
		return -1;
	}
    printf("������Ҫ����Ľ�������");
    char name[20];
    scanf("%s",&name);	
    for(int i=0;i<n;i++){
		if(strcmp((*ph).pcb[i].name,name)==0){
			Pcb pcb=(*ph).pcb[i];
			pcb.isHangup=-1;
			
			(*ts).a--;
			(*ts).b--;
			(*ts).c--;
			pcb.Nsource.a--;
			pcb.Nsource.b--;
			pcb.Nsource.c--;
			pcb.Asource.a=1;
			pcb.Asource.b=1;
			pcb.Asource.c=1;
			
			putReadyQueue(pr,ph,pcb,1,ts);
			(*ph).num--;
			printf("%s ���̼���ɹ�\n",pcb.name); 
			if(i==0){
				(*ph).pcb[0]=(*ph).pcb[1];
				(*ph).pcb[1]=pcb;
				memset(&(*ph).pcb[1],0,sizeof(Pcb)); 
			} 
			else{
				memset(&(*ph).pcb[i],0,sizeof(Pcb)); 
			}
			sortHangupPcb(ph);						         
            return 1;
        }
    }
	printf("�Ҳ�����������Ľ���\n");
	return -1;
}

void sortHangupPcb(Pcb_hangup *ph){
	int num=(*ph).num;
	int n=0;
	for(int i=0;i<100;i++){
		if((*ph).pcb[i].isHangup>0){
			(*ph).pcb[n]=(*ph).pcb[i];
			n++;
		}
		if(n==num){
			break;
		}
	}
		
}

void iohandle(Pcb_block *pb,Pcb_ready *pr,Pcb_hangup *ph,Source *ts){
	int num=(*pb).num;
	if(num<=0){
		printf("����������û�н���\n");
		return;
	}	
	for(int i=0;i<num;i++){
		(*pb).pcb[i].blockTime--;
		if((*pb).pcb[i].blockTime==0){
			putReadyQueue(pr,ph,(*pb).pcb[i],1,ts);
			printf("%s�������IO����׼�������������\n",(*pb).pcb[i].name);
			memset(&(*pb).pcb[i],0,sizeof(Pcb));
			(*pb).num--;
		}
	}
	sortBlockPcb(pb);	
	printf("һ��IO����ȫ�����...\n");
}

void showSystem(){
	printf("\n\n**********************************************************\n");  
	printf("*               ����Ӧ�����ȵ��Ƚ�����ʾϵͳ                 *\n");  
	printf("**************************************************************\n");   
	printf("       1.��������        2.�鿴����        3.�������\n");       
	printf("       4.���н���        5.�������        6.�˳�ϵͳ \n");
	printf("       7.�������        8.��������        9.IO����\n");
	printf("       10.�鿴ʣ����Դ���           \n");
	printf("**************************************************************\n\n");   
	
}

int main(){

	Pcb ready_pcbs[10];
    Pcb block_pcbs[100];
    Pcb hangup_pcbs[100];
	Source totalSource;
	int i;
	for(i=0;i<10;i++){
		ready_pcbs[i].priority=0;
	}
	for(i=0;i<100;i++){
		block_pcbs[i].blockTime=-1;
	}
	for(i=0;i<100;i++){
		hangup_pcbs[i].isHangup=-1;
	}
	
	totalSource.a=10; 
	totalSource.b=10; 
	totalSource.c=10; 
	
	Pcb_ready pcb_ready;
	Pcb_block pcb_block;
    Pcb_execute pcb_execute;
    Pcb_hangup pcb_hangup;

	pcb_ready.size=10;
	pcb_ready.num=0;
	pcb_ready.pcb=ready_pcbs;    //��©��  ��ͷָ��ָ����ַ

	pcb_block.num=0;
    pcb_block.pcb=block_pcbs;     //��©��

    pcb_execute.isLock=0;
    pcb_execute.eachTime=3;
    pcb_execute.currentTime=0;

    pcb_hangup.num=0;
    pcb_hangup.pcb=hangup_pcbs;
	showSystem();
	
	int n=1;
	int kind=-1;
	while(n){   
		int i;
		printf("\n��ѡ��1��10��\n");
		scanf("%d",&i);
		switch(i){
			case 1:{
					Pcb pcb=createPCB(&totalSource);
					putReadyQueue(&pcb_ready,&pcb_hangup,pcb,kind,&totalSource);
					break;
				}
			case 2:{
					queryPcb_ready(&pcb_ready);
					printf("\n");
					queryExecute(&pcb_execute);
					printf("\n");
					queryBlockQueue(&pcb_block);
					printf("\n");
					queryHangupQueue(&pcb_hangup);
					printf("\n");
					break;
				}
			case 3:{
					putHangupQueue(&pcb_ready,&pcb_execute,&pcb_hangup,&totalSource);
					break;
				}
			case 4:{					
					execute(&pcb_ready,&pcb_execute,&pcb_hangup,&pcb_block,&totalSource);
					break;
				}
			case 5:{
					activePcb(&pcb_hangup,&pcb_ready,&totalSource);
					break;
				}
			case 6:{
					n=0;
					break;
				}
			case 7:{
					system("cls");
					showSystem();
					break;
				} 
			case 8:{
				setBlock(&pcb_ready,&pcb_hangup,&pcb_execute,&pcb_block);
				break;
			}
			case 9:{
				iohandle(&pcb_block,&pcb_ready,&pcb_hangup,&totalSource);
				break;
			}
			case 10:{
				querySource(&totalSource); 
				break;
			}
			default:{
				printf("��������ȷָ�1��10��\n");
				break;
			}
		}
		system("pause");
	}

	return 0;
}

