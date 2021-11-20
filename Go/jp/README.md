# jp

jp is a program that can be used to grok json at the command line via path expressions.
Here it is used as a demo for build process.

Dockerfile and Makefile are used to illustrate docker build and deployment examples.

Usage:

As a pretty printer. 
```
cat file.json | jp
```

You can use print to print a specific node in the JSON.  The docker inspect command
outputs JSON which can be piped to jp which then can select a particular path and node
in the JSON.  The - before print indicates that JSON input is stdin. 


```
$ docker inspect a6a2 | jp - print /[0]/Config/Env
["PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"]
```

You can save the JSON output and parse conditionally.  The following prints memory capacity of the system.

```
$ lshw -json > /tmp/x
$ jp /tmp/x if  'children[0]/children[0]/id == memory' print /children[0]/children[0]/size
3.3663483904e+10
```

You can use foreach to print elements of an Array:
```
$ jp /tmp/x foreach  'children[0]/children' print |wc -l
3
```

The following lists out array at children[0]/children[]. Then each line is piped again to another jp to check for CPU 0 capabilities and capacity.
```
$ jp /tmp/x foreach  'children[0]/children' print | \
while read line; do echo $line | \
    jp - if 'businfo == cpu@0' list /capabilities/vmx /capacity  ;done 
true 3.400132e+09 
```

You can run it in server mode which will serve at a given port.
```
jp -port 8080 -server
```

Connect web browser to http://localhost:8080. 

## jy

The directory jy contains a program that can convert YAML to JSON and back.

