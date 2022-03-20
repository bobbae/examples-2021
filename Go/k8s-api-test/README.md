# test  Kubernetes client-go API

Assumes you have access to kubernetes.

Easy way to test is to use minikube. Or if you prefer you can use
three node cluster that can be hosted via Docker-in-Docker method.

```
$ go get github.com/Mirantis/kubeadm-dind-cluster
$ cd $GOPATH/src/github.com/Mirantis/kubeadm-dind-cluster
$ cd fixed
$ ./dind-cluster-v1.10.sh up
```

You should check your kubeconfig context especially if you 
have multiple kubernetes cluster setup

```
$ kubectl config view
$ kubectl config current-context
```
The directory hello-node/ contains simple node.js app. It is used to 
deploy pod and update it.  Two different versions of the apps exist.

