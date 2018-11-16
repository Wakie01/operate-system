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

//pcb块
typedef struct pcb{
    char name[20];
	float executeTime;
    int blockTime;
    int isHangup;
    float priority;
	char content[20];
	int waitTime;       //等待时间 
	Source Nsource;     //需要的资源 
	Source Asource;     //分配的资源 
}Pcb;


 
//就绪
typedef struct pcb_ready{
    int num;  //pcb的数量
    Pcb *pcb;
	int size;
}Pcb_ready;


//阻塞
typedef struct pcb_block{
    int num;  //pcb阻塞数量
	Pcb *pcb;
}Pcb_block;


//执行状态
typedef struct pcb_execute{
    int isLock;   //0或1
	Pcb pcb;
	int eachTime;   //轮流的时间片
	int currentTime;    //当前第几个时间片
}Pcb_execute;

//挂起
typedef struct pcb_hangup{
	int num;   //挂起数量
	Pcb *pcb;
}Pcb_hangup;



//对就绪队列按优先级排序
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
            //指针与数组的转换
            if((*pr).pcb[i].priority<(*pr).pcb[j].priority){
                Pcb pcb=(*pr).pcb[i];
                (*pr).pcb[i]=(*pr).pcb[j];
                (*pr).pcb[j]=pcb;
            }
        }
    }
}


//创建进程
Pcb createPCB(Source *ts){
	Pcb pcb;
	printf("请输入进程名：");
	scanf("%s",&pcb.name);
//	printf("请设置进程优先级 1-10 ：");
//	scanf("%d",&pcb.priority);
    pcb.waitTime=0;
	printf("请设置执行时间 >0 ：");
	scanf("%f",&pcb.executeTime);
	pcb.priority=(pcb.waitTime+pcb.executeTime)/pcb.executeTime;
	printf("分别需要资源A,B,C的数量为");
	scanf("%d,%d,%d",&pcb.Nsource.a,&pcb.Nsource.b,&pcb.Nsource.c); 
	if((*ts).a<1||(*ts).b<1||(*ts).c<1){
		printf("资源不够，创建进程失败\n");
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
	printf("初始分配给资源A，B，C的数量各为1\n"); 
	return pcb;

}

//将进程插入就绪队列   kind=1：用户创建的进程，需要排序     kind=0：从CPU转到就绪队列，不用排序
void putReadyQueue(Pcb_ready *pr,Pcb_hangup *ph,Pcb pcb,int kind,Source *ts){
	int putHangupQueue(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Source *ts);
    void queryPcb_ready(Pcb_ready *pr);

	if(pcb.Nsource.a<0){
		printf("进程插入就绪队列失败\n");
		return;
	}
    int num=(*pr).num;
    int size=(*pr).size;

    if(size>num){
        (*pr).pcb[num]=pcb;            // *((*pr).pcb+num)=pcb;   慎用！！最容易出现指针溢出问题
        (*pr).num++;
        if(kind>0)
            sort(pr);
        printf("%s插入就绪队列成功\n",pcb.name);
    }
    else{
        printf("由于就绪队列已满，请先挂起就绪队列中的一个进程\n");
        int ok=putHangupQueue(pr,NULL,ph,ts);
		if(ok>0){
            putReadyQueue(pr,ph,pcb,1,ts);        
        }
        else{
            printf("进程插入就绪队列失败\n");
        }       
    }
}

//将进程挂起     回收所有资源 
int putHangupQueue(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Source *ts){	
	int isHangup;
	printf("确定设置挂起进程 1 or 0 ：\n");
	scanf("%d",&isHangup);
	while(isHangup!=1&&isHangup!=0){
		printf("输入错误，请按要求重新输入指令\n");
		scanf("%d",&isHangup);
	}
	if(isHangup==1){
		printf("请选择进程位置  就绪队列：0     执行室：1    \n");
		int l; 
    	scanf("%d",&l);
    	while(l!=1&&l!=0){
			printf("输入错误，请按要求重新输入指令\n");
			scanf("%d",&l);
		}
		if(l==0){
			printf("请输入进程名：\n");
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
					printf("进程 %s 挂起成功\n",pcb.name);					
					return 0;	
				}			
			}
			printf("找不到你所输入的进程\n");
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
			printf("进程 %s 挂起成功\n",(*pe).pcb.name);
			memset(&(*pe).pcb,0,sizeof(Pcb));
			(*pe).isLock--;
			return 0;			
		}
	}else{
		printf("取消挂起进程成功\n");
		return -1;
	}

}

//列出就绪队列所有进程
void queryPcb_ready(Pcb_ready *pr){
    int num=(*pr).num;
    if(num<=0){
    	printf("就绪队列没有进程\n");
    	return;
	}
    printf("\n就绪队列\n\t进程名\t优先级\t等待时间\t执行时间\t已有a\t已有b\t已有c\t还需a\t还需b\t还需c\n");
    for(int i=0;i<num;i++){
        Pcb pcb=(*pr).pcb[i];
        printf("\t%s\t%0.2f\t%d\t\t%0.2f\t\t%d\t%d\t%d\t%d\t%d\t%d\n",pcb.name,pcb.priority,pcb.waitTime,pcb.executeTime,pcb.Asource.a,pcb.Asource.b,pcb.Asource.c,pcb.Nsource.a,pcb.Nsource.b,pcb.Nsource.c);
    }
	printf("\n");

}

//将就绪队列的头pcb放入执行室
int readyToExcute(Pcb_ready *pr,Pcb_hangup *ph,Pcb_execute *pe,Pcb_block *pb){
	
	void blockToReady(Pcb_ready *pr,Pcb_block *pb,Pcb_hangup *ph);

    if((*pr).num<=0){
        printf("就绪队列中没有进程\t\t");
		if((*pb).num>0){
			printf("但阻塞队列中有 %d 个进程, So请先完成IO处理\n",(*pb).num);
//			system("pause");
			return 0;
		}
		else{
			printf("阻塞队列中也没有进程\n");
			return -1;
		}
		
    }
    else{
        (*pe).pcb=(*pr).pcb[0];
        (*pr).pcb[0]=(*pr).pcb[1];
        memset(&(*pr).pcb[1],0,sizeof(Pcb));    //memset首地址时会自动把整个数组清空
        (*pr).num--;
        sort(pr);
        (*pe).isLock++;
        printf("就绪队列中的 %s 进程成功占有CPU资源\n\n",(*pe).pcb.name);
		return 1;

    }
}

//cpu执行      执行一次时间片 
int execute(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Pcb_block *pb,Source *ts){
	void executeToReady(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Source *ts);
	//执行室为空时，取进程 
	if((*pe).isLock==0){  
		int ok=readyToExcute(pr,ph,pe,pb);
		if(ok<0){
			printf("进程 %s 从就绪队列移入执行室失败\n",(*pr).pcb[0].name);			
			return -1;
		}
		if(ok==0){      //需要进行IO处理 
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
		printf("进程所需资源不足，先退让\n");
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
	printf("%s执行了一个时间片...\n",(*pe).pcb.name);
	
	
	if((*pe).pcb.executeTime<=0){
		printf("\n进程 %s 结束\n",(*pe).pcb.name);
        (*ts).a+=(*pe).pcb.Asource.a;
        (*ts).b+=(*pe).pcb.Asource.b;
        (*ts).c+=(*pe).pcb.Asource.c;
		memset(&(*pe).pcb,0,sizeof(Pcb));     
        (*pe).isLock--;
	}
	if((*pe).currentTime%(*pe).eachTime==0){
		printf("CPU的一次执行周期结束\n");
		(*pe).currentTime=0; 
		if((*pe).isLock!=0){
			executeToReady(pr,pe,ph,ts);
		}		
	}	
}


//执行到就绪
void executeToReady(Pcb_ready *pr,Pcb_execute *pe,Pcb_hangup *ph,Source *ts){
    putReadyQueue(pr,ph,(*pe).pcb,1,ts);
    memset(&(*pe).pcb,0,sizeof(Pcb));
    (*pe).isLock--;
}


//设置是否阻塞
void setBlock(Pcb_ready *pr,Pcb_hangup *ph,Pcb_execute *pe,Pcb_block *pb){	
	int isBlock;
	printf("确定设置阻塞进程 1 or 0 ：\n");
	scanf("%d",&isBlock);
	while(isBlock!=1&&isBlock!=0){
		printf("输入错误，请按要求重新输入指令\n");
		scanf("%d",&isBlock);
	}
	if(isBlock==1){
		int num=(*pb).num;
		int time;
		printf("请输入需要阻塞的时间：\n");
		scanf("%d",&time);
		(*pb).pcb[num]=(*pe).pcb;
		(*pb).pcb[num].blockTime=time;
		(*pb).num++;
		
		printf("进程 %s 发生阻塞，已放入阻塞队列\n",(*pe).pcb.name);
		memset(&(*pe).pcb,0,sizeof(Pcb));
		(*pe).isLock--;
		return;		
	}
	else{
		return;
	}
}


//对阻塞队列排序
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


//查看阻塞队列
void queryBlockQueue(Pcb_block *pb){
	int num=(*pb).num;
	if(num==0){
		printf("\n阻塞队列没有进程\n");
		return;
	}
    printf("\n阻塞队列\n\t\t进程名\t\t阻塞时间\t执行时间\t已有a\t已有b\t已有c\t还需a\t还需b\t还需c\n");
    for(int i=0;i<num;i++){
        Pcb pcb=(*pb).pcb[i];
        printf("\t\t%s\t\t%d\t\t%0.2f\t\t%d\t%d\t%d\t%d\t%d\t%d\n",pcb.name,pcb.blockTime,pcb.executeTime,pcb.Asource.a,pcb.Asource.b,pcb.Asource.c,pcb.Nsource.a,pcb.Nsource.b,pcb.Nsource.c);
    }
	printf("\n");
}


void queryExecute(Pcb_execute *pe){
    if((*pe).isLock!=0){
		printf("\n执行室\n\t\t进程名\t\t执行时间\t拥有a\t拥有b\t拥有c\n");
		Pcb pcb=(*pe).pcb;
		printf("\t\t%s\t\t%0.2f\t\t%d\t%d\t%d\n",pcb.name,pcb.executeTime,pcb.Asource.a,pcb.Asource.b,pcb.Asource.c); 
		printf("\n");
	}
	else{
		printf("\n执行室没有进程\n");
	}
	
}

void queryHangupQueue(Pcb_hangup *ph){
	int num=(*ph).num;
	if(num==0){
		printf("\n挂起队列没有进程\n");
		return;
	}
	printf("\n挂起队列\n\t\t进程名\t\t优先级\t\t执行时间\t还需a\t还需b\t还需c\n");
    for(int i=0;i<num;i++){
        Pcb pcb=(*ph).pcb[i];
        printf("\t\t%s\t\t%d\t\t%0.2f\t\t%d\t%d\t%d\n",pcb.name,pcb.priority,pcb.executeTime,pcb.Nsource.a,pcb.Nsource.b,pcb.Nsource.c);
    }
	printf("\n");
}

void querySource(Source *ts){
	printf("资源a\t资源b\t资源c\n");
	printf("%d\t%d\t%d\n",(*ts).a,(*ts).b,(*ts).c);
}

int activePcb(Pcb_hangup *ph,Pcb_ready *pr,Source *ts){
	void sortHangupPcb(Pcb_hangup *ph);
	int n=(*ph).num;
	if(n==0){
		printf("挂起队列没有进程\n");
		return -1;
	}
	if((*ts).a<1||(*ts).b<1||(*ts).c<1){
		printf("资源不够，激活进程失败\n");
		return -1;
	}
    printf("请输入要激活的进程名：");
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
			printf("%s 进程激活成功\n",pcb.name); 
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
	printf("找不到你所输入的进程\n");
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
		printf("阻塞队列中没有进程\n");
		return;
	}	
	for(int i=0;i<num;i++){
		(*pb).pcb[i].blockTime--;
		if((*pb).pcb[i].blockTime==0){
			putReadyQueue(pr,ph,(*pb).pcb[i],1,ts);
			printf("%s进程完成IO处理，准备放入就绪队列\n",(*pb).pcb[i].name);
			memset(&(*pb).pcb[i],0,sizeof(Pcb));
			(*pb).num--;
		}
	}
	sortBlockPcb(pb);	
	printf("一次IO处理全部完成...\n");
}

void showSystem(){
	printf("\n\n**********************************************************\n");  
	printf("*               高相应比优先调度进程演示系统                 *\n");  
	printf("**************************************************************\n");   
	printf("       1.创建进程        2.查看进程        3.挂起进程\n");       
	printf("       4.运行进程        5.激活进程        6.退出系统 \n");
	printf("       7.清理界面        8.阻塞进程        9.IO处理\n");
	printf("       10.查看剩余资源情况           \n");
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
	pcb_ready.pcb=ready_pcbs;    //易漏点  给头指针指定地址

	pcb_block.num=0;
    pcb_block.pcb=block_pcbs;     //易漏点

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
		printf("\n请选择（1～10）\n");
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
				printf("请输入正确指令（1～10）\n");
				break;
			}
		}
		system("pause");
	}

	return 0;
}

