FROM base/archlinux AS build

RUN pacman -Syy
RUN pacman -S --noconfirm base-devel 
RUN pacman -S --noconfirm cmake
RUN pacman -S --noconfirm ogre
RUN pacman -S --noconfirm boost

COPY ./src /root/src


WORKDIR /root/src
RUN cmake .
RUN ls /root/
