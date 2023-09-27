# import API
# import sys
# import socket

# def log(string):
#     sys.stderr.write("{}\n".format(string))
#     sys.stderr.flush()

# def main():
#     log("Running...")

#     # Initialize a socket server to listen for commands
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind(('127.0.0.1', 12345))  # Use a specific IP and port
#     server_socket.listen(1)  # Listen for one incoming connection
#     log("Listening for commands on 127.0.0.1:12345")

#     # # Accept a client connection
#     # client_socket, client_address = server_socket.accept()
#     # log(f"Accepted connection from {client_address[0]}:{client_address[1]}")

#     while True:
#         # Accept a client connection
#         client_socket, client_address = server_socket.accept()
#         log(f"Accepted connection from {client_address[0]}:{client_address[1]}")
#         # Receive a command from the client
#         command = client_socket.recv(1024).decode()

#         if command == "exit":
#             break

#         if command:
#             log(f"Received command: {command}")
#             # Execute the command by calling the respective API function
#             try:
#                 eval(f'API.{command}')
#             except AttributeError:
#                 print(f"Unknown command: {command}")

#         client_socket.close()



#     # Close the client socket and server socket
#     client_socket.close()
#     server_socket.close()

# if __name__ == "__main__":
#     main()


# import API
# import sys
# import socket

# def log(string):
#     sys.stderr.write("{}\n".format(string))
#     sys.stderr.flush()

# def handle_client(client_socket):
#     try:
#         while True:
#             command = client_socket.recv(1024).decode()

#             if not command:
#                 break

#             if command == "exit":
#                 break

#             if command == "mF":
#                 log(f"Received command: moveForward")
#                 API.moveForward()
#                 continue

#             log(f"Received command: {command}")
            
#             # Execute the command by calling the respective API function
#             try:
#                 eval(f'API.{command}')
#             except AttributeError:
#                 print(f"Unknown command: {command}")

#     except ConnectionResetError:
#         print("Client closed the connection.")

#     finally:
#         # Close the client socket when done
#         client_socket.close()

# def main():
#     log("Running...")

#     # Initialize a socket server to listen for commands
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind(('127.0.0.1', 12345))  # Use a specific IP and port
#     server_socket.listen(1)  # Listen for one incoming connection
#     log("Listening for commands on 127.0.0.1:12345")

#     while True:
#         # Accept a client connection
#         client_socket, client_address = server_socket.accept()
#         log(f"Accepted connection from {client_address[0]}:{client_address[1]}")

#         # Handle commands from the client in a separate function
#         handle_client(client_socket)

#     # Close the server socket (this part may not be reached)
#     server_socket.close()

# if __name__ == "__main__":
#     main()



# import API
# import sys
# import socket

# def log(string):
#     sys.stderr.write("{}\n".format(string))
#     sys.stderr.flush()

# def handle_server(server_socket):
#     while True:
#         # Receive a message from the server
#         message = server_socket.recv(1024).decode()

#         if not message:
#             break

#         log(f"Received message from server: {message}")

#         # Process the received message as needed
#         # Example: Execute an API function based on the message
#         try:
#             eval(f'API.{message}')
#         except AttributeError:
#             print(f"Unknown command: {message}")

# def main():
#     log("Running...")

#     # Initialize a socket client to connect to the server
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.connect(('127.0.0.1', 12345))  # Connect to the server IP and port

#     # Handle messages received from the server
#     handle_server(server_socket)

#     # Close the server socket
#     server_socket.close()

# if __name__ == "__main__":
#     main()



# Works with the Intermediate.py file. To receive and process commands from the server.
import API
import sys
import socket

def log(string):
    sys.stderr.write("{}\n".format(string))
    sys.stderr.flush()

def handle_server(server_socket):
    while True:
        # Receive a command from the server
        command = server_socket.recv(1024).decode()

        if not command:
            break

        log(f"Received command from server: {command}")

        commands = command.strip().split(">")  # Split the command by ">"

        for command in commands:
            log(f"Cleaned Command: {command}")
            # Process the received command as needed
            if command == "mF":
                # Execute API.moveForward() for "mF" command
                API.moveForward()
            elif command == "tR":
                API.turnRight()
            elif command == "tL":
                API.turnLeft()
            # elif command == "sW":
            #     # "sW-%i-%i-%c\n", x, y, orientationToDirection((orientation - 1) % 4)
            #     # Filter the command string above to extract the x, y, and direction
            #     # Example: "sW-1-2-E" -> "1-2-E" -> ["1", "2", "E"] -> [1, 2, "E"]
            #     x, y, direction = command[3:].split("-")
            #     API.setWall(x, y, direction)
            elif command.startswith("sW-"):
                # Example command: "sW-1-2-E"
                parts = command.strip().split("-")  # Split the command by "-"
                
                # Check if we have enough parts (4 parts in total, we only need the last 3)
                if len(parts) == 4:
                    x, y, direction = parts[1], parts[2], parts[3]
                    # Convert x and y to integers if needed
                    try:
                        x = int(x)
                        y = int(y)
                    except ValueError:
                        # Handle the case where x or y is not a valid integer
                        log("Invalid x or y value in the command.")
                        continue  # Skip processing this command

                    API.setWall(x, y, direction)
            elif command.startswith("sC-"):
                # Example command: "sC-1-2-G"
                parts = command.strip().split("-")  # Split the command by "-"
                
                # Check if we have enough parts (4 parts in total)
                if len(parts) == 4:
                    x, y, character = parts[1], parts[2], parts[3]
                    # Convert x and y to integers if needed
                    try:
                        x = int(x)
                        y = int(y)
                    except ValueError:
                        # Handle the case where x or y is not a valid integer
                        log("Invalid x or y value in the command.")
                        continue  # Skip processing this command

                    API.setColor(x, y, character)
            elif command.startswith("sT-"):
                # Example command: "sT-1-2-1"
                parts = command.strip().split("-")

                # Check if we have enough parts (4 parts in total)
                if len(parts) == 4:
                    x, y, text = parts[1], parts[2], parts[3]
                    # Convert x and y to integers if needed
                    try:
                        x = int(x)
                        y = int(y)
                    except ValueError:
                        # Handle the case where x or y is not a valid integer
                        log("Invalid x or y value in the command.")
                        continue

                    API.setText(x, y, text)
        


def main():
    log("Running...")

    # Initialize a socket client to connect to the server
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.connect(('127.0.0.1', 12345))  # Connect to the server IP and port

    # Handle commands received from the server
    handle_server(server_socket)

    # Close the server socket
    server_socket.close()

if __name__ == "__main__":
    main()





#Test
# import API
# import sys

# def log(string):
#     sys.stderr.write("{}\n".format(string))
#     sys.stderr.flush()

# def main():
#     log("Running...")

#     while True:
#         API.setText(1, 1, "1")

# if __name__ == "__main__":
#     main()