FROM golang:latest
WORKDIR /app

# Copy go mod and sum files
COPY go.mod go.sum ./

# Download all dependencies. Dependencies will be cached if the go.mod and go.sum files are not changed
RUN go mod download

# Copy the source from the current directory to the Working Directory inside the container
COPY . .
#RUN go get ./...
RUN go build 

EXPOSE 2666/udp

#CMD ["./rudp", "-ql", "all", "-qo", "stdout"]
ENTRYPOINT ["./rudp"]
