FROM redis:6.0.8-alpine

# Copy config file
COPY ./redis/redis.conf /usr/local/etc/redis/redis.conf

# Copy TLS files
COPY thesis-prod-ssl/redis-server/thesis-redis-server.key /usr/local/etc/redis/redis.key
COPY thesis-prod-ssl/redis-server/thesis-redis-server.crt /usr/local/etc/redis/redis.crt
COPY thesis-prod-ssl/redis-server/thesis-redis-server.dh /usr/local/etc/redis/redis.dh
COPY thesis-prod-ssl/ca/thesis-ca.crt /usr/local/etc/redis/ca.crt

CMD ["redis-server", "/usr/local/etc/redis/redis.conf"]

# redis-cli --tls --cacert /usr/local/etc/redis/ca.crt --cert /usr/local/etc/redis/redis-cli.crt --key /usr/local/etc/redis/redis-cli.key
