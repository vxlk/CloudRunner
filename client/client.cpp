/* 
 * tcp client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <string>
#include <fstream>

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(const char *msg) {
    perror(msg);
    exit(0);
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
    printf("%s", toBeReturned.c_str());
    return toBeReturned;
}

char getMessage(std::string og)
{
    return og[0];
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    unsigned int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    std::string fileName;

    /* check command line arguments */
    if (argc != 4) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);
    fileName = argv[3];


    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        printf("ERROR, no such host as %s\n", hostname);
        exit(0);
    }
    /* move h_addr to ip_addr. This enables conversion to a form        */
    /* suitable for printing with the inet_ntoa() function.             */
    struct in_addr ip_addr;
    ip_addr = *(struct in_addr *)(server->h_addr);
    //printf("ping %s (%s)\n", hostname, inet_ntoa(ip_addr));
   
    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
    serverlen = sizeof(serveraddr);
    
    if (connect(sockfd, (sockaddr*)&serveraddr, serverlen) == -1)
      //  error("failed on connect");


    //send ack
    printf("sending file name\n");
    std::string updatedFileName(appendMessage(fileName, 'A'));
    printf("%s THIS IS THE FILE NAME", updatedFileName.c_str());
    n = sendto(sockfd, updatedFileName.c_str(), strlen(updatedFileName.c_str()), 0, (sockaddr*)&serveraddr, serverlen);
     if (n < 0) 
          error("ERROR in sendACK");
    printf("waiting for ack\n");
    bzero(buf,BUFSIZE);
    //wait for ACK
     n = recvfrom(sockfd, buf, BUFSIZE, 0, (sockaddr*)&serveraddr, &serverlen);
     if (n < 0) 
     {
        printf("error in recieving ack\n");
     }

     std::string buff(buf);
     if (getMessage(buff) != 'A') printf("ack wrong\n%s\n", buf);

     //wait for file name
     printf("waiting for file name\n");
     n = recvfrom(sockfd, buf, BUFSIZE, 0, (sockaddr*)&serveraddr, &serverlen);
     if (n < 0) 
     {
        printf("error in recieving server file name\n");
     }

     printf("server file name: %s\n", removeMessage(std::string(buf)).c_str());
     //resend the file name for the child
     sendto(sockfd, buf, strlen(buf), 0, (sockaddr*)&serveraddr, serverlen);

    //send the message
    std::ifstream f(fileName);
    if (!f.is_open()) printf("error in opening file\n");
    std::string buffer;
    while(std::getline(f, buffer))
    {
        printf("%s", buffer.c_str());
        buffer = appendMessage(buffer, 'D');
        n = sendto(sockfd, buffer.c_str(), strlen(buffer.c_str()), 0, (sockaddr*)&serveraddr, serverlen);
        if (n < 0) 
          error("ERROR in sendto");
        printf("waiting for ack\n");
        n = recvfrom(sockfd, buf, BUFSIZE, 0, (sockaddr*)&serveraddr, &serverlen);
        if (n < 0) 
        {
            printf("did not recieve ack from server\n");
        }
        std::string bufff(buf);
        if (getMessage(bufff) != 'A') break;
    }
    std::string END = "END";
    END = appendMessage(END, 'E');
    sendto(sockfd, END.c_str(), strlen(END.c_str()), 0, (sockaddr*)&serveraddr, serverlen);
    return 0;
}
