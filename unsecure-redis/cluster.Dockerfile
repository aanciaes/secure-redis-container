FROM redis:6.0-rc1

# Copy config file
COPY ./redis-cluster/redis.conf /usr/local/etc/redis/redis.conf

# Copy TLS files
COPY thesis-prod-ssl/redis-server/thesis-redis-server.key /usr/local/etc/redis/redis.key
COPY thesis-prod-ssl/redis-server/thesis-redis-server.crt /usr/local/etc/redis/redis.crt
COPY thesis-prod-ssl/redis-server/thesis-redis-server.dh /usr/local/etc/redis/redis.dh
COPY thesis-prod-ssl/ca/thesis-ca.crt /usr/local/etc/redis/ca.crt

COPY ./start-cluster.sh /home/start-cluster.sh

RUN chmod +x /home/start-cluster.sh

CMD ["/home/start-cluster.sh"]