#include <stdio.h>
#include <string.h>

#include "a1_lib.h"
#include "message.h"

#define BUFSIZE   3072

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid number of arguments");
        exit(1);
    }
    int sockfd;
    char user_input[BUFSIZE] = {0};
    char server_msg[BUFSIZE] = {0};

    if (connect_to_server(argv[1], atoi(argv[2]), &sockfd) < 0) {
        fprintf(stderr, "oh no\n");
        return -1;
    }
    while (1) {
        memset(user_input, 0, sizeof(user_input));
        memset(server_msg, 0, sizeof(server_msg));
        struct message parsed_msg = {0};

        printf("Enter commands: ");
        fgets(user_input, BUFSIZE, stdin);
        char *token = strtok(user_input, " ");
        int count = 0;
        while (token != NULL) {
            if (count == 0) {
                strcpy(parsed_msg.command, token);
            } else if (count == 1) {
                parsed_msg.arg1 = atoi(token);
            } else if (count == 2) {
                parsed_msg.arg2 = atoi(token);
            }
            count += 1;
            token = strtok(NULL, " ");
        }

        if (strcmp(parsed_msg.command, "exit\n") == 0) {
            break;
        }

        // send the input to server
        int status = send_message(sockfd, (char *) &parsed_msg, sizeof(parsed_msg));
        if (status == -1) {
            fprintf(stderr, "error sending the message to backend.");
        }
        // receive a msg from the server
        ssize_t byte_count = recv_message(sockfd, server_msg, sizeof(server_msg));
        if (strcmp(parsed_msg.command, "shutdown\n") == 0) {
            printf("Server: %s\n", server_msg);
            break;
        }
        if (byte_count <= 0) {
            break;
        }
        printf("Server: %s\n", server_msg);
    }

    return 3;
}

