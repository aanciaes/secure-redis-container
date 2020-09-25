# Redis Secure Server

This repository contains an implementation of the redis-server secured by Intel's SGX tecnhology with the help of the [SCONE]("https://scontain.com/index.html?lang=en") platform. It also presents an implementation of a **simulated** remote attestation server. That runs alongside the redis in order to simulate the real SGX's attestation procedure.

The dev folder is supposed to be used to develop the attestation server. The prod folder contains all the production files - code, bins, tls keys and certificates, configs, passwords etc it is for production use in the OVHCloud.

## Develpoment

1. Run the cross-compiler with `docker run --rm -it -p 8080:8080 -v ~/Desktop/thesis/secure-redis/dev/attestation_server:/home/attestation_server --name compiler sconecuratedimages/crosscompilers`.
2. Run `cd /home/attestation_server` to navigate to the work directory..
3. To compile the C++ code run: `g++ server.cpp -o server -I/home/attestation_server/include -L/home/attestation_server/lib -lchilkat-9.5.0 -static`.
4. To test the compiled code run: `./server`.

Since the folder is mounted as a volume in the compiler conatiner, we can then exit the container and quickly run the compiled code alongside redis:

1. Exit the compiler container with `exit`.
2. Build the image with `docker build -t secure-redis:dev .`
3. Run the image: `docker run --rm --name dev_secure_redis -it -p 6379:6379 -p 8080:8080 secure-redis:dev`


## Production

**Attention:** Modifications done on the development source code **won't** reflect on the production code. Modification must be done manually.

To build the production image:

1. `docker build -t secure-redis:prod .`
2. To run: `docker run --rm --name secure_redis -it -p 6357:6357 -p 8080:8080 secure-redis:prod`

### Push to docker hub

1. Login to docker hub
2. Build image with version as tag:
3. `docker build -t aanciaes/secure-redis:0.1.1 .`
4. `docker push aanciaes/secure-redis:0.1.1`

If image is ready for production, build the prod tag and push:

1. `docker build -t aanciaes/secure-redis:prod .`
2. `docker push aanciaes/secure-redis:prod`

### Running on Production Environment

1. Login to docker
2. docker run --rm --name secure_redis -it -d -p 6357:6357 -p 8080:8080 --device=/dev/isgx -e SCONE_MODE=HW aanciaes/secure-redis:prod

**Notes:**

1. The deployment on the cloud provider should be done by uploading the production image to the docker hub registry and pull from there to avoid any losses.
2. Be aware of the redis ports with the docker run command as they may change in the `redis.conf`
3. (Temporary) Be aware of the hard-coded paths and passwords when merging code from developement to production.
