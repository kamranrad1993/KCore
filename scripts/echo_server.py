import socket
from time import sleep

HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 65432  # Port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        conn.sendall(b"hi")
        # sleep(1)
        # conn.sendall(b"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa++++++++++++++++++++++++++++++++++++++++++++++++++++++++bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb***********************************************************cccccccccccccccccccccccccccccccccccccccccccccccc")
        # sleep(2)
        # conn.close
        while True:
            data = conn.recv(1024)
            if not data:
                break
            conn.sendall(data)

# cretae socket server class and pass the port number
# class SocketServer(object):
#     def __init__(self, port):
#         self.port = port
#         self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#         self.sock.bind((HOST, self.port))
#         self.sock.listen(1)
#         self.conn, self.addr = self.sock.accept()
#         print(f"Connected by {self.addr}")
#         self.conn.sendall(b"hi")
#         sleep(1)

    # def send_data(self, data):
    #     self.conn.sendall(data)
    #     sleep(1)
    #     self.conn.close
    #     self.sock.close()

    # def close(self):
    #     self.conn.close()
    #     self.sock.close()

    # def __del__(self):
    #     self.conn.close()
    #     self.sock.close()


