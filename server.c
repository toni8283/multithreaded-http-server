#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void *handle_client(void *arg)
{
    int client_socket = *((int *)arg);
    free(arg);

    char buffer[BUFFER_SIZE] = {0};

    // Receive HTTP request from browser
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received <= 0)
    {
        close(client_socket);
        return NULL;
    }

    printf("Received request:\n%s\n", buffer);

    // Open the HTML file
    FILE *file = fopen("public/index.html", "r");

    if (file == NULL)
    {
        const char *response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 18\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 File Not Found";

        send(client_socket, response, strlen(response), 0);
        close(client_socket);

        return NULL;
    }

    // Find file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate memory for HTML
    char *file_content = malloc(file_size + 1);

    if (file_content == NULL)
    {
        fclose(file);
        close(client_socket);
        return NULL;
    }

    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';

    fclose(file);

    // Create HTTP response headers
    char header[BUFFER_SIZE];

    int header_length = snprintf(
        header,
        BUFFER_SIZE,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        file_size
    );

    // Send headers
    send(client_socket, header, header_length, 0);

    // Send HTML file
    send(client_socket, file_content, file_size, 0);

    free(file_content);

    close(client_socket);

    return NULL;
}


int main()
{
    int server_socket;

    struct sockaddr_in server_address;

    // Create TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // Allow quick server restart
    int option = 1;

    setsockopt(
        server_socket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &option,
        sizeof(option)
    );

    // Configure server address
    server_address.sin_family = AF_INET;

    server_address.sin_addr.s_addr = INADDR_ANY;

    server_address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(
            server_socket,
            (struct sockaddr *)&server_address,
            sizeof(server_address)
        ) < 0)
    {
        perror("Bind failed");
        close(server_socket);

        return EXIT_FAILURE;
    }

    // Start listening
    if (listen(server_socket, 10) < 0)
    {
        perror("Listen failed");
        close(server_socket);

        return EXIT_FAILURE;
    }

    printf("====================================\n");
    printf(" Multithreaded HTTP Server\n");
    printf("====================================\n");
    printf("Server running on http://localhost:%d\n", PORT);
    printf("Waiting for connections...\n\n");


    while (1)
    {
        struct sockaddr_in client_address;

        socklen_t client_length = sizeof(client_address);

        int *client_socket = malloc(sizeof(int));

        if (client_socket == NULL)
        {
            perror("Memory allocation failed");
            continue;
        }

        // Accept incoming connection
        *client_socket = accept(
            server_socket,
            (struct sockaddr *)&client_address,
            &client_length
        );

        if (*client_socket < 0)
        {
            perror("Accept failed");
            free(client_socket);
            continue;
        }

        printf(
            "New connection from %s:%d\n",
            inet_ntoa(client_address.sin_addr),
            ntohs(client_address.sin_port)
        );

        // Create a new thread
        pthread_t thread;

        if (pthread_create(
                &thread,
                NULL,
                handle_client,
                client_socket
            ) != 0)
        {
            perror("Thread creation failed");

            close(*client_socket);

            free(client_socket);

            continue;
        }

        // Automatically clean thread resources when finished
        pthread_detach(thread);
    }

    close(server_socket);

    return EXIT_SUCCESS;
}