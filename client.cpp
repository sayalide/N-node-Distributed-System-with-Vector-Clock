#include <iostream>
#include <unistd.h>
#include <stdlib.h> 
#include <arpa/inet.h>
#include <vector>
#include <sys/socket.h>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <stdio.h>
#define PORT 8080 
using namespace std; 

vector<string> split(string StringS, string StringDelimiterLength) 
{
    size_t StartPosition = 0, EndPosition, LengthOfDelimiter = StringDelimiterLength.length(); 
    string StringToken;
    vector<string> VectorResult;

    while ((EndPosition = StringS.find (StringDelimiterLength, StartPosition)) != string::npos) 
    {
        StringToken = StringS.substr (StartPosition, EndPosition - StartPosition); 
        StartPosition = EndPosition + LengthOfDelimiter; 
        VectorResult.push_back (StringToken); 
    }

    VectorResult.push_back (StringS.substr (StartPosition)); 
    return VectorResult; 
}
int main(int argc, char const *argv[])
{
    srand((unsigned int)time(NULL));
   
    float LocalClockOfClient = rand() % 10; 
    printf("Client process started. \n");
    printf("Local clock of this client is -  %f \n\n", LocalClockOfClient);
    
    int ClientFileSocket;
    struct sockaddr_in AddressOfServer;
    char BufferOfClient[1024] = {0};
    long  var;
    AddressOfServer.sin_family = AF_INET; 
    AddressOfServer.sin_port = htons(PORT);  
    if ((ClientFileSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nFailed Socket Connection...\n");
        return -1;
    }

    if(inet_pton(AF_INET, "127.0.0.1", &AddressOfServer.sin_addr)<=0) 
    {
        printf("\nInvalid address\n");
        return -1;
    }

    if (connect(ClientFileSocket, (struct sockaddr *)&AddressOfServer, sizeof(AddressOfServer)) < 0)
    {
        printf("\nFailed Socket Connection...\n");
        return -1;
    }
    char IpServer[INET_ADDRSTRLEN]="";
    inet_ntop(AF_INET, &AddressOfServer.sin_addr, IpServer, INET_ADDRSTRLEN);

    printf("Client is connected to the server! \n");
    printf("\n\n");
    var = read( ClientFileSocket , BufferOfClient, 1024);

    printf("%s\n\n",BufferOfClient );

    string MessageReceived = string(BufferOfClient); 
    if (strcmp(BufferOfClient, "Message from SERVER - Requesting your local clock value") == 0) {   
        string MessageString = "Message from CLIENT - my local clock value is -  " + to_string(LocalClockOfClient);
        char CharacterMessageArray[MessageString.length() + 1];
        strcpy(CharacterMessageArray, MessageString.c_str());
        send(ClientFileSocket , &CharacterMessageArray , strlen(CharacterMessageArray) , 0 ); 
        printf("\nSent message to the server with local clock value - %f", LocalClockOfClient);
    }
    var = read( ClientFileSocket , BufferOfClient, 1024); 
    printf("\n\n%s\n\n",BufferOfClient );
    MessageReceived = string(BufferOfClient);

    if (MessageReceived.find("Message from SERVER - The offset of your clock is -  ") != string::npos)
    {
        string SubstringAfterSpace;
        string SubstringAfterLastSpace;
        vector<string> SplitString = split(MessageReceived, " "); 
        SubstringAfterSpace = SplitString[ SplitString.size() - 2 ];
        SubstringAfterLastSpace = SplitString[ SplitString.size() - 1 ];

        cout << "Change the value of this client's clock by offset:  " << SubstringAfterSpace << " " << SubstringAfterLastSpace << endl;
        float SubstringAfterLastSpaceFloat = stof(SubstringAfterLastSpace);
        char CharacterOpenArray[SubstringAfterSpace.length() + 1];
        strcpy(CharacterOpenArray, SubstringAfterSpace.c_str());
        if (strcmp(CharacterOpenArray, "add") == 0 )
        {
            LocalClockOfClient += SubstringAfterLastSpaceFloat;
        }else if (strcmp(CharacterOpenArray, "minus") == 0 )
        {
            LocalClockOfClient -= SubstringAfterLastSpaceFloat;
        } 
        printf("Local Clock of this client is - %f \n\n", LocalClockOfClient);
    }
    close(ClientFileSocket);
    return 0;
   
}



/*** References ***/
/***
https://github.com/DayuanTan/multicast-programming-multicast-ordering
https://github.com/DayuanTan/berkeley-algorithm-implementation/tree/main/p1_berkeley_server_clients

 ***/