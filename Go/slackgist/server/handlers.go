package server

import (
	"fmt"

	"github.com/bobbae/slackgist"
	"github.com/bobbae/slackgist/api"
	"golang.org/x/net/context"
)

type Server struct{}

func (srv *Server) Echo(ctx context.Context, req *api.EchoRequest) (res *api.EchoResponse, err error) {
	res = &api.EchoResponse{
		Message: req.Message,
	}

	return res, nil
}

func (srv *Server) Build(ctx context.Context, req *api.BuildRequest) (res *api.BuildResponse, err error) {
	res = &api.BuildResponse{
		Message: req.Message,
	}

	return res, nil
}

func (srv *Server) Status(ctx context.Context, req *api.StatusRequest) (res *api.StatusResponse, err error) {
	res = &api.StatusResponse{
		Message: req.Message,
		Status:  fmt.Sprintf("last hash %s num builds %d", slackgist.GSC.LastHash, slackgist.GSC.NumBuilds),
	}

	return res, nil
}
