#!/bin/bash

# LOG_FORMAT='$remote_addr - $remote_user [$time_local] "$request" $status $body_bytes_sent "$http_referer" "$http_user_agent" x="$http_x_forwarded_proto"'
CONF_FILE=/etc/nginx/nginx.conf
INDEX_FILE=/usr/share/nginx/html/index.html
SERVER_FILE=/etc/nginx/sites-available/server

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

if [ -z "$HARBOUR_SERVICE_PREFIX" ]; then
   export HARBOUR_SERVICE_PREFIX=/service/foo/
fi

sed -i -e "s|HARBOUR_SERVICE_PREFIX|${HARBOUR_SERVICE_PREFIX}|" $SERVER_FILE

exec /usr/sbin/nginx
