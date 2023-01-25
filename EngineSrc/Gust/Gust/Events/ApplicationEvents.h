#ifndef APPLICTION_EVENTS_HDR
#define APPLICTION_EVENTS_HDR

#include "PreComp.h"

#include "Event.h"

namespace Gust
{
    //This class handles the window resize events
    class WindowResizeEvent : public Event
    {
    public:
        //Height and width are the new size of the resized event.
        //The constructure of the event class is taking in the Event category
        //this specific event belongs to.
        WindowResizeEvent(uint32_t width, uint32_t height) : Event(EVENT_CATEGORY_APPLICATION),
            _width(width), _height(height)
        {
        }

        //This is a debugging function which allows coders to see what this 
        //event does.
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "Window resize state: " << _width << " , " << _height;
            return ss.str();
        }

        uint32_t getWidth() const { return _width; }
        uint32_t getHeight() const { return _height; }

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::WINDOW_RESIZE; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Window resize"; }
    private:
        uint32_t _width;
        uint32_t _height;
    };

    //This class handles the window close events
    class WindowClosedEvent : public Event
    {
    public:
        //The constructure of the event class is taking in the Event category
        //this specific event belongs to.
        WindowClosedEvent() : Event(EVENT_CATEGORY_APPLICATION)
        {}

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::WINDOW_CLOSED; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Window close"; }
    };

    //This class handles the window update events. Current these don't 
    //do anything.
    //TODO: Expand the events functionality.
    class AppUpdateEvent : public Event
    {
    public:
        //The constructure of the event class is taking in the Event category
        //this specific event belongs to.
        AppUpdateEvent() : Event(EVENT_CATEGORY_APPLICATION)
        {}

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::APP_UPDATE; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Application update"; }
    };

    //This class handles the window frames/timing events. 
    //Current these don't do anything.
    //TODO: Expand the events functionality.
    class AppTickEvent : public Event
    {
    public:
        AppTickEvent() : Event(EVENT_CATEGORY_APPLICATION)
        {}

        static EventType getStaticType() { return EventType::APP_TICK; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Application tick"; }
    };

    //This class handles the window render events. 
    //Current these don't do anything.
    //TODO: Expand the events functionality.
    class AppRenderEvent : public Event
    {
    public:
        //The constructure of the event class is taking in the Event category
        //this specific event belongs to.
        AppRenderEvent() : Event(EVENT_CATEGORY_APPLICATION)
        {}

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::APP_RENDER; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Window render"; }
    };
}


#endif // !APPLICTION_EVENTS_HDR
