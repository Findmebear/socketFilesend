#include <pack109.hpp>
#include <hashmap.h>
#include <unistd.h>

// sample run ./build/run/fileserver
int main(int argc, char* argv[]) {
    if (argc != 3) {
      fprintf(stderr,"The incorrect format of command for an executable %s\n", argv[0]);
      exit(0);
    }
    int sockfd, newsockfd;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    HashMap fileHash; //declaring the HashMap to store the received files 
    char* thirdArgument = strtok(argv[2], ":");
    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR creating a socket");
        exit(1);
    }
 
    // Fill in the address structure containing self address
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr(thirdArgument); // declaring IP
    std::cout << thirdArgument << std::endl; 
    thirdArgument = strtok(NULL, ":");
    std::cout << thirdArgument << std::endl;  
    myaddr.sin_port = htons(atoi(thirdArgument));        // Port to listen
    /* Listening to socket, once the connection is received - forking and handling the client
    declaring the vector with maximum size
    declaring the string for the request name
    declaring the int size for the vector size
    pid_t p = fork();
    if(p == 0) { // Child
        Handles the client's input, 
        Decrypts the vector bytes
        If buffer[3] is not 4 - the vector is request, decerialize and send the request.name through shared memory
        Else Sends the vector bytes and the size of the vector to the Parent
        reaping the child
    }
    else { //Parent
        
        if the shared memory is name of the request 
            search through the HashMap and send the File bytes encrypted and decerialized to the socket
        else 
            receives the shared memory vector bytes 
            receives the shared memory size of the vector 
            resized the vector bytes, decerializes to the File struct
            add the File to the HashMap
            continue the while loop
    }
    */
    // Bind a socket to the address
    if (bind(sockfd, (struct sockaddr*) &myaddr, sizeof(myaddr)) < 0) {
        perror("ERROR binding a socket to the address");
        exit(1);
    }
    /* Now start listening for the clients, here process will
    * go in sleep mode and will wait for the incoming connection
    */
    while(1) {
        if (listen(sockfd,SOMAXCONN) != 0 ) {
            perror("listen failed");
            continue;
        }
        else {
            std::cout << "Listening to client to connect" << std::endl;
        }
        clilen = sizeof(cli_addr);
    
        /* Accept actual connection from the client */
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            continue;
        }
        std::cout << "New client connection has been established\n";
        vec buffer(300000);
        int n = recv(newsockfd, buffer.data(), buffer.size(), 0);
        if(n < 1) {
            std::cout << "Error reading the message. Most likely nothing poped up on socket. \n";
            continue;
        }
        buffer.resize(n);
        pack109::decryptVec(buffer);
        if(buffer[3] == 4) { //when the number of name characters is 4 - it is send, otherwise - request
            File fileReceived = pack109::deserialize_file(buffer);

            if(fileHash.insert(fileReceived.name, fileReceived)) { //if true - the File already existed
                std::cout << "The filename is already here, the file has been overwritten\n";
            }
            else { 
                std::cout << "The new file " << fileReceived.name << " has been added \n";
            }
        }
        else {
            Request fileRequested = pack109::deserialize_request(buffer);
            File requestedFile = fileHash.get(fileRequested.name);
            if(requestedFile.name != "nothing") {
                vec serialized_reqFile = pack109::serialize(requestedFile);
                pack109::encryptVec(serialized_reqFile);
                std::cout << "Sending the encrypted bytes of File\n";
                n = send(newsockfd, serialized_reqFile.data(), serialized_reqFile.size(), 0);
            }
        }
        std::cout << "Terminating the connection... \n\n";
        close(newsockfd);
    }
    return 0;
}