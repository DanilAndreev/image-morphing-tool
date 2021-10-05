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

        [[nodiscard]] virtual event_base *copy() const noexcept = 0;
    };

    /**
     * event_emitter - class for dispatching events over the handlers.
     * @class
     */
    class event_emitter {
    public:
        /// EventHandler - event handler function prototype.
        typedef void(*EventHandler)(event_base &);
    private:
        /// subscribers - event subscribers. Sorted by event names.
        std::map<std::string, std::unordered_set<EventHandler>> subscribers;
    public:
        event_emitter() = default;
        ~event_emitter() = default;
    public:
        /**
         * emit - emits new event and propagates it over the handlers.
         * @param eventName - Event name string.
         * @param event - Event object. Each handler can modify it while handling.
         */
        void emit_event(const std::string &eventName, const event_base &event) {
            auto eventSubscribers = this->subscribers.find(eventName);
            if (eventSubscribers == this->subscribers.end()) return;
            event_base *eventCopy = event.copy();
            for (EventHandler handler : eventSubscribers->second) {
                if (event._propagation_stopped) break;
                handler(*eventCopy);
            }
            delete eventCopy;
        }

        /**
         * add_listener - binds handler function to event.
         * @param eventName - Event name string.
         * @param handler - Handler function pointer.
         */
        void add_listener(const std::string &eventName, EventHandler handler) {
            auto iterator = this->subscribers.find(eventName);
            if (iterator == this->subscribers.end()) {
                std::unordered_set<EventHandler> handlers;
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
        void remove_listener(const std::string &eventName, EventHandler handler) {
            std::unordered_set<EventHandler> &eventSubscribers = this->subscribers[eventName];
            eventSubscribers.extract(handler);
        }
    };
}

#endif //IMAGE_MORPHING_TOOL_EVENT_EMITTER_H
