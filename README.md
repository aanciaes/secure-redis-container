# Redis Secure Server
(https://hub.docker.com/repository/docker/aanciaes/secure-redis)

This repository contains an implementation of the redis-server secured by Intel's SGX tecnhology with the help of the [SCONE]("https://scontain.com/index.html?lang=en") platform. It also presents an implementation of a **simulated** remote attestation server. That runs alongside the redis in order to simulate the real SGX's attestation procedure.

The dev folder is supposed to be used to develop the attestation server. The prod folder contains all the production files - code, bins, tls keys and certificates, configs, passwords etc it is for production use in the OVHCloud.

## Develpoment

1. Run the cross-compiler with `docker run --rm -it -p 8541:8541 -v ~/Desktop/thesis/secure-redis-container/dev/attestation_server:/home/attestation_server --name compiler sconecuratedimages/crosscompilers`.
2. Run `cd /home/attestation_server` to navigate to the work directory..
3. To compile the C++ code run: `g++ server.cpp -o server -I/home/attestation_server/include -L/home/attestation_server/lib -lchilkat-9.5.0 -static`.
4. To test the compiled code run: `./server`.

Since the folder is mounted as a volume in the compiler conatiner, we can then exit the container and quickly run the compiled code alongside redis:

### Standalone Redis Node

1. Exit the compiler container with `exit`.
2. Build the image with `docker build -t secure-redis:dev .`
3. Run the image: `docker run --rm --name dev-secure-redis -it -p 6379:6379 -p 8541:8541 secure-redis:dev`

### Cluster Mode

1. Build the image and run docker-compose with `docker-compose -f cluster-compose.yml up --build`
2. Build the cluster by running `docker exec -i -t dev-secure-redis-cluster-7000 redis-cli -p 7000 --cluster create 127.0.0.1:7000 127.0.0.1:7001 127.0.0.1:7002 -a 'redis'`


## Production

**Attention:** Modifications done on the development source code **won't** reflect on the production code. Modification must be done manually.

To build the production image and test locally:

1. `docker build -t secure-redis:prod .`
2. To run: `docker run --rm --name secure-redis -it -p 6357:6357 -p 8541:8541 secure-redis:prod`

### Standalone Secure Production Server

#### Push to docker hub

1. Login to docker hub
2. Build image with version as tag:
3. `docker build -t aanciaes/secure-redis:<version> .`
4. `docker push aanciaes/secure-redis:<version>`

#### Running on Production Environment

1. Login to docker
2. `docker run --rm --name secure-redis -it -d -p 6357:6357 -p 8541:8541 --device=/dev/isgx -e SCONE_MODE=HW -e SCONE_FORK=1 aanciaes/secure-redis:<version>`

### Cluster Secure Production Server

#### Push to docker hub

1. Login to docker hub
2. Build image with version as tag:
3. `docker build -t aanciaes/secure-redis:<version>-cluster .`
4. `docker push aanciaes/secure-redis:<version>-cluster`

#### Running on Production Environment

1. Login to docker
2. Download Docker Compose file with: `wget https://raw.githubusercontent.com/aanciaes/secure-redis-container/master/prod/cluster-compose.yml\?token\=ADHR2YZ4W3Y7T2P3ESZKUJS7RGQBI -O cluster-compose.yml`
3. Run with: `docker-compose -f cluster-compose.yml`
4. Build the cluster by running `docker exec -i -t secure-redis-cluster-7000 redis-cli -p 7000 --cluster create 51.210.0.209:7000 51.210.0.209:7001 51.210.0.209:7002 --user anciaes -a '7Mmo8YDRU3+XGM6rAb72deJD432h)4'`

**Notes:**

1. The deployment on the cloud provider should be done by uploading the production image to the docker hub registry and pull from there to avoid any losses.
2. Be aware of the redis ports with the docker run command as they may change in the `redis.conf`
3. (Temporary) Be aware of the hard-coded paths and passwords when merging code from developement to production.

## Unsecure Redis
(https://hub.docker.com/repository/docker/aanciaes/unsecure-redis/general)

Redis container meant to run in unprotected memory (outside SGX).

### Standalone Unsecure Redis Server

#### Develpoment

1. `docker build -t unsecure-redis:dev .`
2. To run: `docker run --rm --name unsecure-redis -it -p 6363:6363 unsecure-redis:dev`

#### Production

1. `docker build -t aanciaes/unsecure-redis:<version> .`
2. `docker push aanciaes/unsecure-redis:<version>`

#### Running on Production Environment

1. Login to docker
2. `docker run --rm --name unsecure-redis -it -d -p 6363:6363 aanciaes/unsecure-redis:<version>`

### Cluster Unsecure Redis Server

#### Production

1. Login to docker hub
2. Build image with version as tag: `docker build -f cluster.Dockerfile -t aanciaes/unsecure-redis:<version>-cluster .`
4. `docker push aanciaes/unsecure-redis:<version>-cluster`

#### Running on Production Environment

1. Login to docker
2. Download Docker Compose file with: `wget https://raw.githubusercontent.com/aanciaes/secure-redis-container/master/prod/cluster-compose.yml\?token\=ADHR2YZ4W3Y7T2P3ESZKUJS7RGQBI -O cluster-compose.yml`
3. Run with: `docker-compose -f cluster-compose.yml`
4. Build the cluster by running `docker exec -i -t secure-redis-cluster-7000 redis-cli -p 7000 --cluster create 51.210.0.209:7000 51.210.0.209:7001 51.210.0.209:7002 --user anciaes -a '7Mmo8YDRU3+XGM6rAb72deJD432h)4'`
