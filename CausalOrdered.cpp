#include<stdio.h>
#include<unistd.h>
#include<queue>
#include<cstdlib>
#include<iostream>
#include<sstream>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<string>
#include<strings.h>
#include<signal.h>
#include<queue>
using namespace std;


void MessageProcessing(int ReceviedMessageFrom,int cc[],int length);
string DisplayVClock(int cc[]);



const int numberOfMulticasts = 4;
int ClockVector[3] = {0,0,0};
int Ports[3] = {5050, 5060, 5070};
int *AddressSocket;
int ThreadNumber = 0;

struct MessageStruct{
 int SourceReceiver;
 int clock[3];
};


struct MessageDeliveryStruct{
  bool DelayFlag;
  int ThreadNumber;
  int ClockVector[3];
};



queue<MessageStruct> CacheQ;
const int CacheSize = 200;
queue<MessageDeliveryStruct> QueueDelivery;
void *MessageReceiver(void *args)
{
int SocketForward = *(int *)args;
char Cache[CacheSize] = {0};
int SocketNew;
int ClockOne;
int ClockTwo;
int ClockThree;
int ReceviedMessageFrom;
struct sockaddr_in AddOfNewClient;
int SizeofAddOfNewClient = sizeof(AddOfNewClient);
  while(true){
    if((SocketNew = accept(SocketForward,(struct sockaddr *)&AddOfNewClient,(socklen_t*)&SizeofAddOfNewClient)) < 0)
{
   printf("A problem occurred when attempting to connect the process to the network.");
      exit(0);
    }
    else{
        read(SocketNew,Cache,CacheSize);
        string ReceviedStringMessage = strtok(Cache,",");
        string ClockOneString = strtok(NULL,",");
        string ClockTwoString = strtok(NULL,",");
        string ClockThreeString = strtok(NULL,",");
        ReceviedMessageFrom = std::stoi(ReceviedStringMessage);
        ClockOne = std::stoi(ClockOneString);
        ClockTwo = std::stoi(ClockTwoString);
        ClockThree = std::stoi(ClockThreeString);
        int VClockReceiver[3] = {ClockOne,ClockTwo,ClockThree};
        MessageProcessing(ReceviedMessageFrom,VClockReceiver,3);
        bzero(Cache,CacheSize);
        string DisplayResult = "Messages Received";
        send(SocketNew,DisplayResult.c_str(),DisplayResult.size(),0);
        close(SocketNew);
      }
  }

  return NULL;
}

        void *MessageSender(void *args){
            for(int temp =0 ;temp<numberOfMulticasts;temp++)
            {
              sleep(3);
              ClockVector[ThreadNumber-1]++;
              cout << "Broadcasting messsages to every processes simultaneously -  : " << DisplayVClock(ClockVector) << "\n";
              int RndNum;
              for(int num=1;num<=3;num++)
              {
                if(num != ThreadNumber)
                {
                  RndNum = rand() % 2;
                  struct MessageDeliveryStruct m;
                  if(RndNum == 0)
                  {
                    m.DelayFlag = true;
                  }else
                  {
                    m.DelayFlag = false;
                  }
                  m.ThreadNumber = num;
                  m.ClockVector[0] = ClockVector[0];
                  m.ClockVector[1] = ClockVector[1];
                  m.ClockVector[2] = ClockVector[2];
                  QueueDelivery.push(m);
                }
              }
            }
            return NULL;

          }


void MessageProcesser(int ThreadNumForSeding,int c[])
{
  int FSocket;
  char Cache[CacheSize] = {0};
  struct sockaddr_in AddressOfServer;
  int SizeofAddOfNewClient = sizeof(AddressOfServer);
  FSocket = socket(AF_INET,SOCK_STREAM,0);
  int portNumber = 0;
  AddressOfServer.sin_family = AF_INET;
  AddressOfServer.sin_port = htons(Ports[ThreadNumForSeding-1]);
  inet_pton(AF_INET,"127.0.0.1",&AddressOfServer.sin_addr);
  connect(FSocket,(struct sockaddr *)&AddressOfServer,sizeof(AddressOfServer));
  std::ostringstream stream;
  stream << ThreadNumber << ",";
  for (int num=0;num<3;num++) 
  {
      stream << c[num] << ",";
  }
  std::string token(stream.str());
  send(FSocket,token.c_str(),token.size(),0);
  read(FSocket,Cache,CacheSize);
  bzero(Cache,CacheSize);
  close(FSocket);
}
          void MessageProcessing(int ReceviedMessageFrom,int cc[],int length)
          {
            int befClock[3];
            for(int num=0;num<length;num++){
                  befClock[num] = ClockVector[num];
            }
            bool TestOne= false;
            bool TestTwo = true;
              if(ClockVector[ReceviedMessageFrom-1] + 1 == cc[ReceviedMessageFrom-1])
              {
                  TestOne= true;
              }else
              {
                for(int num=0;num<length;num++)
                {
                  if(num != ReceviedMessageFrom-1)
                  {
                    if(cc[num] > ClockVector[num])
                    {
                      TestTwo = false;
                      break;
                    }
                  }
                }
              }

              if(TestOne&& TestTwo)
              {

              ClockVector[ReceviedMessageFrom-1] = cc[ReceviedMessageFrom-1];
                cout << "Message received from the process: " << ReceviedMessageFrom << ", client clock received : " << DisplayVClock(cc) << ", Before Sending Message: : "<< DisplayVClock(befClock) << ", After Sending Message: "<< DisplayVClock(ClockVector) << "\n";
              }else
              {
                cout << "Message received from the process: " << ReceviedMessageFrom << ", client clock received : " << DisplayVClock(cc) <<  ", ClockDisplay : "<< DisplayVClock(ClockVector) << "\n";
                cout << "Adding this message to the Cache \n";
                struct MessageStruct m ;
                m.SourceReceiver = ReceviedMessageFrom;
                m.clock[0] = cc[0];
                m.clock[1] = cc[1];
                m.clock[2] = cc[2];
                CacheQ.push(m);
                
              }
          }

            void *MultiCastMsg(void *args){
              while(true)
              {
                while (!QueueDelivery.empty()) 
                {
                  struct MessageDeliveryStruct m = QueueDelivery.front();
                  if(m.DelayFlag){
                    QueueDelivery.pop();
                    m.DelayFlag = false;
                    QueueDelivery.push(m);
                  }else{
                    MessageProcesser(m.ThreadNumber,m.ClockVector);
                    QueueDelivery.pop();
                  }
                }
                sleep(4);
              }
              return NULL;
            }


        void *MessageDelivery(void *args)
        {
          while(true)
          {
            while (!CacheQ.empty()) 
            {
              int ClockBuff[3];
              for(int num=0;num<3;num++)
              {
                    ClockBuff[num] = ClockVector[num];
              }
              bool TestOne= false;
              bool TestTwo = true;
              struct MessageStruct m = CacheQ.front();
              if(ClockVector[m.SourceReceiver-1] + 1 == m.clock[m.SourceReceiver-1]){
                  TestOne= true;
              }else{
                for(int j=0;j<3;j++){
                  if(j!= m.SourceReceiver-1){
                    if(m.clock[j] > ClockVector[j])
                    {
                      TestTwo = false;
                      break;
                    }
                  }
                }
              }

            if(TestOne&& TestTwo)
            {
              ClockVector[m.SourceReceiver-1] = m.clock[m.SourceReceiver-1];
              cout << "Message received from the process: " << m.SourceReceiver << ", client clock received: " << DisplayVClock(m.clock) << ", Before Sending Message: "<< DisplayVClock(ClockBuff) << ", After Sending Message: "<< DisplayVClock(ClockVector) << "\n";
              CacheQ.pop();
            }else
            {

              CacheQ.pop();
              CacheQ.push(m);
            }

          }
          sleep(1);
        }
        return NULL;
      }


            string DisplayVClock(int dclock[])
            {
              string output = "[";
              for(int temp=0;temp<3;temp++)
              {
                if(temp == 2){
                  output.append(std::to_string(dclock[temp]));
                }
                else{
                  output.append(std::to_string(dclock[temp]));
                  output.append(",");
                }
              }
              output.append("]");
              return output;
            }


            void CheckLink(int a)
            {
              cout << "\n Exiting the Program  \n" << endl;
              if(close(*AddressSocket) != 0)
              {
                printf("Socket closing failed\n");
              }else
              {
                printf("Succesfully closed socket\n");
              }
              exit(a);
            }
            void *CloseLink(void *args)
            {
              signal(SIGINT, CheckLink);
              return NULL;
            }
              int main(int Temp1,char* Temp2[])
              {
                if(Temp1 < 1){
                  printf("Restart after entering the process number correctly.\n");
                  exit(1);
                }

                ThreadNumber = atoi(Temp2[1]);
                pthread_t senderThd;
                pthread_t messageDelivery;
                pthread_t receiverThd;
                pthread_t terminateThd;
                pthread_t bufferDelivery;

              struct sockaddr_in AddressOfServer;
              int SizeofAddOfNewClient = sizeof(AddressOfServer);
              int FSocket = socket(AF_INET, SOCK_STREAM, 0);
              
                AddressSocket = &FSocket;
              AddressOfServer.sin_family = AF_INET;
              AddressOfServer.sin_port = htons(Ports[ThreadNumber-1]);
              AddressOfServer.sin_addr.s_addr = INADDR_ANY;
              if(::bind(FSocket, (struct sockaddr *)&AddressOfServer, sizeof(AddressOfServer)) == 0){
              printf("Successfully binded socket & port!\n");
              } else{
              printf("issues with connecting the socket to the port \n");
              exit(0);
              }
                if(listen(FSocket,100) < 0){
                  printf("Problems with the Listener occurred\n");
                  exit(1);
                }
                pthread_create(&senderThd,NULL,&MessageReceiver,&FSocket);
                pthread_create(&terminateThd,NULL,&CloseLink,NULL);
                sleep(5);  
                pthread_create(&receiverThd,NULL,&MessageSender,NULL);
                pthread_create(&bufferDelivery,NULL,&MessageDelivery,NULL);
                pthread_create(&messageDelivery,NULL,&MultiCastMsg,NULL);
                pthread_join(senderThd,NULL);
                pthread_join(receiverThd,NULL);
                pthread_join(terminateThd,NULL);
                pthread_join(bufferDelivery,NULL);
                pthread_join(messageDelivery,NULL);
                return 0;
                }

/*** References ***/
/***
https://github.com/DayuanTan/multicast-programming-multicast-ordering
https://github.com/deepikarajani24/Time-Synchronization-in-distributed-systems
 ***/