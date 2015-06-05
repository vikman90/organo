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
    except OSError:
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

        if data == 'stop':
            break
    
    unlink()

def main():
    thread = threading.Thread(target=communicate)
    cond.acquire()
    thread.start()
    x = 0

    while True:
        if cond.wait(1.0):
            if data == 'pause':
                cond.wait()

                while True:
                    if data == 'play':
                        break
                    elif data == 'stop':
                        return
                    else:
                        cond.wait()
                    
            elif data == 'stop':
                break
            
        print(x)
        x += 1

    cond.release()

if __name__ == '__main__':
    main()
