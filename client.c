#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define LOG_FILE "chat.log"
char message[100];
char client_name[15];

//clientler arasi mesajları loglayan fonksiyon
void log_message(char * client_name, char * message) {
        FILE * fp = fopen(LOG_FILE, "a");
        if (fp != NULL) {
                time_t rawtime;
                time( & rawtime);
                struct tm * timeinfo = localtime( & rawtime);
                fprintf(fp, "[%d-%d-%d %d:%d:%d] %s %s\n",
                timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            	timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
            	client_name, message);

                fclose(fp);
        }
}

//soket uzerinden gonderilen mesajları alır devamlı mesaj almak için kapanana kadar sonsuz donguye girer
void * ReceiveChat(void * my_sock) {
        int sock = * ((int * ) my_sock);
        int length;
        while ((length = recv(sock, message, 100, 0)) > 0) {
                client_name[strcspn(client_name, "\n")] = 0;
                message[length] = '\0';
                message[strcspn(message, "\n")] = 0;
                printf("%s\n", message);
                log_message(client_name, message);

        }
}
//bir soket oluşturulur ve servera baglanmaya calısılır
int main(int argc, char * argv[]) {
        pthread_t recvt;
        int length;
        int sock;
        char send_message[100];
        char client_name[15];
        struct sockaddr_in Server;
        strcpy(client_name, argv[1]);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        Server.sin_addr.s_addr = inet_addr("127.0.0.1");
        Server.sin_family = AF_INET;
        Server.sin_port = htons(7878);
        if ((connect(sock, (struct sockaddr * ) & Server, sizeof(Server))) == -1)
                printf("\n soket baglantisi basarisiz oldu\n");
        else
                printf("%s baglantin basarili \n", client_name);
//mesaj gelmesini bekleyen client icin thread olusturuyoruz
        pthread_create( & recvt, NULL, (void * ) ReceiveChat, & sock);
//konsoldan mesaj okumaya hazırdır
        while (fgets(message, 10, stdin) > 0) {
                strcpy(send_message, client_name);
                strcat(send_message, "->");
                strcat(send_message, message);
                length = write(sock, send_message, strlen(send_message));

                if (length < 0)
                        printf("\n mesaj gonderilmedi \n");
        }

//thread kapatılır
        pthread_join(recvt, NULL);
//soket kapatılır
        close(sock);
        return 0;
}
