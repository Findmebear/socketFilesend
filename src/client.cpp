#include <pack109.hpp>

    //./build/run/client --hostname localhost:8081 --request document.txt
    //./build/run/client --hostname localhost:8081 --send files/document.txt
int main(int argc, char* argv[]) {
    if (argc != 5) {
      fprintf(stderr,"The incorrect format of command for an executable %s", argv[0]);
      exit(0);
    }
    /* Parse the input argument */
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char* thirdArgument = strtok(argv[2], ":"); 
    server = gethostbyname(thirdArgument);
    
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    /* --store the path of the file to read, as well as the host address and port number */
    thirdArgument = strtok(NULL, ":");
    portno = atoi(thirdArgument);
    // serv_addr.sin_port = htons(portno);
    // serv_addr.sin_family = AF_INET;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    char *action = argv[3]; //--request or --send
    string filePath;   
    string fileName;
    if(strcmp(argv[3], "--send") == 0) {
        filePath = argv[4];
        fileName = filePath.substr(6);
        File file;
        file.name = fileName; 
        file.bytes = pack109::readBytesFromFile(filePath); //calling the method to read bytes from file
        vec serialized_file_bytes = pack109::serialize(file); //serialize the File struct into a vector of bytes
        pack109::encryptVec(serialized_file_bytes); //encrypting vector bytes with the KEY defined in lib.cpp
        n = send(sockfd, serialized_file_bytes.data(), serialized_file_bytes.size(), 0); //sending the ecnrypted serialized vector to socket
        std::cout << "The file has been located and sent to the file server over the socket\n";
    }
    else if (strcmp(argv[3], "--request") == 0) {
        Request requestFile;    
        fileName = argv[4];     
        requestFile.name = fileName;
        filePath = "files/";
        filePath += argv[4];
        vec serialized_requestFile = pack109::serialize(requestFile);
        pack109::encryptVec(serialized_requestFile);
        n = send(sockfd, serialized_requestFile.data(), serialized_requestFile.size(), 0);
        File receivedFile = pack109::requestFile(sockfd);
        string receivedFilePath = "received/" + receivedFile.name;
        std::ofstream wf(receivedFilePath.c_str(), std::ios::out | std::ios::binary);
        if(!wf) {
            perror("ERROR writing in file");
            exit(1);
        }
        int p;
        if(!wf.write((char *)&receivedFile.bytes[0], receivedFile.bytes.size())) {
            perror("ERROR writing in file");
            exit(1);
        }
        std::cout << "The file has been received and saved to the Received folder\n";
    }
    else {
        fprintf(stderr,"ERROR, use commands --request or --send\n");
        exit(0);
    }
} 