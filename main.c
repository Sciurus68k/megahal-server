#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mongoose.h>
#include <megahal.h>
#include <pthread.h>

pthread_mutex_t mhlock;

static int begin_request_handler(struct mg_connection *conn) {

    const struct mg_request_info *request_info = mg_get_request_info(conn);
    char content[1024];
    int content_length = 0;

    if (strcmp(request_info->uri, "/ask") == 0) {
        const char *qs = request_info->query_string;
        char question[1024];
        mg_get_var(qs, strlen(qs == NULL ? "" : qs), "q", question, sizeof(question));
        printf("Question: %s\n", question);

        pthread_mutex_lock(&mhlock);
        char *out = megahal_do_reply(question, 0);
        pthread_mutex_unlock(&mhlock);

        printf("Reply: %s\n", out);
        strncpy (content, out, sizeof(content));
        content_length = strlen(content);
    } else if (strcmp(request_info->uri, "/learn") == 0) {
        const char *qs = request_info->query_string;
        char learn[1024];
        mg_get_var(qs, strlen(qs), "q", learn, sizeof(learn));
        printf("Learn: %s\n", learn);

        pthread_mutex_lock(&mhlock);
        megahal_learn_no_reply(learn, 0);
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

int main(void) {

    char *mhdir = NULL;

    struct mg_context *ctx;
    struct mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = begin_request_handler;

    const char *options[] = {"listening_ports", "8081", NULL};

    pthread_mutex_init(&mhlock, NULL);

    if ((mhdir = getenv("MEGAHAL_DIR"))) {
        megahal_setdirectory(mhdir);
    }

    megahal_initialize();
    char *mhout = megahal_initial_greeting();

    printf("MegaHal initial greeting: %s\n", mhout);

    ctx = mg_start(&callbacks, 0, options);

    while(1) {
        char *i = megahal_input("> ");
        if (megahal_command(i) != 0) continue; 
        mhout = megahal_do_reply(i, 1);
        megahal_output(mhout);
    }

    megahal_cleanup();
    mg_stop(ctx);

    return 0;
}
