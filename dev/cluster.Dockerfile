FROM sconecuratedimages/apps:redis-6-alpine

### Redis Server ###
# Copy config file
COPY ./redis/redis.conf /usr/local/etc/redis/redis.conf

# Attestation Server
COPY attestation_server /home/attestation_server

# Docker start script
COPY ./start-cluster.sh /home/start-cluster.sh

RUN chmod +x /home/start-cluster.sh

CMD ["/home/start-cluster.sh"]