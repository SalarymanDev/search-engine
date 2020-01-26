FROM ubuntu:18.04

# Copy supporting google drive files
ADD gdfuse /gdfuse
WORKDIR /gdfuse

# Install add-apt-repository
RUN apt update \
    && apt install -y software-properties-common

# Install google drive ocaml fuse
RUN add-apt-repository ppa:alessandro-strada/ppa \
    && apt update \
    && apt install -y google-drive-ocamlfuse gdb

# Clean setup
RUN apt clean all \
    && rm /var/log/apt/* /var/log/alternatives.log /var/log/bootstrap.log /var/log/dpkg.log

RUN mkdir /GoogleDrive

# Configure ocamlfuse
RUN echo "user_allow_other" >> /etc/fuse.conf \
    && google-drive-ocamlfuse -serviceaccountpath BBGSearch-29317747b48f.json -serviceaccountuser bbgsearch@bbg-search-254821.iam.gserviceaccount.com /GoogleDrive\
    && cp config ~/.gdfuse/default/config

WORKDIR /
ADD run.sh /run.sh

CMD ["bash", "-c", "run.sh"]
