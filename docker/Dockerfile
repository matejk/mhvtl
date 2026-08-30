FROM almalinux:8

# installing dependencies
RUN dnf -y install \
    policycoreutils \
    policycoreutils-python-utils \
    make \
    lsscsi \
    git \
    gcc \
    vim \
    zlib-devel \
    kmod \
    kmod-libs

# cloning repo
# RUN git clone https://github.com/markh794/mhvtl.git /mhvtl
COPY . /mhvtl

# installing /usr/bin
WORKDIR /mhvtl/usr
RUN make && make install

# same for /etc (allows vtltape@/vtllibrary@ to work)
# semanage needs no masking: the Makefile only calls it on a live SELinux system
WORKDIR /mhvtl/etc
RUN make install

# Install systemd service definitions from the source tree, not from the
# container's own /etc, which never held them.
WORKDIR /mhvtl
RUN find /mhvtl/etc -maxdepth 1 \
        \( -name '*.target' -o -name '*.service' \) \
        -exec cp {} /usr/lib/systemd/system/ \;

RUN mkdir -p /etc/mhvtl /opt/mhvtl

# to test
RUN mkdir -p /etc/mhvtl/test /opt/mhvtl/test
COPY ./docker/mhvtl.conf  /etc/mhvtl/test/mhvtl.conf
