---
title: "traffic shaping"
date: 2020-01-09T10:20:11+02:00
#image: "/images/blog/azure-down.png"
tags: ["traffic","shaping","network","tc", "linux"]
comments: true
---

Linux has a built-in traffic control utility called `tc` which uses [kernel features](https://lartc.org/) to
shape traffic. This is useful to test the network applications against unreliable lossy networks
with excessive varying delays. It is also useful for testing applications that have to
work in a wide area network or wireless networks.

The syntax and featureset of `tc` have changed over the years and the latest implementation
has far more features than the basic features that are immediately useful.

```
sudo tc qdisc add dev eth0 root netem delay 200ms loss 1.3% 25%
```

This adds 200msec latency and 1.3% loss with 25% variation. Once added, it seems the qdisc
cannot be deleted but can be replaced. To turn off the above `tc`,

```
sudo tc qdisc replace dev eth0 root netem delay 0ms loss 0%
```

I think the deletion bug is fairly recent but still remains in the kernel for some time.

To list the `qdisc`,

```
sudo tc qdisc ls dev eth0
```



