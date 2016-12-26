FROM lukasheinrich/higgs-mc-studies-base
RUN yum install -y nano
RUN curl -s https://bootstrap.pypa.io/get-pip.py | python
RUN pip install click pyyaml
COPY . /analysis
WORKDIR /analysis
RUN cd pythia_main && make
RUN mv HC_UFO /code/madgraph-2.3.3/models/
RUN mkdir /code/madgraph-2.3.3/models/dim6
RUN cp dim6.tar.gz /code/madgraph-2.3.3/models/dim6 && cd /code/madgraph-2.3.3/models/dim6 && tar -xf dim6.tar.gz