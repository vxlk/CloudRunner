/* 
 * Concurrent server using c++11 threads
 * Tyler Small
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <thread>
#include <pthread.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

/*
Adil's ack idea
*/
std::string appendMessage(std::string original, char msg)
{
    std::string toBeReturned = "";
    toBeReturned += msg;
    return toBeReturned + original;
}

std::string removeMessage(std::string original)
{
    std::string toBeReturned = "";
    for (int i = 1; i < original.size(); ++i)
        toBeReturned += original[i];
    return toBeReturned;
}

char getMessage(std::string og)
{
    return og[0];
}

/* Global Variables */
int sockfd; /* socket */
int portno; /* port to listen on */
unsigned int clientlen; /* byte size of client's address */
struct sockaddr_in serveraddr; /* server's addr */
struct sockaddr_in clientaddr; /* client addr */
struct hostent *hostp; /* client host info */
char buf[BUFSIZE]; /* message buf */
char *hostaddrp; /* dotted decimal host addr string */
int optval; /* flag value for setsockopt */
int n; /* message byte size */
char* ACK = "ACK";
char* fileNameForChild = new char[512]; //max file size is 255 so this is safe

//function argument to std::thread -> what each 
//thread will do during execution
void threadWork(int newSock, char* buf)
{
	 
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 999999;

  //open file 
  std::string newFileName(buf);
  newFileName = newFileName.substr(0, newFileName.find('.'));
  newFileName += (std::to_string(newSock)) + ".txt";

  //send file name
  newFileName = appendMessage(newFileName, 'A');
  n = sendto(newSock, newFileName.c_str() , strlen(newFileName.c_str()), 0, 
     (struct sockaddr *) &clientaddr, clientlen);
  if (n < 0) 
    error("ERROR in sending File name confirmation");

  //get file name for child
  recvfrom(newSock, buf, BUFSIZE, 0,
   (struct sockaddr *) &clientaddr, &clientlen);

  std::string dataBuf(buf);
  dataBuf = removeMessage(dataBuf);
  std::ifstream fin(dataBuf); //should be ok
  std::ofstream fout(dataBuf);
  if (!fin.is_open() && !fout.is_open()) printf("child could not make file:%s\n", dataBuf.c_str());
  printf("%s made\n", dataBuf.c_str());

  while(1)
  {
    
    //clear buf before read
    bzero(buf, BUFSIZE);
    //read
    recvfrom(newSock, buf, BUFSIZE, 0,
    (struct sockaddr *) &clientaddr, &clientlen);
    std::string message(buf);
    //check for end signifier from client
    if (getMessage(message) == 'E') break;

    message = removeMessage(message);
    //reverse and write
    std::reverse(message.begin(), message.end());
    std::string fileContents = "";
    std::string temp = "";
    while (std::getline(fin,temp))
      fileContents+= temp + '\n';
    fileContents += message + '\n';
    fout << fileContents.c_str();
    printf("%s", fileContents.c_str());
    sendto(newSock, ACK , strlen(ACK), 0, 
     (struct sockaddr *) &clientaddr, clientlen);
  }
  fin.close();
  fout.close();
  printf("\nfile can be opened\n");
    
}

int main(int argc, char **argv) {

  srand(time(NULL));

  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
 
  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  if (listen(sockfd, 10)==-1) error("error on listen");
  /* 
   * main loop: wait for a datagram, then echo it
   */

  clientlen = sizeof(clientaddr);
  int pid;
  int newSock;
  while (1) {
      
    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);

    //change to child socket
    newSock = accept(sockfd, (struct sockaddr*)&clientaddr, &clientlen);
    if (newSock < 0)
    { 
    	perror("new sock problem");
    	break;
    }

    n = recvfrom(newSock, buf, BUFSIZE, 0,
  		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom in main loop");

    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
  	
  	std::string rcvBuff(buf);
  	rcvBuff = removeMessage(rcvBuff);
  	
    printf("server received %s from ip(port):%s(%u)\n",  rcvBuff.c_str(), hostaddrp, ntohs(clientaddr.sin_port));
    
    n = sendto(newSock, ACK, strlen(ACK), 0, 
	       (struct sockaddr *) &clientaddr, clientlen);
    if (n < 0) 
      error("ERROR in sending ACK");


    //error check  - thread -> close sock
  	bzero(fileNameForChild, 512);
  	strcpy(fileNameForChild, rcvBuff.c_str());
  	//open the thread after ack is sent
  	std::thread newThread(threadWork, newSock, fileNameForChild);
  	//detach the thread from the loop -> don't want 
  	//destructor called every iteration...
  	newThread.detach();

  }
  return 0;
}
