# k8s-rest

Use kubernetes rest API to test out things.
 

## to list various things like nodes, pods, ...

The example uses jsonpath to fish out the `names` of items.

```
$ go run main.go
[04:48:47 k8s-rest/main.go:65 main.printResult]                                 
0.000s res=NodeList                                                             
0.001s res=[]interface {}{                                                      
           "kube-master",                                                       
           "kube-node-1",
           "kube-node-2",
       }
0.017s res=PodList
0.018s res=[]interface {}{
           "etcd-kube-master",
           "kube-apiserver-kube-master",
           "kube-controller-manager-kube-master",
           "kube-dns-64d6979467-wrq4c",
           "kube-proxy-kt6lm",
           "kube-proxy-stc9s",
           "kube-proxy-tzt2z",
           "kube-scheduler-kube-master",
           "kubernetes-dashboard-866bbc8b55-g7gqx",
       }
0.094s res=NamespaceList
0.094s res=[]interface {}{
           "default",
           "kube-public",
           "kube-system",
       }
0.100s res=ServiceList
0.100s res=[]interface {}{
           "kubernetes",
...
```


As printed via q.Q


## to test namespace adding

Equivalent of doing 

```
$ kubectl create -f namespace.yaml
```

is to do

```
$ go run main.go -namespace namespace.yaml
$ kubectl get namespaces
NAME               STATUS    AGE
default            Active    2d
kube-public        Active    2d
kube-system        Active    2d
test-namespace-1   Active    4s
```


## to create a deployment

Equivalent of doing

```
$ kubectl apply -f deployment.yaml
```

is to do

```
$ go run main.go -deployment deployment.yaml  -namespace test-namespace-1
```

In this case `-namespace` is not a file, but the `namespace` name 
created before.  

To verify:

```
kubectl get pods --namespace test-namespace-1
NAME          READY     STATUS    RESTARTS   AGE
nginx-82n45   1/1       Running   0          4m
```


