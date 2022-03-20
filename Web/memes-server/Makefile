include config.mk

.PHONY: docker-build docker-push apply-secret delete-secret

docker-build:
	docker build --no-cache -t $(IMAGE) .

docker-push:
	docker push  $(IMAGE)

apply-secret:
	kubectl apply -f meme-server-secret.yaml

delete-secret:
	kubectl delete secret meme-server-secret

run-meme-server:
	cat meme-server-pod.yaml | envsubst | kubectl apply -f - 

expose-meme-server:
	kubectl expose pod meme-server --port=3001 --target-port=3001 --type=LoadBalancer

minikube-service:
	minikube service meme-server

delete-meme-server:
	kubectl delete pod meme-server
	kubectl delete svc meme-server

