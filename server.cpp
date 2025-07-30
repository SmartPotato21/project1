#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <chrono>

#define PORT 8002
#define MAX_BUFFER 1024
#define READ_MAX_BUFFER 64      //bytes of file to send

int handleClient(int client_fd)
{
    std::cout << "Client has connected and given file descriptor " << client_fd << std::endl;
    std::cout << "Waiting for Start from client" << std::endl;
    
    FILE* fp;
    fp = fopen("server_file.txt", "rb");
    if (fp == nullptr) return 1;
    
    
    fseek(fp,0, SEEK_END);
    int file_size = ftell(fp);
    rewind(fp);

    while (true)
    {
        char buffer[MAX_BUFFER] = {0};
        int size_n = recv(client_fd,buffer, MAX_BUFFER, 0);
        if (size_n == 0)
        {
            return 1;
        }

        if (strncmp(buffer, "Start", 5) == 0)       //on start recieve:
        {
            long read_count = 0;                           
            char read_buffer[READ_MAX_BUFFER];

            while (file_size - read_count > 0)      //read file into buffer, send those buffers
            {
                char header_buffer[] = "\n1\n";     //first message in pair
                send(client_fd, header_buffer, 4, 0);

                int to_read = std::min((long)READ_MAX_BUFFER, file_size - read_count);                
                int read_number = fread(read_buffer, 1, to_read, fp);               
                read_count = read_count + read_number;

                send(client_fd, read_buffer, (int)read_number,0);
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); //make text apear slowly
            }

            char end_of_send[] = "\n0\n";
            send(client_fd, end_of_send, 4, 0);
            send(client_fd, end_of_send, 4, 0);

            fclose(fp);
            close(client_fd);
            return 1;
        }
        else if(strncmp(buffer, "Client", 6) == 0)  //on client's first message:
        {
            std::cout.write(buffer, size_n) << std::flush;
           
            char* server_name = getlogin();
            char response[256] = "Server's name: ";
            char file_name_response[] = "\nFile name: server_file.txt";
            char file_size_response[] = "\nFile size: ";
            char file_size_buffer[64];
            sprintf(file_size_buffer, "%d", file_size);
            

            strcat(response, server_name);
            strcat(response, file_name_response);
            strcat(response, file_size_response);
            strcat(response, file_size_buffer);
            send(client_fd, response, strlen(response), 0);
        }
        else
        {
            std::cout.write(buffer, size_n) << std::flush;
        }
    }



    return 1;
}



int main ()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port  = htons(PORT);
    socket_addr.sin_addr.s_addr = INADDR_ANY;


    bind(server_socket, (struct sockaddr*)&socket_addr, sizeof(socket_addr));

    listen(server_socket, 4);
    while (true)
    {
        int client_fd = accept(server_socket, nullptr, nullptr);    //main thread blocked here
        std::cout << client_fd;
        std::thread t(handleClient,client_fd);
        t.detach();                                 //thread dies when returned.
    }


    return 0;
}