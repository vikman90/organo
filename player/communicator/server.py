import os
import os.path
import socket
import threading
import time

SOCK_ADDR = '/tmp/organo'
BACKLOG = 5
BUFF_SIZE = 4096

cond = threading.Condition()
data = ''

def unlink():
    try:
        os.remove(SOCK_ADDR)
    except FileNotFoundError:
        if os.path.exists(SOCK_ADDR):
            raise

def communicate():
    global data
    global cond
    
    unlink()
    server = socket.socket(socket.AF_UNIX)
    server.bind(SOCK_ADDR)
    server.listen(BACKLOG)

    while True:
        sock = server.accept()[0]
        data = sock.recv(BUFF_SIZE).decode()

        with cond:
            cond.notify()

        if data == b'stop':
            break
    
    unlink()

if __name__ == '__main__':
    thread = threading.Thread(target=communicate)
    cond.acquire()
    thread.start()
    x = 0

    while True:
        if cond.wait(1.0):
            if data == 'pause':

                while data != 'play':
                    cond.wait()
                    
            elif data == 'stop':
                break
            
        print(x)
        x += 1
        
