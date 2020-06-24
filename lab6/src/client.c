#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "include/header.h"

int main(int argc, char **argv) {
    uint64_t k = -1;
    uint64_t mod = -1;
    char servers[255] = {'\0'};

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"k", required_argument, 0, 0},
                                        {"mod", required_argument, 0, 0},
                                        {"servers", required_argument, 0, 0},
                                        {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1)
        break;

        switch (c) {
        case 0: {
            switch (option_index) {
            case 0:
                if(!ConvertStringToUI64(optarg, &k))
                {
                    printf("k is a positive number\n");
                    return 1;
                }
                break;
            case 1:
                if(!ConvertStringToUI64(optarg, &mod))
                {
                    printf("mod is a positive number\n");
                    return 1;
                }
                break;
            case 2:
                if(!strlen(optarg))
                {
                    printf("servers isn't empty\n");
                    return 1;
                }
                memcpy(servers, optarg, strlen(optarg));
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
        } break;

        case '?':
            printf("Arguments error\n");
            break;
        default:
            fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == -1 || mod == -1 || !strlen(servers)) {
        fprintf(stderr, "Using: %s --k --mod --servers\n",
                argv[0]);
        return 1;
    }

    FILE *f;
    if( !(f = fopen(servers, "r")))
    {
        printf("invalid servers\n");
        return 1;
    }

    unsigned int servers_num = 0;
    struct Server *to = malloc(sizeof(struct Server) * 10);
    while(!feof(f))
    {
        fscanf(f, "%d", &to[servers_num].port);
        memcpy(to[servers_num].ip, "127.0.0.1", sizeof("127.0.0.1"));
        servers_num++;
    }
    servers_num--;
    int array_step = k/servers_num;
    int last_step = k%servers_num + array_step;
    int total = 1;

    int i;
    for (i = 0; i < servers_num; i++) {
        struct hostent *hostname = gethostbyname(to[i].ip);
        if (hostname == NULL) {
            fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
            exit(1);
        }

        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(to[i].port);
        server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

        int sck = socket(AF_INET, SOCK_STREAM, 0);
        if (sck < 0) {
            fprintf(stderr, "Socket creation failed!\n");
            exit(1);
        }

        if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
            fprintf(stderr, "Connection failed\n");
            exit(1);
        }

        uint64_t begin = 1 + i * array_step;
        uint64_t end = begin + (i == servers_num? last_step: array_step);

        char task[sizeof(uint64_t) * 3];
        memcpy(task, &begin, sizeof(uint64_t));
        memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
        memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

        if (send(sck, task, sizeof(task), 0) < 0) {
            fprintf(stderr, "Send failed\n");
            exit(1);
        }

        uint64_t answer = 0;
        if (recv(sck, &answer, sizeof(uint64_t), 0) < 0) {
            fprintf(stderr, "Recieve failed\n");
            exit(1);
        }

        total *= answer;
        close(sck);
    }
    free(to);
    total %= mod;
    printf("Answer: %d\n", total);

    return 0;
}
