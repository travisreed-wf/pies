#!/bin/bash

# LOG_FORMAT='$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent" x="$http_x_forwarded_proto"'
CONF_FILE=/etc/nginx/nginx.conf
INDEX_FILE=/usr/share/nginx/html/index.html

if [ -n "$LOG_FORMAT" ]; then
   ex $CONF_FILE <<EOF
/access_log/ i
	log_format custom '$LOG_FORMAT';
.
/access_log/ s/;/ custom;/
x
EOF
   
fi

if [ -z "$BODY_TEXT" ]; then
   BODY_TEXT="Hello World"
fi

if [ -z "$SERVER_NAME" ]; then
   SERVER_NAME=$(uname -n)
fi

ex $INDEX_FILE <<EOF
/<body>/ a
Server: $SERVER_NAME
$BODY_TEXT
.
x
EOF

exec /usr/sbin/nginx
