##### Compile Attestation Server

FROM sconecuratedimages/crosscompilers as compiler

COPY ./attestation_server /home/attestation_server 

RUN apk update && apk add libstdc++

WORKDIR /home/attestation_server 

RUN g++ server.cpp -o attestation-server -I/home/attestation_server/include -L/home/attestation_server/lib -lchilkat-9.5.0 -static

##### Production Environment

FROM sconecuratedimages/apps:redis-6-alpine

RUN mkdir -p /home/attestation_server

COPY --from=compiler /home/attestation_server/server /home/attestation_server/server

RUN echo 'asdasd' > /home/attestation_server/file.txt

COPY ./attestation_server/attst-server.p12 /home/attestation_server/attst-server.p12

COPY start.sh /home/start.sh

RUN chmod +x /home/start.sh

CMD ["/home/start.sh"]