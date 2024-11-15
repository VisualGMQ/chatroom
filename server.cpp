#include <iostream>
#include <vector>
#include <unordered_set>
#include "SDL.h"
#include "SDL_net.h"

int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_TIMER) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (SDLNet_Init() != 0)
    {
        std::cerr << "SDLNet_Init Error: " << SDLNet_GetError() << std::endl;
        return 2;
    }

    IPaddress address;
    SDLNet_ResolveHost(&address, INADDR_ANY, 4242);
    TCPsocket sock = SDLNet_TCP_Open(&address);
    if (!sock)
    {
        std::cerr << "SDLNet_TCP_Open Error: " << SDLNet_GetError() << std::endl;
    }
    SDLNet_SocketSet set = SDLNet_AllocSocketSet(512);

    bool shouldExit = false;
    std::unordered_set<TCPsocket> clients;
    std::vector<TCPsocket> shouldEraseClients;

    while (!shouldExit)
    {
        if (TCPsocket client = SDLNet_TCP_Accept(sock))
        {
            SDLNet_TCP_AddSocket(set, client);
            clients.insert(client);
        }

        int num = SDLNet_CheckSockets(set, 0);
        if (num <= 0)
        {
            if (num < 0 && !clients.empty())
            {
                std::cerr << "select() call failed: " << SDLNet_GetError() << std::endl;
            }
            continue;
        }

        for (auto client : clients)
        {
            char buf[1024] = {0};
            if (SDLNet_SocketReady(set))
            {
                int recvNum = SDLNet_TCP_Recv(client, buf, sizeof(buf));
                const IPaddress* addr = SDLNet_TCP_GetPeerAddress(client);
                if (recvNum > 0)
                {
                    std::cout << SDLNet_ResolveIP(addr) << ": ";
                    for (int i = 0; i < recvNum; i++)
                    {
                        std::cout << buf[i];
                    }
                    std::cout << std::endl;
                }
                else
                {
                    std::cout << SDLNet_ResolveIP(addr) << " leave" << std::endl;
                    SDLNet_TCP_Close(client);
                    SDLNet_TCP_DelSocket(set, client);
                    shouldEraseClients.push_back(client);
                }

                SDLNet_TCP_Send(client, buf, recvNum);
            }
        }

        for (auto client : shouldEraseClients)
        {
            clients.erase(client);
        }
        shouldEraseClients.clear();
    }

    SDLNet_TCP_Close(sock);
    SDLNet_FreeSocketSet(set);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
