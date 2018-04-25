FROM ubuntu:trusty
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 379CE192D401AB61
RUN echo "deb http://dl.bintray.com/madmazoku/otus.lessons trusty main" | sudo tee -a /etc/apt/sources.list
RUN apt update
RUN apt-get install -qqy x11-apps
RUN apt install -y xinit
RUN apt install -y libsdl2-2.0
RUN apt install -y kkmeans
