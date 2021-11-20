sudo tc qdisc add dev  enp0s8 root  netem delay 200ms loss 1.3% 25%
sudo tc qdisc ls dev enp0s8

#sudo tc qdisc replace dev enp0s8 root netem loss 0% delay 0ms
#sudo tc qdisc del dev enp0s8 root netem
#sudo tc qdisc ls dev enp0s8
