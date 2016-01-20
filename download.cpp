#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "CS360Utils.h"
#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
#define HOST_NAME_SIZE      255
#define MAXGET 1000
int main(int argc, char *argv[])
{
    vector<char *> headerLines;
    char contentType[MAX_MSG_SZ];
    int hSocket;                 /* handle to socket */
    struct hostent *pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
    int debugCount = 0;
    int count = 1;
    int debug = 0;
    char opt;
	int downloaded = 0;
	int failed = 0;
    if (argc < 4 || argc > 6)
    {
        perror("\nUsage: download host-name host-port path -c or -d\n");
        return -1;
    }
    else
    {
    	while ((opt = getopt(argc, argv, "dc:")) != -1) 
    	{
            switch (opt) 
            {
                case 'd':
                    debug = 1;
                    break;
                case 'c':
                    count = atoi(optarg);
                    debugCount = 1;
                    break;
                default:
                	perror("flag does not exist");
                    abort();
            }
        }
        if(argc - optind != 3)
        	perror("\nUsage: download host-name host-port path -c or -d\n");
        strcpy(strHostName, argv[optind++]);
        std::string port = argv[optind++];
        if(port.find_first_not_of("0123456789") != string::npos)
        {
        	perror("Invalid port");
        	return -1;
        }
        else
        {
        	nHostPort = atoi(&port[0]);
        }
    }
	while(downloaded < count)
	{
	    /* make a socket */
	    hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	    if (hSocket == SOCKET_ERROR)
	    {
	        perror("\nCould not make a socket\n");
	        //return 0;
	    }
	    /* get IP address from name */
	    pHostInfo = gethostbyname(strHostName);
	    if(pHostInfo == NULL)
	    {
	    	perror("\nCouldn't connect to host\n");
	    	return -1;
	    }
	    /* copy address into long */
	    memcpy(&nHostAddress, pHostInfo->h_addr, pHostInfo->h_length);
	    /* fill address struct */
	    Address.sin_addr.s_addr = nHostAddress;
	    Address.sin_port = htons(nHostPort);
	    Address.sin_family = AF_INET;
	    /* connect to host */
	    if (connect(hSocket, (struct sockaddr *) &Address, sizeof(Address)) == SOCKET_ERROR)
	    {
	        perror("\nCould not connect to host\n");
	    }
	    // Create HTTP Message
	    char  *message = (char *) malloc(MAXGET);
	    sprintf(message, "GET %s HTTP/1.1\r\nHost:%s:%d\r\n\r\n", argv[optind], strHostName, nHostPort);
	    // Send HTTP on the socket
	    if(debug == 1)
	    printf("Request: %s\n", message);
	    write(hSocket, message, strlen(message));
	    // Rease Response back from socket
	    // First read the status line
	    char *startline = GetLine(hSocket);
	    if(debug == 1)
	    printf("Status line %s\n\n",startline);
		char status[2];
		status[0]=startline[strlen(startline)-2];
		status[1]=startline[strlen(startline)-1];
	    if(status[0] == 'n' && status[1] == 'd')
	    {
	    	perror("Invalid context, could not reach address");
	    	return -1;
	    }
	    // Read the header lines
	    headerLines.clear();
	    GetHeaderLines(headerLines, hSocket , false);
	    if(debug == 1)
	    {
	    	// Now print them out
		    for (int i = 0; i < headerLines.size(); i++) 
		    {
		        printf("[%d] %s\n",i,headerLines[i]);
		        if(strstr(headerLines[i], "Content-Type")) 
		        {
		            sscanf(headerLines[i], "Content-Type: %s", contentType);
		        }
		    }
		    printf("\n=======================\n");
		    printf("Headers are finished, now read the file\n");
		    printf("Content Type is %s\n",contentType);
		    printf("=======================\n\n");
		}
		if(debugCount == 0)
		{
		    // Now read and print the rest of the file
		    int rval;
		    try
		    {
			    while((rval = read(hSocket,pBuffer,MAX_MSG_SZ)) > 0) 
			    {
			        write(1,pBuffer,rval);
			    }
			}
			catch(...)
			{
				perror("Could not read socket");
			}
		}
	    /* close socket */
	    if (close(hSocket) == SOCKET_ERROR)
	    {
	        perror("\nCould not close socket\n");
	    }
	    if(status[0] == 'O' && status[1] == 'K')
	    	downloaded++;
	    else
	    	failed++;
	}
	    if(debugCount == 1)
	    printf("%d Downloaded,  %d Failed\n",downloaded,failed);
}
