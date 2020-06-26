
#pragma once

#include "OS.h"                         // for FdId (File Descriptor type)
#include <util/Defs.h>                  // for ALT_UTIL_PUBLIC
#include <util/types/EnumSet.h>         // for EnumSet
#include <util/types/OpaquePointer.h>   // for OpaquePointer
#include <util/types/Clock.h>           // for Clock::duration
#include <util/types/TemplateHelper.h>  // for NONCOPYABLE

#include <vector>                       // for vector

namespace alt {

/**
 * \class EventPoller
 * \ingroup Util
 * \brief Abstract Event Poller Interface
 */
class EventPoller
{
  public:
    virtual void poll(Clock::tick_type tick_realtime, Clock::tick_type poll_timeout) =0;
};

/**
 * \enum FDEventId
 * \ingroup Util
 * \brief File descriptor event ids
 */
enum class FDEventId : uint8_t
{
    EVENT_IN,
    EVENT_OUT,
    EVENT_ERROR,
};

/**
 * \class FDEventIdSet
 * \ingroup Util
 * \brief File descriptor event id set
 */
using FDEventIdSet = EnumSet<FDEventId, BitSet<uint32_t>>;

/**
 * \class FDEventHandler
 * \ingroup Util
 * \brief File descriptor event handler interface used by FDEventPoller
 */
class FDEventHandler
{
  public:
    virtual FdId fd() const =0;
    virtual FDEventIdSet onEvent(Clock::tick_type tick_realtime, FDEventIdSet event_ids) = 0;
    virtual ~FDEventHandler() {}
};

/**
 * \struct FDEventPoller
 * \ingroup Util
 * \brief Implements an event poller for system (file descriptor) events. It uses epoll if
 * supported (linux), or poll if epoll is not supported in other operating system
 */
class ALT_UTIL_PUBLIC FDEventPoller : public EventPoller
{
  public:

    NONCOPYABLE(FDEventPoller);

    struct Event
    {
        FDEventHandler*    handler_;
        FDEventIdSet       event_ids_;
    };

	using Events = std::vector<Event>;

    /// \brief constructs an empty FDEventPoller.
	FDEventPoller(bool busy_poller = false);
		
    /// \brief destroys the FDEventPoller
	~FDEventPoller();

    /// \brief registers an event handler for polling the given event ids
    /// if the handler is already registered, modify event ids for it
	void book(FDEventHandler* handler, FDEventIdSet event_ids);

    /// \brief remove the event handler
	void remove(const FDEventHandler* handler);

    /// \return true if the event handler is int registered map
	//bool has(const FDEventHandler* handler) const;

    /// \return true if no event handler is registered
	bool empty() const;

    /// \return removes all registered event handlers
	void clear();

    /// \brief waits until the state of one of the registered events
	/// changes or the timeout expires.
    /// \param tick_realtime nano secs since epoch in realtime
    /// \param timeout timeout in nano secs for polling. Non-blocking if zero
    /// \return a set of events has changed
	void poll(Clock::tick_type tick_realtime, Clock::tick_type timeout) override;

  private:
    class Impl;
	OpaquePointer<Impl> impl_;
};

}

