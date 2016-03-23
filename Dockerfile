FROM lukasheinrich/higgs-mc-studies-base
RUN yum install -y nano
RUN curl -s https://bootstrap.pypa.io/get-pip.py | python
RUN pip install click pyyaml
COPY . /analysis
WORKDIR /analysis
RUN cd pythia_main && make
RUN mv HC_UFO /code/madgraph-2.3.3/models/
