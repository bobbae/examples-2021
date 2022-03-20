PKG = "kuberp"
GOPATH ?= $(shell go env GOPATH)
GO_PACKAGES := $(shell go list ./... | grep -v /vendor/)
BINARIES = kuberp
API_FILES = api/kuberp.pb.go api/kuberp.pb.gw.go api/kuberp.swagger.json

.PHONY: build api dep test race msan

build: api dep ## Build kuberp
	@go build .

dep: api ## Fetch dependencies
	@go get ./...

api: $(API_FILES) ## Auto-generate gRPC/REST Go sources

api/kuberp.pb.go: api/kuberp.proto
	@protoc -I. \
		-I${GOPATH}/src \
		-I${GOPATH}/src/github.com/grpc-ecosystem/grpc-gateway/third_party/googleapis \
		--go_out=plugins=grpc:. \
		api/kuberp.proto

api/kuberp.pb.gw.go: api/kuberp.proto
	@protoc -I. \
		-I${GOPATH}/src \
		-I${GOPATH}/src/github.com/grpc-ecosystem/grpc-gateway/third_party/googleapis \
		--grpc-gateway_out=logtostderr=true:. \
		api/kuberp.proto

api/kuberp.swagger.json: api/kuberp.proto
	@protoc -I. \
		-I${GOPATH}/src \
		-I${GOPATH}/src/github.com/grpc-ecosystem/grpc-gateway/third_party/googleapis \
		--swagger_out=logtostderr=true:. \
		api/kuberp.proto

clean: ## Clean compiled binaries
	@rm -f ${BINARIES}

realclean: ## Clean compiled binaries and all generated files
	@rm -f ${BINARIES}
	@rm -f ${API_FILES}

test: dep ## Run tests
	@go test -short ${GO_PACKAGES}

race: dep ## Run tests with race detector
	@go test -race -short ${GO_PACKAGES}

msan: dep ## Run tests with memory sanitizer
	@go test -msan -short ${GO_PACKAGES}