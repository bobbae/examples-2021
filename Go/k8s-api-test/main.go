package main

import (
	"flag"
	"fmt"
	"path/filepath"

	"github.com/bobbae/q"
	appsv1 "k8s.io/api/apps/v1"
	apiv1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
	"k8s.io/client-go/util/homedir"
	"k8s.io/client-go/util/retry"
)

func main() {
	var kubeconfig *string
	if home := homedir.HomeDir(); home != "" {
		kubeconfig = flag.String("kubeconfig", filepath.Join(home, ".kube", "config"), "(optional) absolute path to the kubeconfig file")
	} else {
		kubeconfig = flag.String("kubeconfig", "", "absolute path to the kubeconfig file")
	}
	flag.Parse()

	config, err := clientcmd.BuildConfigFromFlags("", *kubeconfig)
	if err != nil {
		panic(err)
	}
	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		panic(err)
	}

	deploymentsClient := clientset.AppsV1().Deployments(apiv1.NamespaceDefault)

	deployment := &appsv1.Deployment{
		ObjectMeta: metav1.ObjectMeta{
			Name: "hello-test",
		},
		Spec: appsv1.DeploymentSpec{
			Replicas: int32Ptr(2),
			Selector: &metav1.LabelSelector{
				MatchLabels: map[string]string{
					"app": "hello-test",
				},
			},
			Template: apiv1.PodTemplateSpec{
				ObjectMeta: metav1.ObjectMeta{
					Labels: map[string]string{
						"app": "hello-test",
					},
				},
				Spec: apiv1.PodSpec{
					Containers: []apiv1.Container{
						{
							Name:  "hello-node",
							Image: "hello-node:v1",
							Ports: []apiv1.ContainerPort{
								{
									Name:          "http",
									Protocol:      apiv1.ProtocolTCP,
									ContainerPort: 8080,
								},
							},
						},
					},
				},
			},
		},
	}

	q.Q("Creating deployment...")
	result, err := deploymentsClient.Create(deployment)
	if err != nil {
		panic(err)
	}
	q.Q(result.GetObjectMeta().GetName())

	q.Q("Updating deployment...")

	retryErr := retry.RetryOnConflict(retry.DefaultRetry, func() error {
		result, getErr := deploymentsClient.Get("hello-test", metav1.GetOptions{})
		if getErr != nil {
			panic(fmt.Errorf("Failed to get latest version of Deployment: %v", getErr))
		}

		result.Spec.Replicas = int32Ptr(1)                              // reduce replica count
		result.Spec.Template.Spec.Containers[0].Image = "hello-node:v2" // change nginx version
		_, updateErr := deploymentsClient.Update(result)
		return updateErr
	})
	if retryErr != nil {
		panic(fmt.Errorf("Update failed: %v", retryErr))
	}
	q.Q("Updated deployment...")

	q.Q(apiv1.NamespaceDefault)
	list, err := deploymentsClient.List(metav1.ListOptions{})
	if err != nil {
		panic(err)
	}
	for _, d := range list.Items {
		q.Q(" * %s (%d replicas)\n", d.Name, *d.Spec.Replicas)
	}
	q.Q("Create service")

	svc, err := clientset.CoreV1().Services(apiv1.NamespaceDefault).Create(
		&apiv1.Service{
			ObjectMeta: metav1.ObjectMeta{
				Name: "hello-test",
			},
			Spec: apiv1.ServiceSpec{
				Type: apiv1.ServiceTypeNodePort,
				Selector: map[string]string{
					"app": "hello-test",
				},
				Ports: []apiv1.ServicePort{
					{
						Port: 8888,
					},
				},
			},
		})

	if err != nil {
		panic(err)
	}
	q.Q("created ", svc)

	q.Q("List services")
	services, err := clientset.CoreV1().Services(apiv1.NamespaceDefault).List(metav1.ListOptions{})
	if err != nil {
		panic(err)
	}
	for _, svc := range services.Items {
		q.Q(svc)
		if svc.Name == "hello-test" {
			err = clientset.CoreV1().Services(apiv1.NamespaceDefault).Delete("hello-test", &metav1.DeleteOptions{})
			if err != nil {
				panic(err)
			}
			q.Q("Deleted hello-test service")
		}
	}

	q.Q("Deleting deployment...")
	deletePolicy := metav1.DeletePropagationForeground
	if err := deploymentsClient.Delete("hello-test", &metav1.DeleteOptions{
		PropagationPolicy: &deletePolicy,
	}); err != nil {
		panic(err)
	}
	q.Q("Deleted deployment.")
}

func int32Ptr(i int32) *int32 { return &i }
