class Server {
	Channel Acceptor;
	EventLoop el;
	std::unoredered_map<std::string, scoped_ptr<Channel *> > clients;


	Server(const std::string &str, int port);
	void onAccept(Channel *ch);
	void onRead(Channel *ch, void *arg);
	void onWrite(Channel *ch, void *arg);
	loop();
}

int main(int argc, char *argv[])
{
	Server server;
	server.loop();
}


