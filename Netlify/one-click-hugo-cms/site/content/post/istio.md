---
title: "istio service mesh monolith"
date: 2020-01-19T10:30:11+02:00
#image: "/images/blog/azure-down.png"
tags: ["istio","kubernetes","microservices","monolith","service","mesh", "linux"]
comments: true
---
Istio is a service mesh which exists to solve problems in handling large numbers of sprawling
microservices.  
Istio started out as a collection of microservices itself but recently
the complexity of many microservices seems to have regressed it back to a monolith.

This [article](https://www.solo.io/blog/istio-as-an-example-of-when-not-to-do-microservices/) 
shows that microservices are not always the best solution for the problem.

Indeed, we all know by now, there is [no silver bullet](https://en.wikipedia.org/wiki/No_Silver_Bullet). 

The istiod as of 1.5 release is a monolith. 
