#ifndef LUNITY_UTILS_EVENT
#define LUNITY_UTILS_EVENT

#include <vector>
#include <functional>

template<class T, typename THolder = std::vector<std::function<T>>>
struct Event
{
    THolder Subscribers;
    Event() = default;
    Event(const THolder subscribers) {
        Subscribers = subscribers;
    }
    auto add(const std::function<T> subscriber) -> void {
        Subscribers.push_back(subscriber);
    }
    auto remove(const std::function<T> subscriber) -> void {
        int index = 0;
        for (int i = 0; i < Subscribers.size(); i++) {
            if (Subscribers[i] == subscriber) {
                index = i;
                goto Remove;
            }
        }
        return;
        Remove:
        Subscribers.erase(index);
    }
    auto addUnique(const std::function<T> subscriber) -> void {
        for (auto& s : Subscribers) {
            if (s == subscriber) return;
        }
        Add(subscriber);
    }
    auto removeAll(const std::function<T> subscriber) -> void {
        for (auto& s : Subscribers) {
            if (s == subscriber) Subscribers.erase(s);
        }
    }
    auto contains(const std::function<T> subscriber) -> bool const {
        for (auto& s : Subscribers) {
            if (s == subscriber) return true;
        }
        return false;
    }
    template<class... TArgs> auto invoke(TArgs&&... args) -> void const {
        for (auto& s : Subscribers) {
            s(std::forward<TArgs>(args)...);
        }
    }
    auto operator+=(const std::function<T> subscriber) -> Event<T>& {
        add(subscriber); return *this;
    }
    auto operator-=(const std::function<T> subscriber) -> Event<T>& {
        remove(subscriber); return *this;
    }
    template<class... TArgs> auto operator()(TArgs&&... args) -> void {
        invoke(std::forward<TArgs>(args)...);
    }
};


#endif //LUNITY_UTILS_EVENT
