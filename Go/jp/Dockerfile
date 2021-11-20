# A sample to show how to build and run an app inside docker
#
#  docker build -t jp .
#  docker run -it --rm --name jp jp
#
FROM golang:latest AS build

WORKDIR /go/src/jp
COPY . .

RUN go get -d -v ./...
RUN go install -v ./...

FROM golang:latest

COPY --from=build /go/bin/jp /usr/local/bin/
ENTRYPOINT [ "jp", "-server", "-port", "8080" ]
