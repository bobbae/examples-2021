# https://grpc.io/docs/quickstart/go/
export CGO_ENABLED=0
export GO111MODULE=on # Enable module-aware mode
go get google.golang.org/grpc@v1.28.1
PB_REL="https://github.com/protocolbuffers/protobuf/releases"
curl -LO $PB_REL/download/v3.11.4/protoc-3.11.4-linux-x86_64.zip
unzip protoc-3.11.4-linux-x86_64.zip -d $HOME/.local
export PATH="$PATH:$HOME/.local/bin"
#go mod init github.com/bobbae/slackgist
#make
