socat PTY,link=/dev/ttyS10 PTY,link=/dev/ttyS11 &
socat tcp-l:7777,reuseaddr,fork file:/dev/ttyS11,nonblock,raw,echo=0,waitlock=/srv/tty.lock &

sleep 2

/srv/simulator/build/simulator -p /dev/ttyS11