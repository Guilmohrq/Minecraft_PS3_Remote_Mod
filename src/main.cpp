
#include <sys/prx.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

SYS_MODULE_INFO(MC_Remote, 0, 1, 1);
SYS_MODULE_START(module_start);

void write_mem(uint32_t address, uint32_t value) {
    uint32_t* addr = (uint32_t*)address;
    *addr = value;
}

void server_thread(uint64_t arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080); 

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    while(true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket >= 0) {
            memset(buffer, 0, 1024);
            read(new_socket, buffer, 1024);
            
            // --- Comandos do Mod ---
            if (strstr(buffer, "god_on"))     write_mem(0x00A7D354, 0x38000001);
            if (strstr(buffer, "god_off"))    write_mem(0x00A7D354, 0x900B0000);
            if (strstr(buffer, "items_on"))   write_mem(0x001556C0, 0x60000000);
            if (strstr(buffer, "items_off"))  write_mem(0x001556C0, 0x4BFFFF01);
            if (strstr(buffer, "fly_on"))     write_mem(0x00B0FFE0, 0x38600001);
            if (strstr(buffer, "fly_off"))    write_mem(0x00B0FFE0, 0x4BFFFF01);

            const char* res = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: 2\r\n\r\nOK";
            send(new_socket, res, strlen(res), 0);
            close(new_socket);
        }
        usleep(100000);
    }
}

extern "C" int module_start(uint64_t arg) {
    sys_ppu_thread_t tid;
    sys_ppu_thread_create(&tid, server_thread, 0, 1000, 4096, 0, "MC_Server");
    return 0;
}
