#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <mongoose.h>
#include <megahal.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mhlock;
static int keep_running = 1;

static int begin_request_handler(struct mg_connection *conn) {

    const struct mg_request_info *request_info = mg_get_request_info(conn);
    char content[1024];
    int content_length = 0;

    if (strstr(request_info->uri, "/ask/") != 0) {
        const char *question = request_info->uri + 5;
        printf("Question: %s\n", question);
        pthread_mutex_lock(&mhlock);
        char *out = megahal_do_reply((char*)question, 0);
        pthread_mutex_unlock(&mhlock);

        printf("Reply: %s\n", out);
        strncpy (content, out, sizeof(content));
        content_length = strlen(content);

    } else if (strcmp(request_info->uri, "/learn/") == 0) {
        const char *question = request_info->uri + 7;
        printf("Learn: %s\n", question);

        pthread_mutex_lock(&mhlock);
        megahal_learn_no_reply((char*)question, 0);
        pthread_mutex_unlock(&mhlock);

        content_length = snprintf(content, sizeof(content), "learnt");
    } else {
        content_length = snprintf(content, sizeof(content), "No comprende");
    }

    mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"        // Always set Content-Length
            "\r\n"
            "%s",
            content_length, content);

    return 1;
} 

void cleanup(int sig) {
    keep_running = 0;
}

int main(int argc, char **argv) {

    char *port = "8080";
    char *mhdir = NULL;

    int c;
    while(1) {
        static struct option long_options[] = {
            { "help", no_argument, 0, '?' },
            { "port", optional_argument, 0, 'p' },
            { "dir", optional_argument, 0, 'd' },
            { 0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long(argc, argv, "?p:", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 'p':
                port = optarg;
                break;
            case 'd':
                mhdir = optarg;
                break;
            case '?':
                printf("usage: megahal-server --port=<port> --dir=<megahal-brain-directory>\n");
                exit(1);
        }
    }
    printf("megahal-server: port set to %s and dir set to %s\n",port,mhdir);

    struct mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = begin_request_handler;

    const char *options[] = {"listening_ports", port, NULL};

    pthread_mutex_init(&mhlock, NULL);

    if (mhdir) {
        megahal_setdirectory(mhdir);
    }

    megahal_initialize();
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    char *mhout = megahal_initial_greeting();

    printf("MegaHal initial greeting: %s\n", mhout);

    struct mg_context *ctx;
    ctx = mg_start(&callbacks, 0, options);

/*
    while(1) {
        char *i = megahal_input("> ");
        if (megahal_command(i) != 0) continue; 
        mhout = megahal_do_reply(i, 1);
        megahal_output(mhout);
    }
*/

    while(keep_running) {
        sleep(1);
    }

    printf("Stopping and cleaning up.\n");
    megahal_cleanup();
    mg_stop(ctx);

    return 0;
}
