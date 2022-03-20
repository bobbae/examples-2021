# Modified

Modified original front end app from 
> https://chenmu10.github.io/memeGenerator/

Modifed but not necessarily better. Just more complicated to accommodate the list of features requested by others.

* Has both frontend client web app in pure HTML5 and CSS and Javascript and nodejs server side express server. Backend nodejs
code is in `app.js` and `routes/`. Front end is `index.html` and `js`, `css` and `imgs`. Just do `npm i` and `npm start`. Without backend,
the frontend runs fine as static HTML web page, but cannot do Database
and Minio API calls to the backend.
* App is organized to have frontend and backend in a single `npm start`.
* Due to various issues, Database APIs are better handled in the server side. Bugs were enountered in strictly frontend API to DB and could not be fixed. Some  version of APIs are only available in server side, for example, GCS, BigQuery, etc. Frontend calls backend API and backend express nodejs server has CORS enabled.
* Save/Load from Cassandra / AstraDB via astra.datastax.com. Needs testing with k8s hosted cassandra cluster. There are several limits with Cassandra API driver. The per column / row payload is limited and cannot hold data bigger than 1MB reasonably. The other problem is due to the way ORDER BY is only allowed per key partition, making it useless for the purpose of this app. We use PER PARTITION LIMIT as a workaround but better database should be used. Cassandra is used to hold metadata for images annotated with meme words. Also, storing 100s of KB or MB of image data serialized in base64 as a column value in a row is a bad way of using Cassandra and it refuses to do well in that use case.
* Therefore Minio as installed into k8s cluster and used to hold images in object store. Minio does replication and erasure code based recovery in a cluster and better suited for image store.
* Encrypted assets hold credentials for Astra DB, GCP and other API keys. The k8s `secrets` feature is leveraged but it is not really secure being only base64 based. We encrypt all sensitive data which is then unencrypted and placed into docker images. Potentially, it is harmful if docker repo is public but using private gcr.io repo with proper credentials `imagePullSecrets` for private registry should be secure enough.

## Encrypted assets

* you should get SOME-PASSWORD from Bob
* created via

```
$ tar czvf assets.tar.gz key.json meme-server-secret.yaml secure-connect-astratest1.zip
$ echo SOME-PASSWORD | gpg --batch --yes --passphrase-fd 0 -c -o encrypted-assets assets.tar.gz
```

* Before running this software, you need decrypt and untar the assets
```
$ echo SOME-PASSWORD | gpg --batch --yes --passphrase-fd 0 -d -o assets.tar.gz encrypted-assets 
$ tar xvf assets.tar.gz
meme-server-secret.yaml
secure-connect-astratest1.zip
key.json
```

These untarred files are used as follows:

* meme-server-secret.yaml -- to seed secrets into kubernets cluster. See Makefile.
* key.json -- access GCP
* secure-connect-astratest1.zip -- access astra.datastax.com database

They should not be checked into git.  The .gitignore entries exist for them.

The example `minio` install in k8s will use values specified in minio-test.js for
the demo minio cluster. See my-minio-fs.yaml.

For example,

```
$ kubectl apply -f my-minio-fs.yaml
$ kubectl expose deployment/my-minio-fs --type="NodePort" --port 9000
$ kubectl get services/my-minio-fs -o go-template='{{(index .spec.ports 0).nodePort}}'
# wget https://dl.min.io/client/mc/release/linux-amd64/mc
$ mc alias set minio http://192.168.49.2:9000 minio minio123
$ mc admin --json info minio
```

# The following is the original README from frontend memeGenerator

# About
> https://chenmu10.github.io/memeGenerator/

Pick a picture from the list and add text to create a meme.
Responsive with CSS Grid

## Built With

* [Javascript](http://) - Vanilla JS
* [CSS Grid](https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Grid_Layout) - The CSS Grid Layout Module offers a grid-based layout system, with rows and columns, making it easier to design web pages without having to use floats and positioning.
* [HTML5 Canvas](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/canvas) - HTML <canvas> element can be used to draw graphics via scripting in JavaScript. 


## Authors

Chen Mualem 
[https://github.com/chenmu10/](https://github.com/chenmu10/)
