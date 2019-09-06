#include<iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <vector>
#include "cstdio"
#include "iostream"
#include "ARIMAModel.h"
#include "Pancreas.h"
#include "MAModel.h"
#include <cstring>
#include <cmath>
#include <cstdlib>
#include "ARMAMath.h"
#include <algorithm>
#include<stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <iterator>
#include <iostream>
#include "TCPServer.hpp"
#include<string.h>
#include <unistd.h>
using namespace std;
//global variable
#define start_prediction 20
//#define timestep 288 (pancreas.h)
#define DEMO_PORT 5578
string ip="127.0.0.1";
#define MAX_LISTEN_QUEUE 10
#define buffersize 1024
#define datalen 30
#define DEFAULT_QUEUE_NUM 5
int flag_exit=0;
 int clientSockfd;
double Kc = -0.6;//initial PID parameter
double	tauI = 0.6;
double	tauD = 0;
	double dpv[2];
	double ie[2];
int period=1;//initial arma parameter
int modelCnt=5;
int step = 3;
double predict;
int i=0;
int Model =2;
double buffer[buffersize];
int UperLimits=90;
int LowerLimits=70;
int setpoint =75;
pthread_mutex_t lock_sensor =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_sensor = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_pid =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_pid = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_arma =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_arma = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_difference =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_difference = PTHREAD_COND_INITIALIZER;

pthread_rwlock_t rwlock; 
pthread_t t_sensor,t_pid,t_arma,t_difference,t_commP;
void trans(double a[],double b[])
{
	for(int i=0;i<timestep+1;i++)
	{
		a[i]=b[i];
	}
}
struct pid_data//data of pid controller
{
	double Kc;
	double tauI;
	double tauD;
	double pv;
	double delta_t;
	// int i;
	// double dpv[timestep+1];
	// double ie[timestep+1];
	// double result;
	
};
struct arma_data//data of arma prediction
{
	int modelCnt;
	int period;
	int step;
	std::vector<double> dataArray;
	double result;
	
};


void* get_sensor_data(void* arg)//get glucouse from sensor
	{
		
		while(1)
		{	//sleep(1);
			pthread_mutex_lock(&lock_sensor);
			// cout<<i<<"   ";
						// printf("get data successfully! \n");

			pthread_cond_signal(&cv_sensor);//move to pid
			
			pthread_mutex_unlock(&lock_sensor);
			if (i==timestep)
			{
				flag_exit=1;
				
			}
			if(flag_exit) break;
			sleep(1);
			
			
		}
		
				
	}
void* arma_forecast(void*arg)
{  	
	while(1)
	{
		if (i>=20)
		{
		arma_data *arma;
		arma=(arma_data *) arg;
		int modelCnt=arma->modelCnt;
		int period=arma->period;
		int step=arma->step;

		if(flag_exit) break;
		pthread_mutex_lock(&lock_sensor);
		pthread_cond_wait(&cv_sensor, &lock_sensor);
		
		pthread_mutex_unlock(&lock_sensor);
	
		pthread_mutex_lock(&lock_arma);
				dataArray.push_back(g[0]);

	// printf("start ARMA\n");
    // copy(dataArray.begin(),dataArray.end(),ostream_iterator<double>(cout,"\n"));


		ARIMAModel*arima = new ARIMAModel(dataArray);

		predict = arima->arma_forecast(modelCnt, period, step);
		// arma->result=predict;
		
		pthread_cond_signal(&cv_arma);//move to pid
		pthread_mutex_unlock(&lock_arma);
		sleep(0.5);}
	}
	return (NULL);
}
void* pid_control(void*arg)
{   
	

	pid_data *pid;
	pid=(pid_data *) arg;
	double Kc=pid->Kc;
	double tauI=pid->tauI;
	double tauD=pid->tauD;
	double pv;
	double delta_t=ts;
	// int i=pid->i;
	// double dpv[timestep+1];
	// trans(dpv,pid->dpv);
	// double ie[timestep+1];
	// trans(ie,pid->ie);
	double P[2];
	double Ii[2];
	double D[2];
	double e[2];
	double op[2];

	op[0] = 3.0;
	e[0] = 0;
	dpv[0]=0;
	ie[0]=0;
	double	op_hi = 20.0;
	double op_lo = 0.0;
	//set range=[70~90]
	
    while(1)
	{
		
		if(flag_exit) break;
	
		if(i<20)
		{   
		  

			pthread_mutex_lock(&lock_sensor);
			pthread_cond_wait(&cv_sensor, &lock_sensor);
	
			pthread_mutex_unlock(&lock_sensor);


		}
	
	
		
		if(i>=20)
		{
			pthread_mutex_lock(&lock_arma);
			pthread_cond_wait(&cv_arma,&lock_arma);
			
	        pthread_mutex_unlock(&lock_arma);
		 }
		
		
		pthread_mutex_lock(&lock_pid);
		// printf("start PID\n");

		if(i==1){pv=g[0];	dataArray.push_back(g[0]);}
		else if(i<20){pv=g[0];	dataArray.push_back(g[0]);}
		else if(i>=20){pv=predict;}
		if(Model==1)
		{e[1]=setpoint-pv;}
		if(Model==2)
		{
		if (LowerLimits < pv && pv < UperLimits) { e[1] = 0; }
		else if (pv <= LowerLimits) { e[1] = LowerLimits - pv; }
		else if (pv >= UperLimits) { e[1] = UperLimits - pv; }
		}
		if (i >= 1)
		{
			dpv[1] = (e[1] - e[0]) / delta_t;
			ie[1] = ie[0] + e[0] * delta_t;
			dpv[0]=dpv[1];
			ie[0]=ie[1];
			e[0]=e[1];
		}
		P[0] = Kc * e[1];
		Ii[0] = Kc / tauI * ie[1];
		D[0] = Kc * tauD * dpv[1];
		op[0] = op[0] + P[0] + Ii[0] + D[0];
		if (op[0] > op_hi)
		{
			op[0] = op_hi;
			ie[0] = ie[0] - e[0] * delta_t;
		}
		if (op[0] < op_lo) {
			op[0] = op_lo;
			ie[0] = ie[0] - e[0] * delta_t;
		}
		u[1]=op[0];

		i++;
	
		pthread_cond_signal(&cv_pid);

	pthread_mutex_unlock(&lock_pid);
	

		sleep(0.5);
		}
	//reslut
	// pid->result=op[i];
	// pid->ie[i]=ie[i];
	// pid->dpv[i]=dpv[i];
	
	return (NULL);
}
void* difference(void* arg)
	{   
		
	
		while(1)
		{
			pthread_mutex_lock(&lock_pid);
	pthread_cond_wait(&cv_pid,&lock_pid);
	pthread_mutex_unlock(&lock_pid);

			pthread_mutex_lock(&lock_difference);
			// printf("start difference\n");
		
			if(flag_exit) break;
			i--;

		// cout<<g[i]<<endl;
					// cout<<"insulin="<<u[i+1]<<"   ";

		// cout<<"predict="<<predict<<endl;
			// cout<<d[i+1]<<endl;
			double K1 = -ke * I[0] + u[1];
			double K2 = -ke * (I[0] + 0.5*K1) + u[1];
			double K3 = -ke * (I[0] + 0.5*K2) + u[ 1];
			double K4 = -ke * (I[0] + K3) + u[ 1];
			I[ 1] = I[0] + 0.1666667*0.08333333*(K1 + 2 * K2 + 2 * K3 + K4) * 60;
			double I2h = (I[ 1] + I[0]) / 2;
			K1 = p2 * (I[0] - x[0]);
			K2 = p2 * (I2h - (x[0] + 0.5*K1));
			K3 = p2 * (I2h - (x[0] + 0.5*K2));
			K4 = p2 * (I[ 1] - (x[0] + K3));
			x[ 1] = x[0] + 0.1666667*0.08333333*(K1 + 2 * K2 + 2 * K3 + K4) * 60;
			double x2h = (x[ 1] + x[0]) / 2;
			K1 = u[1] - kemp * q1[0];
			K2 = -kemp * (q1[0] + 0.5*K1) + u[ 1];
			K3 = -kemp * (q1[0] + 0.5*K2) + u[ 1];
			K4 = -kemp * (q1[0] + K3) + u[ 1];
			q1[ 1] = q1[0] + 0.1666667*0.08333333*(K1 + 2 * K2 + 2 * K3 + K4) * 60;
			double q12h = (q1[ 1] + q1[0]) / 2;
			K1 = kemp * (q1[0] - q2[0]);
			K2 = kemp * (q12h - (q2[0] + 0.5*K1));
			K3 = kemp * (q12h - (q2[0] + 0.5*K2));
			K4 = kemp * (q1[ 1] - (q2[0] + K3));
			q2[ 1] = q2[0] + 0.1666667*0.08333333*(K1 + 2 * K2 + 2 * K3 + K4) * 60;
			double q22h = (q2[ 1] + q2[0]) / 2;
			K1 = kemp * q2[0] - kabs * g_gut[0];
			K2 = kemp * q22h - kabs * (g_gut[0] + 0.5*K1);
			K3 = kemp * q22h - kabs * (g_gut[0] + 0.5*K2);
			K4 = kemp * q2[1] - kabs * (g_gut[0] + K3);
			g_gut[1] = g_gut[0] + 0.1666667*0.08333333*(K1 + 2 * K2 + 2 * K3 + K4) * 60;
			double g_gut2h = (g_gut[1] + g_gut[0]) / 2;
			K1 = -p1 * (g[0] - gb) - si * x[0] * g[0] + f * kabs / vg * g_gut[0] + f / vg * d[i + 1];
			
			K2 = -p1 * (g[0] + 0.5*K1 - gb) - si * x2h * (g[0] + 0.5*K1) + f * kabs / vg * g_gut2h + f / vg * d[i + 1];
			K3 = -p1 * (g[0] + 0.5*K2 - gb) - si * x2h * (g[0] + 0.5*K2) + f * kabs / vg * g_gut2h + f / vg * d[i + 1];
			K4 = -p1 * (g[0] + K3 - gb) - si * x[ 1] * (g[0] + K3) + f * kabs / vg * g_gut[ 1] + f / vg * d[i + 2];
			
			g[1] = g[0] + 0.1666667*0.08333333*(K1 + 2 * K2 + 2 * K3 + K4) * 10;
			i++;
			I[0]=I[1];
			x[0]=x[1];
			q1[0]=q1[1];
			q2[0]=q2[1];
			g_gut[0]=g_gut[1];
			g[0]=g[1];
				
	
		if(dataArray.size()<=datalen)
	for (int j=0;j<dataArray.size()-1;j++)
	{
		buffer[j]=dataArray[j];
	}
		else
		{
			for (int j=0;j<datalen-1;j++)
			{int end=dataArray.size();
			buffer[j]=dataArray[end-30+j];}
		
		}
		
		
	
		pthread_mutex_unlock(&lock_difference);
		sleep(0.1);
		
		}
		
		
		
	}


void* HandleClient(void* arg)
{
struct timespec outtime;
//	printf( "Thread %d i s running \n" , pthread_self ( ) ) ;

	int socket_desc , client_sock , c , read_size;
	struct sockaddr_in server , client;
	char client_message[2000],data[10]={'\0'};
	long i;
	//printf("Starting server\n");
//	printk("Server Started\n\n");
	//Create socket
	socket_desc = socket(PF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		cout<<"Could not create socket"<<endl;;
	}
	    cout<<"Socket created"<<endl;

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip.c_str());
	server.sin_port = htons( 5578 );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) != 0)
	{
		// print the error message;
		cout<<"bind failed. Error"<<endl;
		return NULL;
	}
	cout<<"bind done"<<endl;

	//Listen
	listen(socket_desc , 1);

	//Accept and incoming connection
//	printk("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while(1)
	{
		cout<<"Waiting for incoming connections...";
	    //accept connection from an incoming client
	    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	    if (client_sock < 0)
	    {
	        cout<<"accept failed"<<endl;
	        
	    }
	    cout<<"Connection accepted\n"<<endl;
		while(1)
		{
	
        
			char recvBuf[BUFF_SIZE];
			double recvdata[BUFF_SIZE];
			memset(recvBuf, 0, BUFF_SIZE);
			ssize_t recvLen = recv(client_sock, recvBuf, BUFF_SIZE, 0);
			
			if (0 > recvLen)    
				{
					std::cout << "Receive Wrong Information（sockfd = " << client_sock << "）！" << std::endl;
					close(client_sock);
					break;
				}
			else if (0 == recvLen)  
				{
					std::cout << "Receive Information Failed，（sockfd = " << client_sock << "）Disconnection！" << std::endl;
					close(client_sock);
					break;
				}
			else    
			{
				
				std::cout << "Receive（sockfd = " << client_sock << "）Information：" << recvBuf << std::endl;
			}

			if (0 == strcmp(recvBuf, "Get_Data"))
			{
			
				ssize_t sendLen = send(client_sock, buffer,  buffersize, 0);
				continue;
			}
			if (0 == strcmp(recvBuf, "Change_PID"))
			{
				char command[buffersize]={"Cin Paramenter Kc,tauI,tauD;"};
				
				ssize_t sendLen = send(client_sock, command,  buffersize, 0);
				recvLen = recv(client_sock, recvdata, BUFF_SIZE, 0);
				
				Kc=recvdata[0];
				tauI=recvdata[1];
				tauD=recvdata[2];
				char command1[buffersize]={"Send Successfully!"};
				send(client_sock, command1,  buffersize, 0);
				
				continue;
			}
			if (0 == strcmp(recvBuf, "Change_setpoint"))
			{
				char command[buffersize]={"Set Model---1 for setpoint /2 for set range;"};
				
				ssize_t sendLen = send(client_sock, command,  buffersize, 0);
				
				recvLen = recv(client_sock, recvdata, BUFF_SIZE, 0);
				
				if(recvdata[0]==1)
				{
					Model=1;
					char command1[buffersize]={"Cin Setpoint"};
					send(client_sock, command1,  buffersize, 0);
					recvLen = recv(client_sock, recvdata, BUFF_SIZE, 0);
					setpoint=recvdata[0];
					char command2[buffersize]={"Send Successfully!"};
				send(client_sock, command2,  buffersize, 0);
					}
				if(recvdata[0]==2)
				{Model=2;
			
				char command1[buffersize]={"Cin UperLimits and LowerLimits "};
				send(client_sock, command1,  buffersize, 0);
				recvLen = recv(client_sock, recvdata, BUFF_SIZE, 0);
				UperLimits=recvdata[0];
				LowerLimits=recvdata[1];
				char command2[buffersize]={"Send Successfully!"};
				send(client_sock, command2,  buffersize, 0);
				continue;
				}

			}
			
			
			
		if (0 == strcmp(recvBuf, "quit"))
			{
				std::cout << "Cilent（sockfd = " << client_sock << "）Disconnected！" << std::endl;
				break;
			}
           
        }
	close(client_sock);
	outtime.tv_sec =  0;
	outtime.tv_nsec = 10*1000000;
	nanosleep(&outtime,NULL);
	
	}      
		std::cout << "Close Server (sockfd = " << client_sock << ")" << std::endl;

	outtime.tv_sec =  0;
	outtime.tv_nsec = 10*1000000;
	nanosleep(&outtime,NULL);
	

    
    return NULL;
}




int main()
{

	// Pancreas pancreas;
	pid_data pid;
	arma_data arma;
	
	// Initialization//
	// pancreas.sensor_initialiaztion();

	pid.Kc=Kc;
	pid.tauI=tauI;
	pid.tauD=tauD;
	arma.period = period;
	arma.modelCnt = modelCnt;
	arma.step = step;
	if(pthread_create(&t_commP, NULL, HandleClient, NULL))//creat pid thread
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	// TCPServer server(DEMO_IP, DEMO_PORT);
    // if (Ret_success != server.BindAddr())
    // {
    //     exit(1);
    // }
    // if (Ret_success != server.Listen(MAX_LISTEN_QUEUE))
    // {
    //     exit(1);
    // }
    // if (Ret_success != server.Run())
    // {
    //     exit(1);
    // }
	// for (int i = 0; i < timestep; i++)
	// {
		
		// dataArray.push_back(g[i]);//renew data//
		// pid.delta_t= ts;//pancreas.//
		
		// pid.pv= g[i];//
		// pid.i= i;
		// //PID Control//
		// if(pthread_create(&t_pid,NULL,pid_control,&pid))//creat pid thread
		// {
			// fprintf(stderr, "Error creating thread\n");
			// return 1;
		// }
		// pthread_join(t_pid, NULL);//wait for pid thread to end 
		// u[i+1]= pid.result;//output value of insulin//
		// //ARMA//
		 // if (i >= 20)
		// {
			// arma.dataArray=dataArray;
			
			// if(pthread_create(&t_arma,NULL,arma_forecast,&arma))//creat arma thread
			// {
				// fprintf(stderr, "Error creating thread\n");
				// return 1;
			// }	
			// // sleep(1);
			// pid.pv=arma.result;
			// if(pthread_create(&t_pid,NULL,pid_control,&pid))//creat pid thread
					// {
						// fprintf(stderr, "Error creating thread\n");
						// return 1;
					// }
			// pthread_join(t_arma, NULL);//wait for arma thread to end	

			// pthread_join(t_pid, NULL);//wait for pid thread to end
			
			// u[i + 1] = pid.result;//
		// }
			// difference( i);//calculate next value of glucose
			// outFile << g[i] << ',' << u[i] << endl;
	// }
	while(1)
	{
	if(pthread_create(&t_sensor,NULL,get_sensor_data,NULL))//creat pid thread
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		sleep(1);
	if(pthread_create(&t_pid,NULL,pid_control,&pid))//creat pid thread
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		
	
	if(pthread_create(&t_arma,NULL,arma_forecast,&arma))//creat pid thread
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	
	if(pthread_create(&t_difference,NULL,difference,NULL))//creat pid thread
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	
		
		
	sleep(1);
	
	pthread_join(t_sensor, NULL);//wait for arma thread to end	
	pthread_join(t_pid, NULL);//wait for arma thread to end	
	pthread_join(t_arma, NULL);//wait for arma thread to end	
	pthread_join(t_difference, NULL);//wait for arma thread to end	

				// save data//
	}

    
    return 0;
	ofstream outFile;
	outFile.open("test.csv", ios::out);//open csv
	outFile << "Glucose" << ',' << "Insulin" << endl;
	for (i=0;i<timestep;i++)
	{outFile << g[i] << ',' << u[i] << endl;}
	outFile.close();
	return 0;
}