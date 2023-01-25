#ifndef KEY_EVENTS_HDR
#define KEY_EVENTS_HDR

#include "PreComp.h"

#include "Event.h"

namespace Gust
{
    //This class handles the window key events. This is a interface class.
    class KeyEvent : public Event
    {
    public:
        int getKeyCode() const { return keyCode; }

    protected:
        //A key event is both an input event and a keyboard event.
        KeyEvent(int keyCode) : Event(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT), keyCode(keyCode)
        {
        }

        int keyCode;
    };

    //This class handles the window key pressed events.
    class PressedKeyEvent : public KeyEvent 
    {
    public:
        PressedKeyEvent(int keyCode, int repeats) : KeyEvent(keyCode), _repeatedNum(repeats)
        {
        }

        int getTimeseRepeated() const { return _repeatedNum; }

        //This is a debugging function which allows coders to see what this 
        //event does.
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "Key pressed: " << keyCode;// << " (" << _repeatedNum << " repeats)";
            return ss.str();
        }

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::KEY_PRESSED; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Key Pressed"; }
    private:
        int _repeatedNum;
    };

    //This class handles the window key released events.
    class ReleasedKeyEvent : public KeyEvent
    {
    public:
        ReleasedKeyEvent(int keyCode) : KeyEvent(keyCode)
        {
        }

        //This is a debugging function which allows coders to see what this 
        //event does.
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "Key released: " << keyCode;
            return ss.str();
        }

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::KEY_RELEASED; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Key Released"; }
    };

    //This class handles the window key typed events.
    class TypedKeyEvent : public KeyEvent
    {
    public:
        TypedKeyEvent(int keyCode) : KeyEvent(keyCode)
        {
        }

        //This is a debugging function which allows coders to see what this 
        //event does.
        std::string toString() const override
        {
            std::stringstream ss;
            ss << "Key pressed: " << keyCode;
            return ss.str();
        }

        //The static return is used to get the event type. Every instance will
        //share the same event type of this class. This is needed for the event
        //dispatcher to work correctly.
        static EventType getStaticType() { return EventType::KEY_TYPED; }
        virtual EventType getEventType() const override { return getStaticType(); };
        virtual const char* getName() const override { return "Key Pressed"; }
    };
}

#endif // !KEY_EVENTS_HDR
