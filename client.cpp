#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>

#define PORT 8002
#define MAX_BUFFER 1024


int strlen(char* string)        //i dont like string.h
{
    int len = 0;
    while (string[0] != '\0')
    {
        string++;
        len++;

    }
    return len;
}   

bool check_for_end(char* buffer, int read_limit)
{
    for (int i = 0; i < read_limit; i++)
    {
        if(buffer[0] == '\0')
        {
            buffer = buffer - 2;
            if (buffer[0] == '0')
                return true;
            buffer = buffer + 2;
        }
        buffer++;
    }

    return false;
}


int main ()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port  = htons(PORT);
    socket_addr.sin_addr.s_addr = INADDR_ANY;

    connect(server_socket, (struct sockaddr*)&socket_addr, sizeof(socket_addr)); 


    //boiler plate cpp socket ^

    char header[64] = "Client of  "; 
    const char* username = getlogin();
    strcat(header, username);

    char message2[] = "\nQuery file name\n";
    send(server_socket, header, strlen(header), 0);
    send(server_socket, message2, strlen(message2), 0);


    char read_buffer[MAX_BUFFER];
    recv(server_socket, read_buffer, MAX_BUFFER, 0);
    std::cout.write(read_buffer, strlen(read_buffer)) << '\n' << std::flush;

    while(true)
    {
        char buffer[MAX_BUFFER];

        std::cout << "Send message to the server: " << std::flush;
        std::cin.getline(buffer,MAX_BUFFER);
        //  socket fd,      buffer, sizeof buffer, flag;
        send(server_socket, buffer, strlen(buffer), 0);
        if (strncmp(buffer, "Start", 5) == 0)
        {
            
            while (true)
            {

                char read_buffer[MAX_BUFFER];
                int size_n = recv(server_socket, read_buffer, MAX_BUFFER, 0);
                std::cout.write(read_buffer, size_n) << std::flush;
                if(check_for_end(read_buffer, size_n))
                {
                    close(server_socket);
                    exit(EXIT_SUCCESS);
                }
                   

                
            }

        }
    }

    return 0;
}