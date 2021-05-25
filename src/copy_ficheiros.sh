#!/bin/bash

DIR=~/GNS3/projects/ProjetoRC/project-files/docker
SERVER=2281951d-472b-4db7-b28c-a98393e94a98
PC1=aae0385f-7ced-4f7f-b6c2-0416a6468907
PC2=cfe9bfed-8835-44f0-921e-4f7ea43b3608
PC3=489e4844-7daa-4e69-b53d-88946fd058ab
PC4=a03d84f9-3ed1-41df-b959-78a6df1e7e01

#compile and move stuff to server
cd server/
gcc -o server server.c server.h dict.c dict.h -Wall -pthread
cp server $DIR/$SERVER/home
echo "Copied files to server"

#compile and move stuff to clients
cd ..
cd client/
gcc -o client client.c client.h dict.c dict.h -Wall -pthread
gcc -o admin adminClient.c -Wall -pthread

cp client $DIR/$PC1/home
#cp admin $DIR/$PC1/home

cp client $DIR/$PC2/home
cp admin $DIR/$PC2/home

cp client $DIR/$PC3/home
#cp admin $DIR/$PC3/home

cp client $DIR/$PC4/home
#cp admin $DIR/$PC4/home

echo "Copied files to all clients"
