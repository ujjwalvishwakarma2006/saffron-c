#include "common.h"
#include "args.h"

void print_usage(const char *program_name) {
    printf("Usage: %s --server|--client [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  --help            Show this help message\n");
    printf("  --server          Run in server mode\n");
    printf("  --client          Run in client mode\n");
    printf("  -ip <address>     IP address (default: %s)\n", server_ip);
    printf("  -mp <port>        Message Port number (default: %d)\n", msg_port);
    printf("  -fp <port>        File Port number (default: %d)\n", file_port);
}

// Function to setup the environment and some global variables based on command line inputs
// e.g. whether the application is being run as client or server? 
void setup(int argc, char* argv[]) {

    // Ensure that .saffron-temp directory exists
    int child_pid = fork();
    if (child_pid == 0) {
        char* args[] = {
            "mkdir", "-p", ".saffron-temp", NULL
        };

        execvp(args[0], args);

        exit(1);
    }

    waitpid(child_pid, NULL, 0);

    if ((argc < 2) || (strcmp(argv[1], "--server") != 0 && strcmp(argv[1], "--client") != 0)) {
        print_usage("app");
        exit(1);
    }

    display_name = strcmp(argv[1], "--server") == 0 ? "Client" : "Server";
    app_mode = strcmp(argv[1], "--server") == 0 ? SERVER : CLIENT;

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-ip") == 0) {
            if (i + 1 < argc) {
                server_ip = argv[++i];
            } else {
                print_usage("app");
                exit(1);
            }
        }
        else if (strcmp(argv[i], "-mp") == 0) {
            if (i + 1 < argc) {
                msg_port = atoi(argv[++i]);
                if (msg_port <= 0 || msg_port > 65535) {
                    print_usage("app");
                    exit(1);
                }
            } else {
                print_usage("app");
                exit(1);
            }
        }
        else if (strcmp(argv[i], "-fp") == 0) {
            if (i + 1 < argc) {
                file_port = atoi(argv[++i]);
                if (file_port <= 0 || file_port > 65535) {
                    print_usage("app");
                    exit(1);
                }
            } else {
                print_usage("app");
                exit(1);
            }
        }
        else if (strcmp(argv[i], "--help") == 0) {
            print_usage("app");
            exit(1);
        }
        else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            print_usage("app");
            exit(1);
        }
    }

    return;
}