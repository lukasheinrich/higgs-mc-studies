1) First, create a new cluster:

    carina create  bcm2 --wait
    eval $(carina env bcm2)

2) Then, create a shared workdir volume

    docker volume create --name workdirbase
    export PACKTIVITY_WORKDIR_LOCATION="/workdirbase:$(docker volume inspect -f '{{.Mountpoint}}' workdirbase)"

3) Pull Yadage:
    docker pull lukasheinrich/yadage

4) Start yadage container:
    docker run -it -v /var/run/docker.sock:/var/run/docker.sock -e PACKTIVITY_WORKDIR_LOCATION=$PACKTIVITY_WORKDIR_LOCATION -e PACKTIVITY_WITHIN_DOCKER=true -v workdirbase:/workdirbase lukasheinrich/yadage bash

-----

5) Within the container, setup the workflow parameters:

    $> cat << EOF > init.yml
    > par1: 0.3
    > par2: 0.5
    > nevents: 1000
    > seeds: [12,34]
    > EOF

6) make the workdirectory for this set of pars:
     mkdir /workdirbase/work1

7) run the workflow:

    yadage-run -t from-github/higgsmcproduction /workdirbase/work1 rootflow.yml init.yml 2>&1 | tee work1
   
Repeat steps 5-7 as necessary, you can disconnect from the yadage container via Ctrl-P 
