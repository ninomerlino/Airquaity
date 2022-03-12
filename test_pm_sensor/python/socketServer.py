from socket import socket, AF_INET, SOCK_STREAM
import asyncio

async def handleSocket(client : socket, clientname):
    print("socket accepted")
    buffer_size = 1024
    msg = b''
    while buffer := client.recv(1024):
        msg += buffer;
    client.send("message recived".encode('utf-8'))
    client.close()
    print(f"FROM {clientname} MESSAGE : {msg.decode('utf-8')}")

async def serverLoop():
    print("server set up")
    server = socket(AF_INET, SOCK_STREAM)
    server.bind(("192.168.1.13",65000))
    server.listen(1)
    print("server ready")
    while True:
        try:
            sock, addr = server.accept()
            asyncio.get_running_loop().create_task(handleSocket(sock, addr))
        except KeyboardInterrupt:
            print("Closing socket server")
            break
    server.close()

asyncio.run(serverLoop())