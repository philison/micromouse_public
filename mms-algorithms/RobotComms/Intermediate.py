# import socket

# def send_command(command):
#     try:
#         client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#         client_socket.connect(('127.0.0.1', 12345))  # Connect to the server IP and port
#         client_socket.send(command.encode())
#         client_socket.close()
#     except ConnectionRefusedError:
#         print("Error: The server is not running or is not accepting connections.")

# if __name__ == "__main__":
#     while True:
#         user_input = input("Enter a command (e.g., 'setColor 0 0 R' or 'exit' to quit): ")
        
#         if user_input.lower() == "exit":
#             break
        
#         send_command(user_input)


# import socket

# def send_command(command):
#     try:
#         client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#         client_socket.connect(('127.0.0.1', 12345))  # Connect to the server IP and port
#         client_socket.send(command.encode())

#         # Keep the socket open for continuous communication
#         while True:
#             user_input = input("Enter a command (e.g., 'setColor 0 0 R' or 'exit' to quit): ")
            
#             if user_input.lower() == "exit":
#                 client_socket.send("exit".encode())
#                 break
            
#             client_socket.send(user_input.encode())

#     except ConnectionRefusedError:
#         print("Error: The server is not running or is not accepting connections.")

# if __name__ == "__main__":
#     send_command("")  # Start sending commands, initially with an empty string


# The Intermediate becomes the server, and the Main becomes the client.
# import socket

# def main():
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind(('127.0.0.1', 12345))  # Use a specific IP and port
#     server_socket.listen(1)  # Listen for one incoming connection
#     print("Server is listening on 127.0.0.1:12345")

#     # Accept a client connection
#     client_socket, client_address = server_socket.accept()
#     print(f"Accepted connection from {client_address[0]}:{client_address[1]}")

#     while True:
#         user_input = input("Enter a message to send to the client (or 'exit' to quit): ")
        
#         if user_input.lower() == "exit":
#             break
        
#         # Send the user's input to the client
#         client_socket.send(user_input.encode())

#         # # Receive a response from the client
#         # response = client_socket.recv(1024).decode()
#         # print(f"Received from client: {response}")

#     # Close the client socket
#     client_socket.close()

# if __name__ == "__main__":
#     main()




import socket
import serial

def main():
    # Initialize the serial connection to /dev/rfcomm0
    try:
        serial_port = serial.Serial("/dev/rfcomm0", baudrate=115200)

    except serial.SerialException as e:
        print(f"Failed to open serial port: {e}")
        return

    # Start the server to communicate with the SIM client
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', 12345))
    server_socket.listen(1)
    print("Server is listening on 127.0.0.1:12345")

    # Accept a client connection
    client_socket, client_address = server_socket.accept()
    print(f"Accepted connection from {client_address[0]}:{client_address[1]}")

    try:
        while True:
            # Read a command from the serial port (e.g., "mF")
            command = serial_port.readline().decode().strip()
            # Print the received command to the console
            print(f"Received UART command: {command}")

            # Only send the command to the client if it is a valid command
            if command == "mF" or command == "tR" or command == "tL" or command.startswith("sW-") or command.startswith("sC-") or command.startswith("sT-"):
                # Add a destinctive character to the beginning of the command
                command = ">" + command
                # Send the received command to the client
                client_socket.send(command.encode())

            # Send the received command to the client
            # client_socket.send(command.encode())

            # Receive a response from the client
            # response = client_socket.recv(1024).decode()
            # print(f"Received from client: {response}")

    except Exception as e:
        print(f"Error: {e}")

    finally:
        serial_port.close()
        client_socket.close()

if __name__ == "__main__":
    main()


## Just print the received UART data
# import serial

# def main():
#     print(f"Starting Intermediate server...")
#     # Initialize the serial connection to /dev/rfcomm0
#     try:
#         # serial_port = serial.Serial("/dev/rfcomm0", baudrate=115200, timeout=1)
#         serial_port = serial.Serial("/dev/rfcomm0", baudrate=115200)
#     except serial.SerialException as e:
#         print(f"Failed to open serial port: {e}")
#         return

#     try:
#         while True:
#             # Read a command from the serial port (e.g., "mF")
#             command = serial_port.readline().decode().strip()

#             if not command:
#                 break

#             # Print the received command to the console
#             print(f"Received UART command: {command}")

#     except Exception as e:
#         print(f"Error: {e}")

#     finally:
#         serial_port.close()

# if __name__ == "__main__":
#     main()