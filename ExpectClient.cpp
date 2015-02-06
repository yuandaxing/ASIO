class Client {
	EventLoop el;
	Channle  ch;
	connect(const std::string &se, int port);
	onRead(channle &ch, void *arg);
	onWrite(channle &ch, void *arg);
}

int main(int argc, char *argv[])
{
	Client c;
	c.connect();
	client.loop();
}
