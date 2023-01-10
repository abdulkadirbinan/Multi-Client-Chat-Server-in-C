#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex;
int client[10];
int n = 0;
//mesajlari clientler icin gonderir
void ServerClient(char * message, int var) {
        int i;
        char client_name[15];
        pthread_mutex_lock( & mutex);
        for (i = 0; i < n; i++) {
                if (client[i] != var) {
                        if (send(client[i], message, strlen(message), 0) < 0) {
                                printf("\n hatali gonderim\n");
                                continue;
                        }
                }
        }
        pthread_mutex_unlock( & mutex);
}
//mesajlari alır ve bir sonraki mesajin alinmasini bekler 
void * ReceiveChat(void * client_sock) {
        int sock = * ((int * ) client_sock);
        char message[100];
        char client_name[15];
        int length;
        while ((length = recv(sock, message, 100, 0)) > 0) {
                message[length] = '\0';
                ServerClient(message, sock);
        }

}
//server icin soket oluşturulur
int main() {
        struct sockaddr_in Server;
        pthread_t recvt;
        int sock = 0, Client_sock = 0;

        Server.sin_family = AF_INET;
        sock = socket(AF_INET, SOCK_STREAM, 0);
        Server.sin_addr.s_addr = inet_addr("127.0.0.1");
        Server.sin_port = htons(7878);
        if (bind(sock, (struct sockaddr * ) & Server, sizeof(Server)) == -1)
                printf("baglanamiyor tekrar deneyin \n");
        else
                printf("chat uygulamasi baslatildi\n");

        if (listen(sock, 20) == -1)
                printf("dinleme basarisiz\n");

        while (1) {
                if ((Client_sock = accept(sock, (struct sockaddr * ) NULL, NULL)) < 0)
                        printf("kabul edilemedi \n");
                pthread_mutex_lock( & mutex);
                client[n] = Client_sock;
                n++;
//her client için thread oluşturuyoruz 
                pthread_create( & recvt, NULL, (void * ) ReceiveChat, & Client_sock);
                pthread_mutex_unlock( & mutex);
        }
        return 0;

}
