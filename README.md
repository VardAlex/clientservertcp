# clientservertcp
SERVER

Вызываем функцию socket (int __domain, int __type, int __protocol) которая создает сокет типа __type в домене __domain используя протокол __protocol (если значение 0 то используется стандартный протокол для используемого типа сокета). Функция возвращает файловый дескриптор который мы записываем в listening.

Вызывается функция __sys_socket(int family, int type, int protocol) определенная в linux/net/socket.c
В ней создаем структуру типа socket (описанную в linux/net.h), определяем необходимые для установки флаги. Вызываем функцию sock_create(int family, int type, int protocol, struct socket **res) которая в свою очередь вызывает __sock_create(current->nsproxy->net_ns, family, type, protocol, res, 0) где current->nsproxy->net_ns сетевой контекст текущего процесса.
Проверяем находятся ли переданные значения в нужных диапазонах. Далее вызываем функцию security_socket_create которая в свою очередь вызывает call_int_hook из security/security.c для безопасной работы с системными вызовами (LSM).
Выделяем сокет с помощью функции sock_alloc и обрабатываем возможные возникшие ошибки.
Возвращаемся в __sys_socket которая в конце вызывает и возвращает результат функции sock_map_fd. В свою очередь sock_map_fd устанавливает определенные ранее флаги, определяет файловый дескриптор в sock_alloc_file и возвращает его

Создаем структуру sockaddr_in server из netinet/in.h и устанавливаем значения которые мы привяжем к сокету через параметры sin_family, sin_port и sin_addr (семейство, адресов, порт и адрес соответственно).

Привязку выполняем с помощью вызова функции bind(listening, (sockaddr *) &server, sizeof(server)). Переходим к функции __sys_bind(int fd, struct sockaddr __user *umyaddr, int addrlen) из net/socket.c
Копируем сокет через файловый дескриптов функцией sockfd_lookup_light. move_addr_to_kernel с помощью audit_sockaddr копирует адрес в пространство ядра и связывает его с сокетом функцией security_socket_bind которая вызывает call_int_hook. В случае ошибки вызывается метод bind структуры ops (proto_ops которая используется для специфических операций с сокетами) которая является одним из параметров сокета

listen(listening, 1) - устанавливаем "прослушку" соединений вызывая функцию __sys_listen(int fd, int backlog). Снова копируем сокет функцией sockfd_lookup_light. Определяем максимальное количество соединений и устанавливаем сокет в режим прослушки функцией security_socket_listen.

accept(listening, NULL, NULL) - функция __sys_accept4 создает новый сокет, устанавливает соединение с клиентом и возвращает копию подключенного файлового дескриптора. Адреса коннекторов собираются в пространстве ядра и передаются пользователю в самом конце. Внутри функции мы создаем копию слушающего файлового дескриптора и передаем ее вместе с остальными параметрами в функцию __sys_accept4_file. После проверки флагов создается новый сокет и задаются его параметры. Вызывается функция security_socket_accept или же в случае неудачи метод sock->ops->accept. Адресс клиента и его длинна записывается в upeer_sockaddr и upeer_addrlen соответственно.

recv(clientSocket, buf, 256, 0) - получаем фрейм из сокета. Вызывается __sys_recvfrom (2 последних параметра addr и addr_len равняются NULL). Внутри используются структуры iovec для разброра/сборки ввода-вывода и msghdr для описания сообщений. Вызываем функцию import_single_range из linux/uio.h. Устанавливаем значение полей структуры msg (msghdr). Принимаем сообщение из сокета функцией sock_recvmsg что в свою очередь вызывает security_socket_recvmsg. Копируем адрес в пространство пользователя функцией move_addr_to_user.

close закрывает связанный с дескриптором файл.

CLIENT

Аналогичным образом создает сокет.

connect(client, (sockaddr *) &server, sizeof(server)) - подключается к сокету адрес которого записан в server. Вызывается функция __sys_connect. move_addr_to_kernel копирует переданный адрес в ядро. Далее вызывается __sys_connect_file где в свою очередь вызывается security_socket_connect который в последствии снова вызовет call_int_hook.

Функция send работает аналогично recv: __sys_sendto ->  sock_sendmsg -> security_socket_sendmsg

Во всех вышеперечисленных функциях очевидно в процессе обрабатываются всемозможные ошибки. 

old:
Создаем сокет протокола IPv4 типа SOCK_STREAM который по умолчанию работает в TCP.
Его дескриптор записываем в listening.
Создаем структуру server типа sockaddr_in для описания сокета. В нее записываем тип адресса, порт в котором будет работаьь сокет и IP адресс (с помощью функции inet_pton).
Привязываем указанную ранее информацию к сокету командой bind.
Вводим сокет в режим прослешивания с максимальной длинной очередери равной 1.
На стороне клиента инициализхируем сокет и задаем адрес для подключения.
С помощью команды accept ждем соединения со стороны клиента. В свою очередь клиент подключается к серверу функцией connect.
На сервере клиентский сокет дублируется в clientSocket.
После закрываем сокет для прослушивания и функциями send и recv отправляем и принимаем сообщение соответственно.
Закрываем оставшиеся сокеты на стороне сервера и клиента.
