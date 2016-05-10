#!/bin/bash

# LOG_FORMAT='$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent" x="$http_x_forwarded_proto"'

if [ -n "$LOG_FORMAT" ]; then
   ex /etc/nginx/nginx.conf <<EOF
/access_log/ i
	log_format custom '$LOG_FORMAT';
.
x
EOF
   
fi

exec /usr/sbin/nginx
