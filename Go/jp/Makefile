NAME ?= golang
TAG ?= latest
CONTAINER = $(NAME):$(TAG)

.PHONY: gobuild
gobuild:
	docker run --rm -v `pwd`:/go/src/myapp -w /go/src/myapp $(CONTAINER)  sh -c 'go get ./... && go build -v ./...' 

.PHONY: testjp
testjp: 
	docker inspect golang > test.out
	docker run --rm -v `pwd`:/go/src/myapp -w /go/src/myapp $(CONTAINER)  sh -c 'go get ./... && go install ./... && cat test.out | jp - print /[0]/ContainerConfig/Env' 

.PHONY: dockerbuild
dockerbuild:
	docker build -t jp .

.PHONY: runjp
runjp:
	docker run -it --rm --name jp jp

goclean:
	go clean ./...
