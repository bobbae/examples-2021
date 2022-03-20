default:

service:
	protoc -I/usr/local/include -I. \
		-I$$GOPATH/src \
		-I$$GOPATH/src/github.com/grpc-ecosystem/grpc-gateway/third_party/googleapis \
		--go_out=google/api/annotations.proto=github.com/grpc-ecosystem/grpc-gateway/third_party/googleapis/google/api,plugins=grpc:. \
		pb/service.proto

proxy:
	protoc -I/usr/local/include -I. \
		-I$$GOPATH/src \
		-I$$GOPATH/src/github.com/grpc-ecosystem/grpc-gateway/third_party/googleapis \
		--grpc-gateway_out=logtostderr=true:. \
		pb/service.proto

getgrpc:
	go get google.golang.org/grpc
	go get -u github.com/golang/protobuf/{proto,protoc-gen-go}
	go get -u github.com/grpc-ecosystem/grpc-gateway/protoc-gen-grpc-gateway

getprotoc:
	wget https://github.com/google/protobuf/releases/download/v3.5.1/protoc-3.5.1-linux-x86_64.zip
	wget https://github.com/google/protobuf/releases/download/v3.5.1/protobuf-all-3.5.1.tar.gz

runserver:
	go run server/server.go -alsologtostderr

runproxy:
	go run server/server-rproxy.go -alsologtostderr

runclient:
	go run client/client.go

proxyclient:
	curl -X POST "http://localhost:8080/v1/example/echo/1/hello123"
