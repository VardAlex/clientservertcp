#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int client = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(5400);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    // connecting to "server" using "client" socket
    connect(client, (sockaddr *) &server, sizeof(server));
    send(client, "hello world", 12, 0);
    close(client);
    return 0;
}

/*
 Создаем сокет протокола IPv4 типа SOCK_STREAM который по умолчанию работает в TCP.
 Его дескриптор записываем в listening.
 Создаем структуру server типа sockaddr_in для описания скоета. В нее записываем тип адресса, порт в котором будет работаьь сокет и IP адресс (с помощью функции inet_pton).
 Привязываем указанную ранее информацию к сокету командой bind.
 Вводим сокет в режим прослешивания с максимальной длинной очередери равной 1.
 На стороне клиента инициализхируем сокет и задаем адрес для подключения.
 С помощью команды accept ждем соединения со стороны клиента. В свою очередь клиент подключается к серверу функцией connect.
 На сервере клиентский сокет дублируется в clientSocket.
 После закрываем сокет для прослушивания и функциями send и recv отправляем и принимаем сообщение соответственно.
 Закрываем оставшиеся сокеты на стороне сервера и клиента.
*/