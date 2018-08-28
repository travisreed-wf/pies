FROM drydock-prod.workiva.net/workiva/smithy-runner-golang:176833 as build

# Build Environment Vars
ARG GIT_SSH_KEY
ARG KNOWN_HOSTS_CONTENT

ENV BUILD_DIR=/go/src/github.com/Workiva/pies

RUN mkdir /root/.ssh && \
    echo "$KNOWN_HOSTS_CONTENT" > "/root/.ssh/known_hosts" && \
    chmod 700 /root/.ssh/ && \
    umask 0077 && echo "$GIT_SSH_KEY" >/root/.ssh/id_rsa && \
    eval "$(ssh-agent -s)" && ssh-add /root/.ssh/id_rsa

RUN git config --global url.git@github.com:.insteadOf https://github.com

ADD . ${BUILD_DIR}
WORKDIR ${BUILD_DIR}

ARG BUILD_ARTIFACTS_AUDIT=/${BUILD_DIR}/glide.lock

RUN glide install
RUN go build -x -o pies-server
RUN cp pies-server /
ARG BUILD_ARTIFACTS_RELEASE=/pies-server

FROM drydock-prod.workiva.net/workiva/smithy-runner-golang:176833

COPY --from=build /pies-server /pies-server

ENTRYPOINT [ "/pies-server" ]
