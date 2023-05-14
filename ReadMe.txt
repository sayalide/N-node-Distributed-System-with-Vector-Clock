Team Members:
1)Ajay Venkatesha (1001861936)
2)Sayali Dilip Deshmukh(1001966628)

======================================

Requirements:
Visual Studio Code
======================================

Programming Language: 
C++

======================================

Execution and Instructions:


1)Compilation and Execution  of Part 1:

Code Compilation: 
Open new terminal and enter following commands where the file is loacated
g++ server.cpp -o server -std=c++11
g++ client.cpp -o client -std=c++11

Code Execution of Part 1:
1) To start the server, open a new terminal and type ./server
2) To create client 1, open a new terminal and type ./client
3) On the server console, enter 0 to add extra clients (Terminal 1)
4) To create client 2, open a new terminal and type ./client
5) Once the two clients have been created, enter 1 to begin the algorithm.


2)Compilation and Execution  of Part 2:
Code Compilation:
Open a new terminal and enter the following commands where the file is located
g++ CausalOrdered.cpp -o CausalOrdered -lpthread

Code Execution of Part 2:
We have set the number of processes to 3 to make coding easier.
Run each procedure in a different terminal using following sequence

In terminal 1 enter ./CausalOrdered 1
In terminal 2 enter ./CausalOrdered 2
In terminal 3 enter ./CausalOrdered 3

