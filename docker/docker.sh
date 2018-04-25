echo ========= Build docker image
docker build -t otus.lessons.30.01 .
echo ========= Execute kkmeans
docker run --rm -i otus.lessons.30.01 kkmeans -v
xhost +SI:$USER:root
xauth nlist :0 | sed -e 's/^..../ffff/' | xauth -f /tmp/.docker.xauth nmerge -
docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v /tmp/.docker.xauth:/tmp/.docker.xauth -v /etc/machine-id:/etc/machine-id -ti otus.lessons.30.01 kkmeans
echo ========= Remove docker image
docker rmi otus.lessons.30.01