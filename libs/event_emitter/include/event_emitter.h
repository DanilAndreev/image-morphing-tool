// Copyright (c) 2021-2021.
// License: CC0 1.0 Universal
// Permissions:
// - Commercial use
// - Modification
// - Distribution
// - Private use
//
// Limitations:
// - Liability
// - Trademark use
// - Patent use
// - Warranty
//
// Author: Danil Andreev | danssg08@gmail.com | https://github.com/DanilAndreev

#ifndef IMAGE_MORPHING_TOOL_EVENT_EMITTER_H
#define IMAGE_MORPHING_TOOL_EVENT_EMITTER_H

#include <string>
#include <map>
#include <unordered_set>
#include <functional>

namespace events {
    class event_emitter;

    /**
     * event_base - base abstract class for events.
     * Has propagation setting and abstract copy method.
     * @class
     */
    class event_base {
        friend event_emitter;
    private:
        /// _propagation_stopped - it true, event will stop propagation after last handler.
        bool _propagation_stopped;
    public:
        event_base() noexcept {
            _propagation_stopped = false;
        }

        virtual ~event_base() noexcept = default;

    public:
        /**
         * Stops event propagation.
         * Next handlers in chain won't be executed.
         */
        void stop_propagation() noexcept {
            this->_propagation_stopped = true;
        }
    };

    /**
     * event_emitter - class for dispatching events over the handlers.
     * @class
     */
    class event_emitter {
    public:
        /// event_handler_t - event handler function prototype.
//        typedef void(*event_handler_t)(event_base &);
        using event_handler_t = std::function<void(event_base &)>;
    private:
        /// subscribers - event subscribers. Sorted by event names.
        std::map<std::string, std::unordered_set<event_handler_t*>> subscribers;
    public:
        event_emitter() = default;
        ~event_emitter() = default;
    protected:
        /**
         * emit - emits new event and propagates it over the handlers.
         * @param eventName - Event name string.
         * @param event - Event object. Each handler can modify it while handling.
         */
        void emit_event(const std::string &eventName, event_base &event) {
            auto eventSubscribers = this->subscribers.find(eventName);
            if (eventSubscribers == this->subscribers.end()) return;
            for (event_handler_t* handler : eventSubscribers->second) {
                if (event._propagation_stopped) break;
                (*handler)(event);
            }
        }

    public:
        /**
         * add_listener - binds handler function to event.
         * @param eventName - Event name string.
         * @param handler - Handler function pointer.
         */
        void add_listener(const std::string &eventName, event_handler_t* handler) {
            auto iterator = this->subscribers.find(eventName);
            if (iterator == this->subscribers.end()) {
                std::unordered_set<event_handler_t*> handlers;
                handlers.insert(handler);
                this->subscribers.insert(std::make_pair(eventName, handlers));
            } else {
                iterator->second.insert(handler);
            }
        }

        /**
         * remove_listener - unbinds handler function from the event.
         * @param eventName - Event name string.
         * @param handler - Handler function pointer.
         */
        void remove_listener(const std::string &eventName, event_handler_t* handler) {
            std::unordered_set<event_handler_t*> &eventSubscribers = this->subscribers[eventName];
            eventSubscribers.extract(handler);
        }
    };
}

#endif //IMAGE_MORPHING_TOOL_EVENT_EMITTER_H
