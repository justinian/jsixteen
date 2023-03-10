#include <new>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <j6/cap_flags.h>
#include <j6/errors.h>
#include <j6/flags.h>
#include <j6/init.h>
#include <j6/protocols/service_locator.h>
#include <j6/syscalls.h>
#include <j6/sysconf.h>
#include <j6/types.h>
#include <util/hash.h>

extern "C" {
    int main(int, const char **);
}

extern j6_handle_t __handle_self;
j6_handle_t g_handle_sys = j6_handle_invalid;

struct entry
{
    uint8_t bytes;
    uint8_t area;
    uint8_t severity;
    char message[0];
};

static const uint8_t level_colors[] = {0x00, 0x09, 0x01, 0x0b, 0x0f, 0x07, 0x08};
char const * const level_names[] = {"", "fatal", "error", "warn", "info", "verbose", "spam"};
char const * const area_names[] = {
#define LOG(name, lvl) #name ,
#include <j6/tables/log_areas.inc>
#undef LOG
    nullptr
};

void
send_all(j6_handle_t cout, char *data, size_t len)
{
    do {
        size_t sent = len;
        j6_status_t s = j6_channel_send(cout, data, &sent);
        if (s != j6_status_ok)
            return;

        len -= sent;
        data += sent;
    } while (len);
}

void
print_header(j6_handle_t cout)
{
    char stringbuf[150];

    unsigned version_major = j6_sysconf(j6sc_version_major);
    unsigned version_minor = j6_sysconf(j6sc_version_minor);
    unsigned version_patch = j6_sysconf(j6sc_version_patch);
    unsigned version_git = j6_sysconf(j6sc_version_gitsha);

    size_t len = snprintf(stringbuf, sizeof(stringbuf),
            "\e[38;5;21mjsix OS\e[38;5;8m %d.%d.%d (%07x) booting...\e[0m\r\n",
            version_major, version_minor, version_patch, version_git);

    send_all(cout, stringbuf, len);
}

void
log_pump_proc(j6_handle_t cout)
{
    size_t buffer_size = 0;
    void *message_buffer = nullptr;
    char stringbuf[300];

    j6_status_t result = j6_system_request_iopl(g_handle_sys, 3);
    if (result != j6_status_ok)
        return;

    while (true) {
        size_t size = buffer_size;
        j6_status_t s = j6_system_get_log(g_handle_sys, message_buffer, &size);

        if (s == j6_err_insufficient) {
            free(message_buffer);
            buffer_size = size * 2;
            message_buffer = malloc(buffer_size);
            continue;
        } else if (s != j6_status_ok) {
            j6_log("uart driver got error from get_log");
            continue;
        }

        const entry *e = reinterpret_cast<entry*>(message_buffer);

        const char *area_name = area_names[e->area];
        const char *level_name = level_names[e->severity];
        uint8_t level_color = level_colors[e->severity];

        size_t len = snprintf(stringbuf, sizeof(stringbuf),
                "\e[38;5;%dm%7s %7s: %s\e[38;5;0m\r\n",
                level_color, area_name, level_name, e->message);
        send_all(cout, stringbuf, len);
    }
}

int
main(int argc, const char **argv)
{
    j6_log("logging server starting");


    g_handle_sys = j6_find_first_handle(j6_object_type_system);
    if (g_handle_sys == j6_handle_invalid)
        return 1;

    j6_handle_t slp = j6_find_first_handle(j6_object_type_mailbox);
    if (g_handle_sys == j6_handle_invalid)
        return 2;

    j6_handle_t cout = j6_handle_invalid;

    for (unsigned i = 0; i < 100; ++i) {
        uint64_t tag = j6_proto_sl_find;
        uint64_t proto_id = "jsix.protocol.stream.ouput"_id;

        j6_status_t s = j6_mailbox_call(slp, &tag, &proto_id, &cout);
        if (s == j6_status_ok &&
            tag == j6_proto_sl_result &&
            cout != j6_handle_invalid)
            break;

        cout = j6_handle_invalid;
        j6_thread_sleep(10000); // 10ms
    }

    if (cout == j6_handle_invalid)
        return 3;

    print_header(cout);
    log_pump_proc(cout);
    return 0;
}

