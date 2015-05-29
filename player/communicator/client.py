import socket

SOCK_ADDR = '/tmp/organo'

if __name__ == '__main__':
    
    while True:
        data = input('mensaje:').encode()
        sock = socket.socket(socket.AF_UNIX)
        sock.connect(SOCK_ADDR)
        sock.send(data)
