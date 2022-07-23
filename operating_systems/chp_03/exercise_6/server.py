import socket
import threading

# This example creates threads and continuously runs after each connection
# and prints the message of the day until you use Kill (PID) or CTRL-C to
# kill the process.  My suggestion is to run the program from a terminal using
# the command "python server.py &" so that it runs in the background.

#------ Broadcast address, (0.0.0.0) is used for when running on cloud server
HOST = "127.0.0.1" #<--- Standard loopback interface address (localhost) (127.0.0.1)
PORT = 65432  # Port to listen on (non-privileged ports are > 1023)

#----- Other Constants -----!!
HEADER = 64
FORMAT = 'utf-8'
MESG_DAY = "Be Strong\n But Not Rude\n\nBe Kind But\n Not Weak\n\nBe Humble\n But Not Timid\nUtf-8 Encoded string  ==  "


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
    server_socket.bind( (HOST, PORT) )


    # Protocol for sending message with a HEADER
    def send(conn, msg):
        message = msg.encode(FORMAT)
        msg_length = len(message)
        send_length = str(msg_length).encode(FORMAT)
        send_length += b' ' * (HEADER - len(send_length))

        
        conn.send(send_length)
        conn.send(message)

        # These are test cases below
        # conn.send("Be Strong\n But Not Rude\n\nBe Kind But\n Not Weak\n\nBe Humble\n But Not Extra Stuff Added Here".encode(FORMAT))
        # conn.send("Be Strong\n But Not Rude\n\nBe Kind But\n Not Weak\n\nBe Humble\n But".encode(FORMAT))
        


    def handle_client(conn, addr):
        print(f"\n[NEW CONNECTION] {addr} connected")
        with conn:
            print( f"Connected by {addr}" )

            # Send Message to server
            send(conn, MESG_DAY)
        
        # Close connection
        conn.close()


    def start():
        server_socket.listen()
        while True:
            #----This part is blocking -----
            ( conn, addr ) = server_socket.accept()
            thread = threading.Thread( target=handle_client, args=(conn, addr) )
            thread.start()
            print(f"[ACTIVE CONNECTIONS] {threading.active_count() - 1}")


    print( "\n[STARTING] server is starting" )
    start()

    
    