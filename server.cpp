#include <iostream> 
#include <iomanip> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <vector> 
#include <ctime> 
#include <cstdlib> 
#include <unistd.h> 

#define PORT 8080 
using namespace std; 


    vector<string> split(string s, string d) 
    {
    size_t StartPosition = 0;

    size_t EndPosition; 

    size_t LengthOfDelimiter = d.length(); 

    string StringToken;

    vector<string> VectorResult;

    while ((EndPosition = s.find (d, StartPosition)) != string::npos) 
	{

        StringToken = s.substr (StartPosition, EndPosition - StartPosition); 

        StartPosition = EndPosition + LengthOfDelimiter; 

        VectorResult.push_back (StringToken); 

    }
    VectorResult.push_back (s.substr (StartPosition)); 

    return VectorResult; 
    }

    int main(int argc, char *argv[])
    {   
	float LocalClockOfServer = rand() % 10; 

    srand((unsigned int)time(NULL)); 

    vector<float> ClientLocalClock;

    printf("Hey!Sever has been started...\n");

    printf("Server Local CLock -  %f \n\n", LocalClockOfServer);

    
    vector<string> clientIPS;

    vector<int> ClientPort;

    vector<int> ClientSocket;

    int ServerFileSocket, NewSocket;


    struct sockaddr_in AddressOfServer;

    AddressOfServer.sin_family = AF_INET;    

    AddressOfServer.sin_addr.s_addr = INADDR_ANY; 

    AddressOfServer.sin_port = htons( PORT ); 
    int Option  = 1; 
    if ((ServerFileSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
        {
        perror("Failed Socket Connection..."); 
        exit(EXIT_FAILURE);
        }
   if (setsockopt(ServerFileSocket, SOL_SOCKET, SO_REUSEADDR ,&Option, sizeof(Option)))
        {
        perror("Failed Socket Connection..."); 

        exit(EXIT_FAILURE);
        }
    if (::bind(ServerFileSocket, (struct sockaddr *)&AddressOfServer, sizeof(AddressOfServer))<0)
        {
        perror("Failed Socket Binding..."); 

        exit(EXIT_FAILURE);
    }
    if (listen(ServerFileSocket, 7) < 0) 
    {
        perror("Failed listening on the server..."); 

        exit(EXIT_FAILURE);
        }

    char ReceivedBuffer[65536];


    int CtrClient = 0;

    printf("Server is listening...\n\nStart the client processes.\n");

    memset(ReceivedBuffer, '\0', sizeof(ReceivedBuffer));

    int EnoughClient = 0;


    while ( EnoughClient == 0) 
    { 
        struct sockaddr_in AddressOfClient;

        socklen_t length = sizeof(AddressOfClient);

        if ((NewSocket = accept(ServerFileSocket, (struct sockaddr *)&AddressOfClient, 
                        (socklen_t*)&length))<0)
        {
            perror("Accepting client failed!");

            exit(EXIT_FAILURE);
        }
        CtrClient ++;

        printf("\nNumber of clients connected -  %d ", CtrClient);

        char IpClient[INET_ADDRSTRLEN] = "";

        inet_ntop(AF_INET, &AddressOfClient.sin_addr, IpClient, INET_ADDRSTRLEN);

        printf("\nNew client accepted!");

        ClientSocket.push_back(NewSocket);

        clientIPS.push_back(IpClient);

        ClientPort.push_back(ntohs(AddressOfClient.sin_port));

        printf("Updated number of connected clients - %d \n", int(ClientSocket.size()) );
        cout << "To add more clients press 0 \nTo continue with process, press 1\n" ;

        cin >> EnoughClient;
        if (EnoughClient == 0){
            cout << "Continue creating new clients\n" << endl;

        }else if (EnoughClient != 1)
        {
            cout << "Your can create more clients by opening new terminal\n" << endl;
            EnoughClient = 0;
        }
    }
        printf("\nClients creation finished\nTotal number of connected clients - %d\n\n", int(ClientSocket.size()) );
        printf("Requesting all the clients for their local clock value:\n"); 

        \for (int a = 0; a < ClientSocket.size(); a++){
        const char *msg = "Message from SERVER - Requesting your local clock value";
        send(ClientSocket[a] , msg , strlen(msg) , 0 );
        while(recv(ClientSocket[a], ReceivedBuffer, sizeof(ReceivedBuffer), 0) > 0 ){
            string recv_msg = string(ReceivedBuffer);
            if (recv_msg.find("Message from CLIENT - my local clock value is - ") != string::npos){
                string substr_after_last_space;
                vector<string> split_str = split(recv_msg, " "); 
                substr_after_last_space = split_str[ split_str.size() - 1 ];

                cout << "Receiving local clock of client  -  " << substr_after_last_space << endl;
                float substr_after_last_space_f = stof(substr_after_last_space);
              

                ClientLocalClock.push_back(substr_after_last_space_f);
            }

            memset(ReceivedBuffer, '\0', strlen(ReceivedBuffer));
            break;
        }
    }
    printf("\n\n");
    float all_clock_sum = LocalClockOfServer;
    for (int a = 0; a < ClientLocalClock.size(); a++)
    {
        all_clock_sum += ClientLocalClock[a];
    }
    float clockAvg = all_clock_sum / (ClientSocket.size() + 1);
    for (int a = 0; a < ClientSocket.size(); a++){
        
        float temp = ClientLocalClock[a] - clockAvg;
        string func;
        if (temp >= 0){
            func = "minus";
        }else{
            func = "add";
            temp = 0 - temp;
        }
        string StringMessage = "Message from SERVER - The temp of your clock is -   " + func + " " + to_string(temp);
        char ArrayMessageChar[StringMessage.length() + 1];
        strcpy(ArrayMessageChar, StringMessage.c_str());
        
        send(ClientSocket[a] , &ArrayMessageChar , strlen(ArrayMessageChar) , 0 );
        
    
    }
    LocalClockOfServer += clockAvg - LocalClockOfServer;
    printf("After applying Berkeley's algorithm\nThe updated local clock of this server is -  %f \n\n", LocalClockOfServer);
    printf("\n");
    close(ServerFileSocket);
    return 0;
}

/*** References ***/
/***
https://github.com/DayuanTan/multicast-programming-multicast-ordering
 ***/