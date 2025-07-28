#include <winsock2.h>
#include <stdio.h> 
#include <ws2tcpip.h> 
#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 512
/*NOTE TO SELF
use AF_INET which is ivP4
And SOCK_STREAM for a normal TCP connection 
For protocol make it 0 as defualt*/

//python -m http.server 27015
//.\client_server.exe 127.0.0.1 


int main(int argc, char *argv[]){
//   Initializing Winsock  //

WSADATA wsadata; // The WSADATA structure contains information about the Windows Sockets implementation.
/*  WSAStartup takes two parameter 
1. is wVersionRequired, This is the highest version of Windows Sockets specification that the caller can use
2. is A pointer to the WSADATA data structure that is to receive details of the Windows Sockets implementation.
This will return a int to check for errors. Zero for sucess */ 
int Ipresult = WSAStartup(MAKEWORD(2,2),&wsadata); 
if(Ipresult!= 0){
    printf("WSAStartup failed %d\n", Ipresult);
    return 1;
}
//      Creating a socket for the client        //

struct addrinfo *result = NULL,//	Stores the linked list of resolved addresses 
*ptr = NULL, // Use to traverse the result when linking with multiple ip's
hints;// specifics of what kind of addresses we want
ZeroMemory(&hints,sizeof(hints)); // cleares the value inside hints ie clear it of all the grbage values
    hints.ai_family = AF_INET;  // ai.family restricts IP versions and here we choose IvP4
    hints.ai_socktype = SOCK_STREAM; // Defines the socket type and we choose TCP
    hints.ai_protocol = 0; // Explicitly chooses TCP/UDP (or lets the system decide), So i let the system decide

/*The getaddrinfo function provides protocol-independent translation from an ANSI host name to an address, Its parameters are:
1. [in, optional] pNodeName: A pointer to a NULL-terminated ANSI string containing a host name or numeric IP address (IPv4 dotted-decimal or IPv6 hex).
2. [in, optional] pServiceName: contains either a service name or port number represented as a string. A service name is a string alias for a port number.
3. [in, optional] pHints: A pointer to an addrinfo structure that provides hints about the type of socket the caller supports.
4. [out] ppResult: A pointer to a linked list of one or more addrinfo structures that contains response information about the host
*/
Ipresult = getaddrinfo(argv[1],DEFAULT_PORT,&hints,&result);
if(Ipresult!=0){
    printf("Failed getadder %d",Ipresult);
    WSACleanup();//The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll)
    return 1;
}
// create a socket for connecting to server
SOCKET connectSocket = INVALID_SOCKET;

ptr = result;
while(ptr != NULL){

//The socket function creates a socket that is bound to a specific transport service provider.
connectSocket = socket(ptr-> ai_family, ptr-> ai_socktype, ptr->ai_protocol);
// check for errors 
if(connectSocket == INVALID_SOCKET){
    ptr = ptr-> ai_next;
    continue;
}
//      Connecting to a Socket      //


if(connect(connectSocket,ptr-> ai_addr, (int)ptr->ai_addrlen)!= SOCKET_ERROR){ // if connect succesfully 
    break;
}
// if not, then close socket and try next
closesocket(connectSocket);// close the exisisting socket if error occurs
connectSocket = INVALID_SOCKET;// rest the value to old one
ptr = ptr->ai_next;
}

freeaddrinfo(result);

if(connectSocket == INVALID_SOCKET){
    printf("Unable to connect to any server: %d\n", WSAGetLastError());
    WSACleanup();
    return 1;
}

//      Sending and Receiving Data on the Client        //
int recivebuflen = DEFAULT_BUFLEN;
const char *sendbuf = "This is a test";
char recivebuf[DEFAULT_BUFLEN];
/*The send function sends data on a connected socket
1.[in] SOCKET, [in] buf, [in] len, [in] flags*/
Ipresult = send(connectSocket,sendbuf, (int)strlen(sendbuf),0);
if(Ipresult == SOCKET_ERROR){
    printf("send failed: %d\n", WSAGetLastError());
    closesocket(connectSocket);
    WSACleanup();
    return 1;
}
printf("Bytes Sent: %ld\n", Ipresult);
// shutdown the connection for sending since no more data will be sent
// the client can still use the ConnectSocket for receiving data
Ipresult = shutdown(connectSocket,1); // this will showdown the socket but only the send operation
if(Ipresult == SOCKET_ERROR){
    printf("Shutdown failed %d\n", WSAGetLastError());
    closesocket(connectSocket);
    WSACleanup();
    return 1;
}
// Receive data until the server closes the connection

do{
    Ipresult = recv(connectSocket,recivebuf,recivebuflen,0);
    if(Ipresult >0) printf("Bytes received: %d\n", Ipresult);
    else if(Ipresult == 0) printf("closed connection\n");
    else printf("recv failed: %d\n", WSAGetLastError());
}while (Ipresult > 0);

closesocket(connectSocket);
WSACleanup();

return 0;
}
