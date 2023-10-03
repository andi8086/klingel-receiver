#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>

#include <SDL.h>
#include <SDL_mixer.h>

Mix_Chunk *bell_sound = NULL;
int udp_sock = -1;

void cleanup()
{
        printf("cleanup...\n");
        if (udp_sock != -1) {
                close(udp_sock);
        }
        Mix_FreeChunk(bell_sound);

}


int main(void)
{
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
                return 1;
        }

        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        bell_sound = Mix_LoadWAV("sheep.wav");

        if (!bell_sound) {
                return 1;
        }

        atexit(cleanup);

        /* start udp listener on port 12345 */

        udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (!udp_sock) {
                return 1;
        }

        struct sockaddr_in sa;
        
        sa.sin_family = AF_INET;
        sa.sin_port = htons(12345);
        sa.sin_addr.s_addr = inet_addr("0.0.0.0");

        if (bind(udp_sock, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
                printf("Could not bind socket\n");
                return 1;
        }

        const char *dingdong = "dingdong";
        char msg[9];

        printf("Bell sound is playing for test\n");
        Mix_PlayChannel(-1, bell_sound, 1);

        do {
                struct pollfd fds[1];
                fds[0].fd = udp_sock;
                fds[0].events = POLLIN;
                
                if (poll(fds, 1, 1000) <= 0) {
                        if (errno == EINTR) {
                                break;
                        }
                        continue;
                };
                memset(msg, 0, sizeof(msg)); 
                if (recvfrom(udp_sock, msg, sizeof(msg),
                             0, NULL, NULL) == strlen(dingdong)) {
                        if (strcmp(msg, dingdong)) {
                                continue;
                        }
                        int chan = Mix_PlayChannel(-1, bell_sound, 1);
                        while (Mix_Playing(chan));
                }
        } while (true);

        return 0;
}
