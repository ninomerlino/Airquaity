from socket import socket, AF_INET, SOCK_STREAM
import asyncio

async def handleSocket(client : socket, clientname):
    print(f"socket accepted {clientname}")
    buffer_size = 1024
    msg = b''
    while buffer := await asyncio.get_running_loop().sock_recv(client, buffer_size):
        msg += buffer;
    client.send("message recived".encode('ascii'))
    client.close()
    print(f"FROM {clientname} MESSAGE : {msg.decode('utf-8')}")

async def serverLoop():
    print("server set up")
    server = socket(AF_INET, SOCK_STREAM)
    server.bind(("192.168.1.13",65000))
    server.setblocking(False)
    server.listen(1)
    print("server ready")
    while True:
        try:
            sock, addr = await asyncio.get_running_loop().sock_accept(server)
            asyncio.get_running_loop().create_task(handleSocket(sock, addr))
        except KeyboardInterrupt:
            print("Closing socket server")
            break
    server.close()

asyncio.run(serverLoop())