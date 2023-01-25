#ifndef MOUSE_EVENTS_HDR
#define MOUSE_EVENTS_HDR

#include "PreComp.h"

#include "Event.h"

namespace Gust
{
    //This handles the movement of the mouse.
    class MouseMovedEvent : public Event
    {
    public:
        //This event belongs to both input and mouse events.
        MouseMovedEvent(float x, float y) : Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT),
            x(x), y(y) 
        {
        }

        //This is a debugging function which allows coders to see what this 
        //event does.
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "Mouses position: " << x << " , " << y;
            return ss.str();
        }

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static inline EventType getStaticType() { return EventType::MOVED_MOUSE; }
        inline EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Mouse Moved"; }
    public:
        float x;
        float y;
    };

    class MouseScrolledEvent : public Event
    {
    public:
        //This event belongs to both input and mouse events.
        //The y value is for the rare occasion that a user has a scroll whell
        //that goes left to right as well as up and down.
        MouseScrolledEvent(float x, float y) : Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT),
            x(x), y(y)
        {
        }

        //This is a debugging function which allows coders to see what this 
        //event does.
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "Mouses scroll position: " << x << " , " << y;
            return ss.str();
        }

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::MOUSED_SCROLLED; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Mouse Scrolled"; }
    public:
        float x;
        float y;
    };

    //This class is a interface class to mouse button events.
    class MouseButtonEvent : public Event
    {
    public:
        int getMouseEvent() const { return _button; }

    protected:
        //This event belongs to both input and mouse events.
        MouseButtonEvent(int button) : Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT | EVENT_CATEGORY_MOUSEBUTTON),
            _button(button)
        {
        }

        int _button;
    };

    //This class handles mouse pressed events.
    class MouseButtonEventPressed : public MouseButtonEvent
    {
    public:
        MouseButtonEventPressed(int button) : MouseButtonEvent(button)
        {
        }

        //This is a debugging function which allows coders to see what this 
        //event does.
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "Mouse button pressed state: " << _button;
            return ss.str();
        }

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::MOUSE_BUTTON_PRESSED; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Mouse button pressed"; }
    };

    //This class handles mouse released events.
    class MouseButtonEventReleased : public MouseButtonEvent
    {
    public:
        MouseButtonEventReleased(int button) : MouseButtonEvent(button)
        {
        }

        //This is a debugging function which allows coders to see what this 
        //event does.
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "Mouse button released state: " << _button;
            return ss.str();
        }

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::MOUSE_BUTTON_RELEASED; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Mouse button released"; }
    };

}

#endif // !MOUSE_EVENTS_HDR
