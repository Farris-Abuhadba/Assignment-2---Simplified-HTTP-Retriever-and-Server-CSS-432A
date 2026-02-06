
#start webserver in the background and pause for a moment while it loads
./webserver &
sleep 2

./retriever www.neverssl.com/

#create all your other test cases here


# shut down the webserver so it isn't running when we do this again.
killall webserver