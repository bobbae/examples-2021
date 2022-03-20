---
title: "azure down"
date: 2019-02-04T11:40:11+02:00
#image: "/images/blog/azure-down.png"
tags: ["azure","database","kubernetes","digitalocean"]
comments: true
---

Databases losing data is bad. Not [once](https://www.jamesserra.com/archive/2018/08/azure-sql-database-disaster-recovery/) but [repeatedly](https://nakedsecurity.sophos.com/2019/02/01/dns-outage-turns-tables-on-azure-database-users/). 

AKS (Azure Kubernetes) is still using old 1.9.X versions which are not secure. The 1.9.X version reached End-of-life as of September, 2018. The AKS is not available in AzureStack either.   Often you can only create Kubernetes cluster in the US-central.  Many resources are created and billed when you create AKS cluster.

Better option may be to use [Digital Ocean](https://m.do.co/c/1c6de959e2b7).  They bill monthly on flat rate.  

It's [easy](https://kubernetes.io/docs/setup/independent/create-cluster-kubeadm/) to create a kubernetes cluster via kubeadm.  

To create a  kubernetes cluster on digital ocean, you can use their kubernetes cluster service or just create some VMs.  [Here](https://5pi.de/2016/11/20/15-producation-grade-kubernetes-cluster/), you can find a good blog article on how to set up a $15 kubernetes cluster on Digital Ocean.  The details are [here](https://kubernetes.io/docs/setup/independent/create-cluster-kubeadm/).  

Further simplified steps:

On first node (master node),

```
$ kubeadm init --pod-network-cidr=192.168.0.0/16
[init] Using Kubernetes version: vX.Y.Z
[preflight] Running pre-flight checks
[preflight] Pulling images required for setting up a Kubernetes cluster
...
```

This takes up to 10 minutes. When finished, the join `TOKEN` is printed, which will be later used to join worker nodes.

After that, you will run a CNI on the same node.  A good choice is Calico.  

```
$ kubectl apply -f https://docs.projectcalico.org/v3.3/getting-started/kubernetes/installation/hosted/rbac-kdd.yaml
$ kubectl apply -f https://docs.projectcalico.org/v3.3/getting-started/kubernetes/installation/hosted/kubernetes-datastore/calico-networking/1.7/calico.yaml
```
On other nodes (worker nodes), run the join command:

```
$ kubeadm join TOKEN
```

where `TOKEN` is the join token printed after running kubeadm init on the master node.

Wait a bit and then on the master node:

```
$ kubectl get nodes
```

