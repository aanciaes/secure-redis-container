##### Compile Attestation Server

FROM sconecuratedimages/crosscompilers as compiler

COPY ./attestation_server /home/attestation_server 

RUN apk update && apk add libstdc++

WORKDIR /home/attestation_server 

RUN g++ server.cpp -o attestation-server -I/home/attestation_server/include -L/home/attestation_server/lib -lchilkat-9.5.0 -static

##### Production Environment

FROM sconecuratedimages/apps:redis-6-alpine-scone4.2.1

### Attestation Server ###
RUN mkdir -p /home/attestation_server

COPY --from=compiler /home/attestation_server/attestation-server /home/attestation_server/attestation-server

COPY ./attestation_server/attestation-identity-key.p12 /home/attestation_server/attestation-identity-key.p12

### Redis Server ###
# Copy config file
COPY ./redis/redis-cluster.conf /usr/local/etc/redis/redis.conf

# Copy TLS files
# Copy TLS files
# TLS files will be fecthed from CAS
# COPY ./redis/tls/redis-server/thesis-redis-server.key /usr/local/etc/redis/redis.key
# COPY ./redis/tls/redis-server/thesis-redis-server.crt /usr/local/etc/redis/redis.crt
COPY ./redis/tls/redis-server/thesis-redis-server.dh /usr/local/etc/redis/redis.dh
COPY ./redis/tls/ca/thesis-ca.crt /usr/local/etc/redis/ca.crt

# CAS and LAS environment variables
ENV SCONE_CAS_ADDR=4-2-1.scone-cas.cf
ENV SCONE_LAS_ADDR=51.210.0.209

COPY start-cluster.sh /home/start-cluster.sh

RUN chmod +x /home/start-cluster.sh

CMD ["/home/start-cluster.sh"]