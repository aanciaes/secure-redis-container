# Thesis-Attestation-Server

## How to Compile

Run `docker run -it -p 8080:8080 -v ~/Desktop/thesis/attestation_server:/home --name compiler sconecuratedimages/crosscompilers`

Run `docker exec -i -t compiler /bin/sh`

Run `cd /home/attestation_server`

Run `g++ server.cpp -o server -I/home/attestation_server/include -L/home/attestation_server/lib -lchilkat-9.5.0 -static`

Run `exit`

Run `docker build -t attst .`

Run `docker run -i -t -p 8080:8080 attst`
