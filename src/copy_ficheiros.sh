#!/bin/bash

DIR=~/Desktop/UC/2ºano/2ºsemestre/RC/ProjetoRC/project-files/docker/
SERVER=5ecfd568-1422-49e1-be40-de82f2ff6318
PC1=314f6ff3-b5c4-47e4-82ad-a629920435aa
PC2=38b8351d-f081-404f-962d-8f43e88a0190
PC3=044adf29-34c3-4532-af54-723cea6c955e
PC4=cf59de45-4c59-4738-9b71-a4028481f361

#compile and move stuff to server
cd server/
gcc -o server server.c dict.c -Wall -pthread
cp server $DIR/$SERVER/home
cp users $DIR/$SERVER/home
echo "Copied files to server"

#compile and move stuff to clients
cd ..
cd client/
gcc -o client client.c dict.c -Wall -pthread
gcc -o admin adminClient.c dict.c -Wall -pthread

cp client $DIR/$PC1/home
cp admin $DIR/$PC1/home

cp client $DIR/$PC2/home
#cp admin $DIR/$PC2/home

cp client $DIR/$PC3/home
#cp admin $DIR/$PC3/home

cp client $DIR/$PC4/home
#cp admin $DIR/$PC4/home

echo "Copied files to all clients"
