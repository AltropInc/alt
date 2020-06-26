#include "SocketStream.h"               // for SocketStream, this class

namespace alt {

SocketStream::SocketStream(const SocketAddress& addr)
    : Socket (addr.family(), SOCK_STREAM)
{
    connect(addr);
}

ssize_t SocketStream::sendBytes(const void* buffer, int length, int flags)
{
	const char* p = reinterpret_cast<const char*>(buffer);
	int remaining = length;
	ssize_t sent = 0;
	bool nonblock_enabled = isEnabled(SO_FLAG::NonBlock);
	while (remaining > 0)
	{
		ssize_t n = Socket::send(p, remaining, flags);
		p += n;
		sent += n;
		remaining -= n;
		if (!nonblock_enabled && remaining > 0)
			Poco::Thread::yield();
		else
			break;
	}
	return sent;
}

int SocketStream::sendBytes(FIFOBuffer& fifoBuf)
{
	ScopedLock<Mutex> l(fifoBuf.mutex());

	int ret = impl()->sendBytes(fifoBuf.begin(), (int) fifoBuf.used());
	if (ret > 0) fifoBuf.drain(ret);
	return ret;
}

} // end namespace alt
