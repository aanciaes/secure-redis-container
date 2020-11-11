FROM sconecuratedimages/apps:redis-6-alpine-scone4.2.1

COPY attestation_server /home/attestation_server
COPY start.sh /home/start.sh

RUN chmod +x /home/start.sh

RUN apk update && apk add libstdc++

CMD ["/home/start.sh"]