#!/bin/sh

SCONE_HASH=1 /home/attestation_server/server > /home/attestation_server/attst_server_mrenclave
SCONE_VERSION=1 /home/attestation_server/server &
SCONE_HASH=1 redis-server > /home/attestation_server/redis_mrenclave
#chmod 444 /home/attestation_server/mrenclave
SCONE_VERSION=1 redis-server