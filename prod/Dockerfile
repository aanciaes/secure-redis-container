##### Compile Attestation Server

FROM sconecuratedimages/crosscompilers as compiler

COPY ./attestation_server /home/attestation_server 

RUN apk update && apk add libstdc++

WORKDIR /home/attestation_server 

RUN g++ server.cpp -o attestation-server -I/home/attestation_server/include -L/home/attestation_server/lib -lchilkat-9.5.0 -static

##### Production Environment

FROM sconecuratedimages/apps:redis-6-alpine

### Attestation Server ###
RUN mkdir -p /home/attestation_server

COPY --from=compiler /home/attestation_server/attestation-server /home/attestation_server/attestation-server

COPY ./attestation_server/attestation-identity-key.p12 /home/attestation_server/attestation-identity-key.p12

### Redis Server ###
# Copy config file
COPY ./redis/redis.conf /usr/local/etc/redis/redis.conf

# Copy TLS files
COPY ./redis/tls/redis-server/thesis-redis-server.key /usr/local/etc/redis/redis.key
COPY ./redis/tls/redis-server/thesis-redis-server.crt /usr/local/etc/redis/redis.crt
COPY ./redis/tls/redis-server/thesis-redis-server.dh /usr/local/etc/redis/redis.dh
COPY ./redis/tls/ca/thesis-ca.crt /usr/local/etc/redis/ca.crt

COPY start.sh /home/start.sh

RUN chmod +x /home/start.sh

CMD ["/home/start.sh"]