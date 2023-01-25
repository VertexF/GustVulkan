#ifndef EVENT_HDR
#define EVENT_HDR

#include "PreComp.h"

/*This file contains the event handler and dispatcher.*/

namespace Gust
{
    //These are the different types of events that can happen across the
    //engine.
    enum class EventType
    {
        None = 0,
        WINDOW_CLOSED, WINDOW_RESIZE, WINDOW_FOCUS, WINDOW_FOCUS_LOST, WINDOW_MOVED,
        APP_TICK, APP_UPDATE, APP_RENDER,
        KEY_PRESSED, KEY_RELEASED, KEY_TYPED,
        MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOVED_MOUSE, MOUSED_SCROLLED
    };

    //This allows use to categories the different events.
    enum EventCategory
    {
        //(1 << 0) magic numbers need to be refactored.
        None = 0,
        EVENT_CATEGORY_APPLICATION = (1 << 0),
        EVENT_CATEGORY_INPUT =       (1 << 1),
        EVENT_CATEGORY_KEYBOARD =    (1 << 2),
        EVENT_CATEGORY_MOUSE =       (1 << 3),
        EVENT_CATEGORY_MOUSEBUTTON = (1 << 4)
    };

    //This is the interface of what an event can look like.
    class Event
    {
    public:
        //The category is just a number but every other event need to tell 
        //the event handler what type of event it.
        Event(int category) : category(category)
        {
        }
        virtual ~Event() = default;

        bool isHandled = false;

        virtual EventType getEventType() const = 0;
        virtual const char* getName() const = 0;
        int getCategoryFlag() const { return category; }
        virtual std::string toString() const { return getName(); }

        bool isInCategory(EventCategory cat)
        {
            return getCategoryFlag() & cat;
        }

    protected:
        int category;
    };

    //The event dispatcher is a template pattern as we have an common interface
    //but depending on the event type we dispatch that even with a different
    //function.
    class EventDispatcher 
    {
    public:
        EventDispatcher(Event &events) : _event(events)
        {
        }

        //If they event type matches the one needing to be ran, then we run that 
        //event function.
        template<typename T, typename F>
        bool dispatch(const F &func)
        {
            if (_event.getEventType() == T::getStaticType()) 
            {
                _event.isHandled |= func(static_cast<T&>(_event));
                return true;
            }

            return false;
        }
    private:
        Event &_event;
    };

    //This allows our event dispatcher to print to the logger. Meant for 
    //debuggin only.
    inline std::ostream& operator<<(std::ostream& os, const Event& e) 
    {
        return os << e.toString();
    }
}

#endif // !EVENT_HDR