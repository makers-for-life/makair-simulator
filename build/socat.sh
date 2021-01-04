socat -d -d pty,link=dev/tty0,raw,echo=0 pty,link=dev/tty1,raw,echo=0 & pid="$!"
wait pid
./simulator -p dev/tty0 &
./makair-control -p dev/tty1
